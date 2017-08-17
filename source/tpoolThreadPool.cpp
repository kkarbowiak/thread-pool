#include "tpoolThreadPool.h"

#include "tpoolCommand.h"
#include "tpoolJob.h"
#include "tpoolWorker.h"

#include <utility> // std::move()
#include <exception> // std::exception


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

