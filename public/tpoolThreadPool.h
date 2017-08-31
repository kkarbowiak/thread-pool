#ifndef TPOOL_THREAD_POOL_H
#define TPOOL_THREAD_POOL_H

#include "tpoolJobType.h"
#include "tpoolJobQueue.h"

#include <memory> // std::unique_ptr
#include <cstddef> // std::size_t


namespace tpool
{
    class Worker;
}

namespace tpool
{
    class ThreadPool
    {
        public:
            explicit ThreadPool(std::size_t num_workers);
            ~ThreadPool();

            void addJob(job_type job);

            void clearPendingJobs();
            void waitUntilJobsCompleted();

            ThreadPool(ThreadPool const &) = delete;
            ThreadPool & operator=(ThreadPool const &) = delete;

        private:
            JobQueue mJobQueue;
            std::unique_ptr<Worker[]> mWorkers;
            std::size_t mWorkersNumber;
    };
}

#endif /* TPOOL_THREAD_POOL_H */

