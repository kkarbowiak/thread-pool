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
    std::promise<void> workers_promise1;
    std::promise<void> workers_promise2;
    std::promise<void> tpool_promise;
    std::shared_future<void> tpool_future = tpool_promise.get_future().share();
    std::atomic_size_t counter(0);

    auto synchro = [&]
    {
        counter.fetch_add(1);
        if (counter.load() == mWorkersNumber)
        {
            workers_promise1.set_value();
        }

        tpool_future.wait();

        counter.fetch_add(1);
        if (counter.load() == mWorkersNumber)
        {
            workers_promise2.set_value();
        }
    };

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        mJobQueue.addJob(synchro);
    }

    workers_promise1.get_future().wait();
    counter.store(0);
    tpool_promise.set_value();
    workers_promise2.get_future().wait();
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

