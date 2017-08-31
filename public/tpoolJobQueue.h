/*
    Copyright 2017 Krzysztof Karbowiak
*/

#ifndef TPOOL_JOB_QUEUE_H
#define TPOOL_JOB_QUEUE_H

#include "tpoolJobType.h"

#include <list>
#include <mutex>
#include <condition_variable>
#include <cstddef> // std::size_t


namespace tpool
{
    class JobQueue
    {
        public:
            explicit JobQueue(std::size_t guaranteed_capacity);

            void addJob(job_type job);
            void addJobGuaranteed(job_type job);

            job_type getJob();

            void clear();

        private:
            std::list<job_type> mJobs;
            std::list<job_type> mGuaranteed;
            std::size_t mGuaranteedCapacity;
            std::mutex mMutex;
            std::condition_variable mCondVar;
    };
}

#endif /* TPOOL_JOB_QUEUE_H */

