#ifndef TPOOL_COMMAND_QUEUE_H
#define TPOOL_COMMAND_QUEUE_H

#include <list>
#include <mutex>
#include <condition_variable>


namespace tpool { class Command; }

namespace tpool
{
    class CommandQueue
    {
        public:
            void addCommand(Command command);
            Command getCommand();

        private:
            std::list<Command> mCommands;
            std::mutex mMutex;
            std::condition_variable mCondVar;
    };
}

#endif /* TPOOL_COMMAND_QUEUE_H */

