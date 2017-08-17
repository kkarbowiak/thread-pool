#include "tpoolThreadPool.h"

#include "tpoolCommand.h"
#include "tpoolJob.h"
#include "tpoolWorker.h"

#include <utility> // std::move()


namespace
{
    void stopThreads(std::size_t num_threads, tpool::CommandQueue & command_queue);
}

namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
ThreadPool::ThreadPool(std::size_t num_workers)
    : mCommandQueue()
    , mWorkersArray(new Worker[num_workers])
    , mWorkersNumber(num_workers)
{
    for (std::size_t i = 0; i < num_workers; ++i)
    {
        mWorkersArray[i].start(mCommandQueue);
    }
}
////////////////////////////////////////////////////////////////////////////////
ThreadPool::~ThreadPool()
{
    stopThreads(mWorkersNumber, mCommandQueue);

    delete[] mWorkersArray;
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

        command_queue.addCommand(std::move(termination_command));
    }
}
////////////////////////////////////////////////////////////////////////////////
}

