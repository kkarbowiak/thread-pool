#ifndef TPOOL_THREAD_POOL_H
#define TPOOL_THREAD_POOL_H

#include "tpoolCommandQueue.h"

#include <thread>

#include <cstddef> // std::size_t


namespace tpool { class Job; }

namespace tpool
{
    class ThreadPool
    {
        public:
            explicit ThreadPool(std::size_t num_workers);
            ~ThreadPool();

            void addJob(std::unique_ptr<Job> job);

        private:
            ThreadPool(ThreadPool const &) = delete;
            ThreadPool & operator=(ThreadPool const &) = delete;

        private:
            CommandQueue mCommandQueue;
            std::thread * mWorkersArray;
            std::size_t mWorkersNumber;
    };
}

#endif /* TPOOL_THREAD_POOL_H */

