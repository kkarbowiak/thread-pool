/*
    Copyright 2017 Krzysztof Karbowiak
*/

#include "tpoolWorker.h"

#include "tpoolJobQueue.h"


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
    auto thread_function = [&job_queue]
    {
        while (true)
        {
            job_type job = job_queue.getJob();
            if (job)
            {
                job();
            }
            else
            {
                break;
            }
        }
    };

    mThread = std::thread(thread_function);
}
////////////////////////////////////////////////////////////////////////////////
}

