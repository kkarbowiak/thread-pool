#include "tpoolThreadPool.h"

#include "tpoolCommand.h"
#include "tpoolJob.h"
#include "tpoolWorker.h"

#include <utility> // std::move()
#include <exception> // std::exception
#include <condition_variable>


namespace
{
    class semaphore
    {
        public:
            semaphore();

            void acquire();
            void release();

        private:
            std::size_t mCount;
            std::mutex mMutex;
            std::condition_variable mCondVar;
    };

    class WaitJob : public tpool::Job
    {
        public:
            WaitJob(semaphore & worker_wait, semaphore & tpool_wait);
            void execute() override;

        private:
            semaphore & mWorkerWait;
            semaphore & mTpoolWait;
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
    semaphore worker_wait;
    semaphore tpool_wait;

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        Command job_command(std::unique_ptr<Job>(new WaitJob(worker_wait, tpool_wait)));

        mCommandQueue.addCommand(std::move(job_command));
    }

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        tpool_wait.acquire();
    }

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        worker_wait.release();
    }

    for (std::size_t i = 0; i < mWorkersNumber; ++i)
    {
        tpool_wait.acquire();
    }
}
////////////////////////////////////////////////////////////////////////////////
}

namespace
{
////////////////////////////////////////////////////////////////////////////////
semaphore::semaphore()
    : mCount(0)
    , mMutex()
    , mCondVar()
{
}
////////////////////////////////////////////////////////////////////////////////
void semaphore::acquire()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while (mCount == 0)
    {
        mCondVar.wait(lock);
    }

    --mCount;
}
////////////////////////////////////////////////////////////////////////////////
void semaphore::release()
{
    std::unique_lock<std::mutex> lock(mMutex);

    ++mCount;

    mCondVar.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
WaitJob::WaitJob(semaphore & worker_wait, semaphore & tpool_wait)
    : mWorkerWait(worker_wait)
    , mTpoolWait(tpool_wait)
{
}
////////////////////////////////////////////////////////////////////////////////
void WaitJob::execute()
{
    mTpoolWait.release();
    mWorkerWait.acquire();
    mTpoolWait.release();
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

