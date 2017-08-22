#ifndef TPOOL_JOB_QUEUE_H
#define TPOOL_JOB_QUEUE_H

#include <functional> // std::function
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

            void addJob(std::function<void ()> job);
            void addJobGuaranteed(std::function<void ()> job);

            std::function<void ()> getJob();

            void clear();

        private:
            std::list<std::function<void ()>> mJobs;
            std::list<std::function<void ()>> mGuaranteed;
            std::size_t mGuaranteedCapacity;
            std::mutex mMutex;
            std::condition_variable mCondVar;
    };
}

#endif /* TPOOL_JOB_QUEUE_H */

