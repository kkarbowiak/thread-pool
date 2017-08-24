#include "tpoolJobQueue.h"

#include <utility> // std::move()


namespace
{
    void discardFrontElement(std::list<tpool::job_type> & jobs, std::list<tpool::job_type> & guaranteed, std::size_t guaranteed_capacity);
}

namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
JobQueue::JobQueue(std::size_t guaranteed_capacity)
    : mJobs()
    , mGuaranteed()
    , mGuaranteedCapacity(guaranteed_capacity)
    , mMutex()
    , mCondVar()
{
    for (std::size_t i = 0; i < guaranteed_capacity; ++i)
    {
        mGuaranteed.push_back(job_type());
    }
}
////////////////////////////////////////////////////////////////////////////////
void JobQueue::addJob(job_type job)
{
    std::unique_lock<std::mutex> lock(mMutex);

    mJobs.push_back(std::move(job));

    mCondVar.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
void JobQueue::addJobGuaranteed(job_type job)
{
    std::unique_lock<std::mutex> lock(mMutex);

    auto it = mGuaranteed.begin();
    *it = std::move(job);
    mJobs.splice(mJobs.end(), mGuaranteed, it);

    mCondVar.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
job_type JobQueue::getJob()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while (mJobs.empty())
    {
        mCondVar.wait(lock);
    }

    job_type job = std::move(mJobs.front());
    discardFrontElement(mJobs, mGuaranteed, mGuaranteedCapacity);

    return job;
}
////////////////////////////////////////////////////////////////////////////////
void JobQueue::clear()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while (!mJobs.empty())
    {
        discardFrontElement(mJobs, mGuaranteed, mGuaranteedCapacity);
    }
}
////////////////////////////////////////////////////////////////////////////////
}

namespace
{
////////////////////////////////////////////////////////////////////////////////
void discardFrontElement(std::list<tpool::job_type> & jobs, std::list<tpool::job_type> & guaranteed, std::size_t guaranteed_capacity)
{
    if (guaranteed.size() < guaranteed_capacity)
    {
        auto it = jobs.begin();
        guaranteed.splice(guaranteed.end(), jobs, it);
    }
    else
    {
        jobs.pop_front();
    }
}
////////////////////////////////////////////////////////////////////////////////
}

