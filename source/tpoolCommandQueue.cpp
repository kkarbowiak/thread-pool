#include "tpoolCommandQueue.h"

#include "tpoolCommand.h"

#include <utility> // std::move()


namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
void CommandQueue::addCommand(Command command)
{
    std::unique_lock<std::mutex> lock(mMutex);

    mCommands.push_back(std::move(command));

    mCondVar.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
Command CommandQueue::getCommand()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while (mCommands.empty())
    {
        mCondVar.wait(lock);
    }

    Command command = std::move(mCommands.front());
    mCommands.pop_front();

    return command;
}
////////////////////////////////////////////////////////////////////////////////
}

