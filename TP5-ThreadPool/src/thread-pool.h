// /**
//  * File: thread-pool.h
//  * -------------------
//  * This class defines the ThreadPool class, which accepts a collection
//  * of thunks (which are zero-argument functions that don't return a value)
//  * and schedules them in a FIFO manner to be executed by a constant number
//  * of child threads that exist solely to invoke previously scheduled thunks.
//  */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>     // for size_t
#include <functional>  // for the function template used in the schedule signature
#include <thread>      // for thread
#include <vector>      // for vector
#include <queue>       
#include <mutex>       
#include <condition_variable> 
#include "Semaphore.h" // for Semaphore
#include <atomic>


using namespace std;

/**
 * @brief Represents a worker in the thread pool.
 *
 * The `worker_t` struct contains information about a worker
 * thread in the thread pool.
 */
typedef struct worker {
    worker() : available(true), ready(0) {}  // Constructor por defecto

    thread ts;                         // worker thread
    function<void(void)> thunk;        // tarea a ejecutar
    bool available;                    // flag de disponibilidad
    Semaphore ready;                   // semaforo para señal de despacho
    mutex thunkMutex;                  // mutex para thunk

    worker(const worker&) = delete;
    worker& operator=(const worker&) = delete;
} worker_t;


class ThreadPool {
  public:

    /**
     * Constructs a ThreadPool configured to spawn up to the specified
     * number of threads.
     */
    ThreadPool(size_t numThreads);

    /**
     * Schedules the provided thunk (a zero-argument void function)
     * to be executed by one of the ThreadPool's threads as soon as
     * all previously scheduled thunks have been handled.
     */
    void schedule(const function<void(void)>& thunk);

    /**
     * Blocks and waits until all previously scheduled thunks
     * have been executed in full.
     */
    void wait();

    /**
     * Waits for all previously scheduled thunks to execute, and then
     * properly brings down the ThreadPool and any resources tapped
     * over the course of its lifetime.
     */
    ~ThreadPool();

  private:
    void dispatcher();                  // dispatcher loop
    void worker(int id);               // worker loop
    bool isWorkerThread();                 // identifica si el thread actual es uno de los workers

    thread dt;                                     // dispatcher thread
    vector<worker_t> wts;                        // worker thread info
    queue<function<void(void)>> tasks;             // task queue

    mutex queueLock;                               // protege la cola de tareas
    condition_variable_any taskAvailable;         // avisa al dispatcher de nueva tarea

    size_t numAvailableWorkers;        
    Semaphore dispatcherWakeup;      

    atomic<bool> done;

    bool destroyed_ = false;           

    mutex destroyedLock;
    atomic<size_t> totalTasks;

    mutex countLock;                   // proteje totalTasks y numAvailableWorkers

    condition_variable_any allDone;   // espera a q todas las tares se completn

    condition_variable_any workerAvailable;

    
    ThreadPool(const ThreadPool& original) = delete;
    ThreadPool& operator=(const ThreadPool& rhs) = delete;
};

#endif







