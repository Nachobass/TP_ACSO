// /**
//  * File: thread-pool.cc
//  * --------------------
//  * Presents the implementation of the ThreadPool class.
//  */


#include "thread-pool.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads)
    : wts(numThreads),
      numAvailableWorkers(numThreads),
      dispatcherWakeup(0),
      done(false),
      totalTasks(0)
{
    // creo el hilo del dispatcher
    dt = thread([this] { dispatcher(); });

    // creo los hilos de los workers
    for( size_t i = 0; i < wts.size(); ++i ){
        wts[i].ts = thread([this, i] { worker(i); });
    }
}



void ThreadPool::schedule(const function<void(void)>& thunk) {
    {
        lock_guard<mutex> lg(destroyedLock);
        if( destroyed_ ) throw runtime_error("Cannot schedule on destroyed ThreadPool");
    }

    if( !thunk ) throw invalid_argument("Scheduled function cannot be nullptr");

    // verifico si el hilo que llama es un worker
    bool isWorker = isWorkerThread();

    if( isWorker ) {
        // si esta adentro de un worker, ejecuta la tarea inline (evita deadlock)
        {
            lock_guard<mutex> lock(countLock);
            totalTasks++;
        }

        thunk();  // ejecutamos inline

        {
            lock_guard<mutex> lock(countLock);
            totalTasks--;
            if( totalTasks == 0 ) allDone.notify_all();
        }
        return;
    }

    // si no es worker, agrego la tarea a la cola y despacho al dispatcher
    {
        lock_guard<mutex> lock(queueLock);
        tasks.push(thunk);
        dispatcherWakeup.signal();
    }

    {
        lock_guard<mutex> lock(countLock);
        totalTasks++;
    }
}




void ThreadPool::dispatcher() {
    while( true ){
        dispatcherWakeup.wait(); // espero a que el dispatcher me avise

        if( done && tasks.empty() ) break;

        function<void(void)> nextTask;
        {
            lock_guard<mutex> lg(queueLock);
            if( !tasks.empty() ){
                nextTask = tasks.front();
                tasks.pop();
            }else{
                continue;
            }
        }

        // busco un worker disponible
        bool assigned = false;
        while( !assigned ){
            for( size_t i = 0; i < wts.size(); ++i ){
                lock_guard<mutex> lock(wts[i].thunkMutex);  // protejo el acceso a thunk y available

                if( wts[i].available ){
                    wts[i].available = false;
                    wts[i].thunk = nextTask;
                    wts[i].ready.signal(); // aviso al worker q tiene una tarea
                    assigned = true;

                    {
                        lock_guard<mutex> lock(countLock);
                        numAvailableWorkers--;
                    }
                    break;
                }
            }
            // si ningun worker estaba libre, espero a que uno este disponible
            if( !assigned ){
                unique_lock<mutex> lock(queueLock);
                workerAvailable.wait(lock);
            }
        }
    }
}


void ThreadPool::worker(int id) {
    while( true ){
        wts[id].ready.wait(); // espero a que el dispatcher me avise

        if( done ) break;

        if( wts[id].thunk ){
            wts[id].thunk(); // ejecuto la tarea
            {
                lock_guard<mutex> lock(wts[id].thunkMutex);
                wts[id].thunk = nullptr;
                wts[id].available = true;
            }
            workerAvailable.notify_one(); // aviso al dispatcher q esta disponible
        }

        {
            lock_guard<mutex> lock(countLock);
            totalTasks--;
            numAvailableWorkers++;
            if( totalTasks == 0 ){
                allDone.notify_all(); // aviso a todos los hilos q todas las tareas se completaron
            }
        }
    }
}



bool ThreadPool::isWorkerThread() {
    thread::id this_id = this_thread::get_id();
    for( auto& w : wts ){
        if( w.ts.get_id() == this_id ){
            return true; // si el hilo que llama es un worker, retorna true
        }
    }
    return false;
}



void ThreadPool::wait() {
    unique_lock<mutex> ul(countLock);

    if( isWorkerThread() ){
        // si es un worker, ejecuto inline tareas mientras tenga
        ul.unlock();
        while( true ){
            function<void()> next;
            {
                lock_guard<mutex> ql(queueLock);
                if( tasks.empty() ) break;
                next = tasks.front();
                tasks.pop();
            }
            next();  // ejecuto tarea
            {
                lock_guard<mutex> lock(countLock);
                totalTasks--;
                if( totalTasks == 0 ) allDone.notify_all(); // aviso si es la ultima tarea
            }
        }
        return;
    }else{
        // si no es un worker, espero hasta qu no haya tareas pendientes
        allDone.wait(ul, [this] { return totalTasks == 0; });
    }
}




ThreadPool::~ThreadPool() {
    wait(); // espero a que todas las tareas se completen

    // marco el pool como destruido
    {
        lock_guard<mutex> lg(destroyedLock);
        destroyed_ = true;
    }

    done = true; // señalo a dispatcher y workers que deben finalizar

    dispatcherWakeup.signal();  

    for(auto& w : wts ){
        w.ready.signal(); 
    }

    if( dt.joinable() ) dt.join(); // espero a que el dispatcher termine

    for( auto& w : wts )
        if (w.ts.joinable()) w.ts.join(); // espero a que los workers terminen
}