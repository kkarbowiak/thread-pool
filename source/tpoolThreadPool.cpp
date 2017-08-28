#include "tpoolThreadPool.h"

#include "tpoolWorker.h"

#include <utility> // std::move()
#include <exception> // std::exception
#include <future>
#include <atomic>
#include <cassert>


namespace
{
    void stopThreads(std::size_t num_threads, tpool::JobQueue & job_queue);
}

namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
ThreadPool::ThreadPool(std::size_t num_workers)
    : mJobQueue(num_workers)
    , mWorkers(new Worker[num_workers])
    , mWorkersNumber(num_workers)
{
    std::size_t started_workers = 0;

    try
    {
        for (; started_workers < num_workers; ++started_workers)
        {
            mWorkers[started_workers].start(mJobQueue);
        }
    }
    catch (std::exception const &)
    {
        stopThreads(started_workers, mJobQueue);

        throw;
    }
}
////////////////////////////////////////////////////////////////////////////////
ThreadPool::~ThreadPool()
{
    stopThreads(mWorkersNumber, mJobQueue);
}
////////////////////////////////////////////////////////////////////////////////
void ThreadPool::addJob(job_type job)
{
    assert(job);

    mJobQueue.addJob(std::move(job));
}
////////////////////////////////////////////////////////////////////////////////
void ThreadPool::clearPendingJobs()
{
    mJobQueue.clear();
}
////////////////////////////////////////////////////////////////////////////////
void ThreadPool::waitUntilJobsCompleted()
{
    std::promise<void> workers_promise;
    std::promise<void> tpool_promise;
    std::shared_future<void> tpool_future = tpool_promise.get_future().share();
    std::atomic_size_t counter(0);

    auto synchro = [&, tpool_future]
    {
        if (counter.fetch_add(1) == mWorkersNumber - 1)
        {
            workers_promise.set_value();
        }

        tpool_future.wait();
    };

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        mJobQueue.addJob(synchro);
    }

    workers_promise.get_future().wait();
    tpool_promise.set_value();
}
////////////////////////////////////////////////////////////////////////////////
}

namespace
{
////////////////////////////////////////////////////////////////////////////////
void stopThreads(std::size_t num_threads, tpool::JobQueue & job_queue)
{
    for (std::size_t i = 0; i < num_threads; ++i)
    {
        job_queue.addJobGuaranteed(tpool::job_type());
    }
}
////////////////////////////////////////////////////////////////////////////////
}

