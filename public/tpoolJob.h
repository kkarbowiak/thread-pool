#ifndef TPOOL_JOB_H
#define TPOOL_JOB_H


namespace tpool
{
    class Job
    {
        public:
            virtual void execute() = 0;

            virtual ~Job() = default;
    };
}

#endif /* TPOOL_JOB_H */

