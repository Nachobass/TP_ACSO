/**
 * File: tpcustomtest.cc
 * ---------------------
 * Unit tests *you* write to exercise the ThreadPool in a variety
 * of ways.
 */


// de concurrencia, funcionalidad, de errores, stress
#include <iostream>
#include <sstream>
#include <atomic>
#include <chrono>
#include <cassert>
#include <map>
#include <string>
#include <functional>
#include <cstring>
#include <mutex>
#include <sys/types.h> // used to count the number of threads
#include <unistd.h>    // used to count the number of threads
#include <dirent.h>    // for opendir, readdir, closedir

#include "thread-pool.h"


// using namespace std;
using namespace std::chrono;

void sleep_for(int slp){
    this_thread::sleep_for(chrono::milliseconds(slp));
}

static mutex oslock;

static const size_t kNumThreads = 4;
static const size_t kNumFunctions = 10;

static void simpleTest() {
  ThreadPool pool(kNumThreads);
  for (size_t id = 0; id < kNumFunctions; id++) {
    pool.schedule([id] {
      oslock.lock();
      cout << "Thread (ID: " << id << ") has started." << endl;
      oslock.unlock();
      size_t sleepTime = (id % 3) * 10;
      sleep_for(sleepTime);
      oslock.lock();
      cout <<  "Thread (ID: " << id << ") has finished." << endl ;
      oslock.unlock();
    });
  }

  pool.wait();

cout << "[Test --simple-test completed ✅]" << endl;
}


static void singleThreadNoWaitTest() {
    ThreadPool pool(4);

    pool.schedule([&] {
        oslock.lock();
        cout << "This is a test." << endl;
        oslock.unlock();
    });
    sleep_for(1000); // emulate wait without actually calling wait (that's a different test)
    cout << "[Test --single-thread-no-wait completed ✅]" << endl;
}

static void singleThreadSingleWaitTest() {
    ThreadPool pool(4);
    pool.schedule([] {
        oslock.lock();
        cout << "This is a test." << endl;
        oslock.unlock();
        sleep_for(1000);
    });
    cout << "[Test --single-thread-single-wait completed ✅]" << endl;
}

static void noThreadsDoubleWaitTest() {
    ThreadPool pool(4);
    pool.wait();
    pool.wait();
    cout << "[Test --no-threads-double-wait completed ✅]" << endl;
}

static void reuseThreadPoolTest() {
    ThreadPool pool(4);
    for (size_t i = 0; i < 16; i++) {
        pool.schedule([] {
            oslock.lock();
            cout << "This is a test." << endl;
            oslock.unlock();
            sleep_for(50);
        });
    }
    pool.wait();
    pool.schedule([] {
        oslock.lock();
        cout << "This is a code." << endl;
        oslock.unlock();
        sleep_for(1000);
    }); 
    pool.wait();
    cout << "[Test --reuse-thread-pool completed ✅]" << endl;
}

void parallelismTest() {
    atomic<int> counter(0);

    auto start = high_resolution_clock::now();

    vector<thread> threads;
    for (int i = 0; i < 8; i++) {
        threads.emplace_back([&] {
            this_thread::sleep_for(milliseconds(1000));
            counter++;
        });
    }

    for (auto& t : threads) t.join();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "Time: " << duration << "ms, Counter: " << counter << endl;
    cout << "[Test --parallelism completed ✅]" << endl;
}

static void exceptionTest() {
    ThreadPool pool(4);
    for (int i = 0; i < 4; ++i) {
        pool.schedule([i] {
            try {
                if (i == 2) throw runtime_error("Task throws!");
            } catch (const exception& e) {
                oslock.lock();
                cout << "[Caught exception in thread]: " << e.what() << endl;
                oslock.unlock();
            }
        });
    }
    pool.wait();
    cout << "[Test --exception completed ✅]" << endl;
}


static void stressTest() {
    ThreadPool pool(8);
    atomic<int> counter(0);
    for (int i = 0; i < 1000; i++) {
        pool.schedule([&counter] {
            counter++;
        });
    }
    pool.wait();
    cout << "[stressTest] Completed " << counter << " tasks." << endl;
    if (counter == 1000) {
        cout << "[Test --stress completed ✅]" << endl;
    } else {
        cout << "[Test --stress failed ❌]" << endl;
    }
}

static void zeroTasksTest() {
    ThreadPool pool(4);
    pool.wait();  // No tareas
    cout << "[Test --zero-tasks completed ✅]" << endl;
}

static void sequentialWaitTest() {
    ThreadPool pool(4);
    atomic<int> counter(0);

    for (int i = 0; i < 4; ++i) {
        pool.schedule([&counter] {
            sleep_for(100);
            counter++;
        });
    }
    pool.wait();

    for (int i = 0; i < 4; ++i) {
        pool.schedule([&counter] {
            sleep_for(100);
            counter++;
        });
    }
    pool.wait();

    if (counter == 8) {
        cout << "[Test --sequential-wait completed ✅]" << endl;
    } else {
        cout << "[Test --sequential-wait failed ❌]" << endl;
    }
}

static void raceConditionTest() {
    ThreadPool pool(8);
    int counter = 0;
    mutex mtx;
    for (int i = 0; i < 1000; ++i) {
        pool.schedule([&] {
            lock_guard<mutex> lock(mtx);
            ++counter;
        });
    }
    pool.wait();
    assert(counter == 1000);
    cout << "[Test --race completed ✅]" << endl;
}

static void saturationTest() {
    ThreadPool pool(2); // pocos hilos
    atomic<int> counter(0);
    for (int i = 0; i < 50; ++i) {
        pool.schedule([&] {
            this_thread::sleep_for(chrono::milliseconds(10));
            ++counter;
        });
    }
    pool.wait();
    assert(counter == 50);
    cout << "[Test --saturation completed ✅]" << endl;
}

static void stressTest2() {
    ThreadPool pool(8);
    atomic<int> counter(0);
    const int N = 100000;
    for (int i = 0; i < N; ++i) {
        pool.schedule([&] {
            ++counter;
        });
    }
    pool.wait();
    assert(counter == N);
    cout << "[Test --stress completed ✅]" << endl;
}

static void multiExceptionTest() {
    ThreadPool pool(4);
    for (int i = 0; i < 10; ++i) {
        pool.schedule([i] {
            try {
                if (i % 3 == 0)
                    throw runtime_error("Boom " + to_string(i));
            } catch (const exception& e) {
                oslock.lock();
                cout << "[Caught exception]: " << e.what() << endl;
                oslock.unlock();
            }
        });
    }
    pool.wait();
    cout << "[Test --multiexception completed ✅]" << endl;
}

static void reuseTest() {
    ThreadPool pool(4);
    atomic<int> sum(0);

    for (int j = 0; j < 10; ++j) {
        for (int i = 0; i < 100; ++i) {
            pool.schedule([&] {
                sum += 1;
            });
        }
        pool.wait(); // esperar antes de nuevo ciclo
    }

    assert(sum == 1000);
    cout << "[Test --reuse completed ✅]" << endl;
}

static void emptyDestructionTest() {
    {
        ThreadPool pool(4);
        // No schedule()
    }
    cout << "[Test --empty completed ✅]" << endl;
}

static void mixedLatencyTest() {
    ThreadPool pool(4);
    atomic<int> counter(0);
    for (int i = 0; i < 20; ++i) {
        pool.schedule([&] {
            if (i % 5 == 0)
                this_thread::sleep_for(chrono::milliseconds(50));
            ++counter;
        });
    }
    pool.wait();
    assert(counter == 20);
    cout << "[Test --latency completed ✅]" << endl;
}

static void earlyShutdownTest() {
    ThreadPool pool(4);
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }
    // El destructor del pool debería esperar correctamente sin errores
    std::cout << "[earlyShutdownTest] OK\n";
}

static void continuousLoadTest() {
    ThreadPool pool(4);
    atomic<int> counter{0};
    for (int i = 0; i < 1000; ++i) {
        pool.enqueue([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            counter++;
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    assert(counter == 1000);
    std::cout << "[continuousLoadTest] OK\n";
}

static void recursiveEnqueueTest() {
    ThreadPool pool(4);
    atomic<int> count{0};
    pool.enqueue([&]() {
        count++;
        pool.enqueue([&]() { count++; });
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(count == 2);
    std::cout << "[recursiveEnqueueTest] OK\n";
}


struct testEntry {
    string flag;
    function<void(void)> testfn;
};

static void buildMap(map<string, function<void(void)>>& testFunctionMap) {
    testEntry entries[] = {
        {"--single-thread-no-wait", singleThreadNoWaitTest},
        {"--single-thread-single-wait", singleThreadSingleWaitTest},
        {"--no-threads-double-wait", noThreadsDoubleWaitTest},
        {"--reuse-thread-pool", reuseThreadPoolTest},
        {"--s", simpleTest},
        {"--parallelism", parallelismTest},
        {"--exception", exceptionTest},
        {"--stress", stressTest},
        {"--zero-tasks", zeroTasksTest},
        {"--sequential-wait", sequentialWaitTest},
        {"--race", raceConditionTest},
        {"--saturation", saturationTest},
        {"--stress2", stressTest2},
        {"--multiexception", multiExceptionTest},
        {"--reuse", reuseTest},
        {"--empty", emptyDestructionTest},
        {"--latency", mixedLatencyTest},
        {"--early-shutdown", earlyShutdownTest},
        {"--continuous-load", continuousLoadTest},
        {"--recursive-enqueue", recursiveEnqueueTest},
    };

    for (const testEntry& entry: entries) {
        testFunctionMap[entry.flag] = entry.testfn;
    }
}

static void executeAll(const map<string, function<void(void)>>& testFunctionMap) {
    for (const auto& entry: testFunctionMap) {
        cout << entry.first << ":" << endl;
        entry.second();
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Ouch! I need exactly two arguments." << endl;
        return 0;
    }

    map<string, function<void(void)>> testFunctionMap;
    buildMap(testFunctionMap);
    string flag = argv[1];
    if (flag == "--all") {
        executeAll(testFunctionMap);
        return 0;
    }
    auto found = testFunctionMap.find(argv[1]);
    if (found == testFunctionMap.end()) {
        cout << "Oops... we don't recognize the flag \"" << argv[1] << "\"." << endl;
        return 0;
    }

    found->second();
    return 0;
}
