#include "tpoolWorker.h"

#include "tpoolCommandQueue.h"
#include "tpoolCommand.h"
#include "tpoolJob.h"

#include <functional> // std::ref()


namespace
{
    void workerThreadFunction(tpool::CommandQueue & command_queue);
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
void Worker::start(CommandQueue & command_queue)
{
    mThread = std::thread(&workerThreadFunction, std::ref(command_queue));
}
////////////////////////////////////////////////////////////////////////////////
}

namespace
{
////////////////////////////////////////////////////////////////////////////////
void workerThreadFunction(tpool::CommandQueue & command_queue)
{
    while (true)
    {
        tpool::Command command = command_queue.getCommand();
        if (command.getType() == tpool::Command::JOB_EXECUTION)
        {
            command.getJob()->execute();
        }
        else
        {
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
}

