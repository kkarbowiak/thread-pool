#include "tpoolThreadPool.h"

#include "tpoolCommand.h"
#include "tpoolJob.h"
#include "tpoolWorker.h"

#include <utility> // std::move()
#include <exception> // std::exception
#include <functional>
#include <future>
#include <atomic>


namespace
{
    class WaitJob : public tpool::Job
    {
        public:
            explicit WaitJob(std::function<void ()> function);
            void execute() override;

        private:
            std::function<void ()> mFunction;
    };
}

namespace
{
    void stopThreads(std::size_t num_threads, tpool::CommandQueue & command_queue);
}

namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
ThreadPool::ThreadPool(std::size_t num_workers)
    : mCommandQueue(num_workers)
    , mWorkers(new Worker[num_workers])
    , mWorkersNumber(num_workers)
{
    std::size_t started_workers = 0;

    try
    {
        for (; started_workers < num_workers; ++started_workers)
        {
            mWorkers[started_workers].start(mCommandQueue);
        }
    }
    catch (std::exception const &)
    {
        stopThreads(started_workers, mCommandQueue);

        throw;
    }
}
////////////////////////////////////////////////////////////////////////////////
ThreadPool::~ThreadPool()
{
    stopThreads(mWorkersNumber, mCommandQueue);
}
////////////////////////////////////////////////////////////////////////////////
void ThreadPool::addJob(std::unique_ptr<Job> job)
{
    Command job_command(std::move(job));

    mCommandQueue.addCommand(std::move(job_command));
}
////////////////////////////////////////////////////////////////////////////////
void ThreadPool::clearPendingJobs()
{
    mCommandQueue.clear();
}
////////////////////////////////////////////////////////////////////////////////
void ThreadPool::waitUntilJobsCompleted()
{
    std::promise<void> workers_promise1;
    std::promise<void> workers_promise2;
    std::promise<void> tpool_promise;
    std::shared_future<void> tpool_future = tpool_promise.get_future().share();
    std::atomic_size_t counter(0);
    std::size_t const total_count = mWorkersNumber;

    auto synchro = [&]
    {
        counter.fetch_add(1);
        if (counter.load() == total_count)
        {
            workers_promise1.set_value();
        }

        tpool_future.wait();

        counter.fetch_add(1);
        if (counter.load() == total_count)
        {
            workers_promise2.set_value();
        }
    };

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        Command job_command(std::unique_ptr<Job>(new WaitJob(synchro)));

        mCommandQueue.addCommand(std::move(job_command));
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
WaitJob::WaitJob(std::function<void ()> function)
    : mFunction(function)
{
}
////////////////////////////////////////////////////////////////////////////////
void WaitJob::execute()
{
    mFunction();
}
////////////////////////////////////////////////////////////////////////////////
}

namespace
{
////////////////////////////////////////////////////////////////////////////////
void stopThreads(std::size_t num_threads, tpool::CommandQueue & command_queue)
{
    for (std::size_t i = 0; i < num_threads; ++i)
    {
        tpool::Command termination_command;

        command_queue.addCommandGuaranteed(std::move(termination_command));
    }
}
////////////////////////////////////////////////////////////////////////////////
}

