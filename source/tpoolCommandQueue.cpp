#include "tpoolCommandQueue.h"

#include "tpoolCommand.h"

#include <utility> // std::move()


namespace tpool
{
////////////////////////////////////////////////////////////////////////////////
CommandQueue::CommandQueue(std::size_t guaranteed_capacity)
    : mCommands()
    , mGuaranteed()
    , mGuaranteedCapacity(guaranteed_capacity)
    , mMutex()
    , mCondVar()
{
    for (std::size_t i = 0; i < guaranteed_capacity; ++i)
    {
        mGuaranteed.push_back(Command());
    }
}
////////////////////////////////////////////////////////////////////////////////
void CommandQueue::addCommand(Command command)
{
    std::unique_lock<std::mutex> lock(mMutex);

    mCommands.push_back(std::move(command));

    mCondVar.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
void CommandQueue::addCommandGuaranteed(Command command)
{
    std::unique_lock<std::mutex> lock(mMutex);

    auto it = mGuaranteed.begin();
    *it = std::move(command);
    mCommands.splice(mCommands.end(), mGuaranteed, it);

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
    if (mGuaranteed.size() < mGuaranteedCapacity)
    {
        auto it = mCommands.begin();
        mGuaranteed.splice(mGuaranteed.end(), mCommands, it);
    }
    else
    {
        mCommands.pop_front();
    }

    return command;
}
////////////////////////////////////////////////////////////////////////////////
void CommandQueue::clear()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while (!mCommands.empty())
    {
        if (mGuaranteed.size() < mGuaranteedCapacity)
        {
            auto it = mCommands.begin();
            mGuaranteed.splice(mGuaranteed.end(), mCommands, it);
        }
        else
        {
            mCommands.pop_front();
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
}

