#include "tpoolWorker.h"

#include "tpoolJobQueue.h"

#include <functional> // std::ref()


namespace
{
    void workerThreadFunction(tpool::JobQueue & job_queue);
}

namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
Worker::~Worker()
{
    if (mThread.joinable())
    {
        mThread.join();
    }
}
////////////////////////////////////////////////////////////////////////////////
void Worker::start(JobQueue & job_queue)
{
    mThread = std::thread(&workerThreadFunction, std::ref(job_queue));
}
////////////////////////////////////////////////////////////////////////////////
}

namespace
{
////////////////////////////////////////////////////////////////////////////////
void workerThreadFunction(tpool::JobQueue & job_queue)
{
    while (true)
    {
        std::function<void ()> job = job_queue.getJob();
        if (job)
        {
            job();
        }
        else
        {
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
}

