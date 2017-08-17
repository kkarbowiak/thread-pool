#ifndef TPOOL_COMMAND_QUEUE_H
#define TPOOL_COMMAND_QUEUE_H

#include <list>
#include <mutex>
#include <condition_variable>
#include <cstddef> // std::size_t


namespace tpool { class Command; }

namespace tpool
{
    class CommandQueue
    {
        public:
            explicit CommandQueue(std::size_t guaranteed_capacity);

            void addCommand(Command command);
            void addCommandGuaranteed(Command command);

            Command getCommand();

            void clear();

        private:
            std::list<Command> mCommands;
            std::list<Command> mGuaranteed;
            std::size_t mGuaranteedCapacity;
            std::mutex mMutex;
            std::condition_variable mCondVar;
    };
}

#endif /* TPOOL_COMMAND_QUEUE_H */

