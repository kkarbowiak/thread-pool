#ifndef TPOOL_THREAD_POOL_H
#define TPOOL_THREAD_POOL_H

#include "tpoolCommandQueue.h"

#include <memory> // std::unique_ptr
#include <cstddef> // std::size_t


namespace tpool
{
    class Job;
    class Worker;
}

namespace tpool
{
    class ThreadPool
    {
        public:
            explicit ThreadPool(std::size_t num_workers);
            ~ThreadPool();

            void addJob(std::unique_ptr<Job> job);

            void clearPendingJobs();
            void waitUntilJobsCompleted();

        private:
            ThreadPool(ThreadPool const &) = delete;
            ThreadPool & operator=(ThreadPool const &) = delete;

        private:
            CommandQueue mCommandQueue;
            std::unique_ptr<Worker[]> mWorkers;
            std::size_t mWorkersNumber;
    };
}

#endif /* TPOOL_THREAD_POOL_H */

