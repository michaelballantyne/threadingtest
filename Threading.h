#ifndef NEBO_THREADING_H
#define NEBO_THREADING_H

#include <boost/atomic.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <stdexcept>
#include <string>
#include <sched.h>

class Threading {

    public:
        static void setup(int nthreads) {
            instance = new Threading(nthreads);
        }

        static Threading & getInstance() {
            return *instance;
        }

        void scheduleTasks(std::vector<boost::function0<void> > & tasks) {
            //if (tasks.size() != nthreads) {
            //throw std::invalid_argument("ntasks must match nthreads");
            //}

            for (int i = 1; i < nthreads; i++) {
                tasksArray[i]->store(&tasks[i], boost::memory_order_relaxed);
            }

            remaining.store(nthreads - 1, boost::memory_order_relaxed);

            atomic_thread_fence(boost::memory_order_release);

            tasks[0]();

            while(remaining.load(boost::memory_order_acquire) > 0) {
                __asm__ __volatile__ ("pause;");
                // Spin until done
            }
        }

        const int nthreads;

    private:
        static Threading *instance;

        // Make sure the only way to construct one is via getInstance
        Threading(const Threading & other);

        boost::atomic<boost::function0<void> *> **tasksArray;
        boost::thread **threadsArray;
        boost::atomic<int> remaining;

        // (Private) constructor taking thread count.
        Threading(int nthreadsArg) : nthreads(nthreadsArg), remaining(0) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(0, &cpuset);
            sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

            // Create atomics for communication with threads
            tasksArray = new boost::atomic<boost::function0<void> *> *[nthreads];
            for (int i = 1; i < nthreads; i++) {
                tasksArray[i] = new boost::atomic<boost::function0<void> *>(NULL);
            }

            // Start threads
            threadsArray = new boost::thread *[nthreads];
            for (int i = 1; i < nthreads; i++) {
                threadsArray[i] = new boost::thread(boost::bind(&Threading::threadBody, this, i));
            }
        }

        // I probably don't need this whole thing at all.
        ~Threading() {

            // Interrupt and threads and wait for them to die.
            for (int i = 1; i < nthreads; i++) {
                threadsArray[i]->interrupt();
            }

            for (int i = 1; i < nthreads; i++) {
                threadsArray[i]->join();
                delete threadsArray[i];
            }

            delete threadsArray;

            // Then we delete the boost::atomics we allocated.
            for (int i = 1; i < nthreads; i++) {
                delete tasksArray[i];
            }

            delete tasksArray;
        }

        void threadBody(int threadId) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(threadId, &cpuset);
            sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

            while (true) {
                boost::function0<void> *task = NULL;
                while((task = tasksArray[threadId]->load(boost::memory_order_relaxed)) == NULL) {
                    __asm__ __volatile__ ("pause;");
                    // Wait for work
                }

                boost::atomic_thread_fence(boost::memory_order_acquire);

                // Execute task
                (*task)();

                // Report back
                tasksArray[threadId]->store(NULL, boost::memory_order_relaxed);

                remaining.fetch_sub(1, boost::memory_order_release);

                //boost::this_thread::interruption_point();
            }
        }
};

Threading * Threading::instance = NULL;

#endif

