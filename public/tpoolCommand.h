#ifndef TPOOL_COMMAND_H
#define TPOOL_COMMAND_H

#include <memory>


namespace tpool
{
    class Job;
}

namespace tpool
{
    class Command
    {
        public:
            enum Type
            {
                JOB_EXECUTION,
                WORKER_TERMINATION
            };

        public:
            Command();
            explicit Command(std::unique_ptr<Job> job);
            Command(Command && other);
            ~Command();

            Command & operator=(Command && other);

            Type getType() const;
            std::unique_ptr<Job> getJob();

        private:
            Type mType;
            std::unique_ptr<Job> mJob;
    };
}

#endif /* TPOOL_COMMAND_H */

