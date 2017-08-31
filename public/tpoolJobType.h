/*
    Copyright 2017 Krzysztof Karbowiak
*/

#ifndef TPOOL_JOB_TYPE_H
#define TPOOL_JOB_TYPE_H

#include <functional> // std::function


namespace tpool
{
    using job_type = std::function<void ()>;
}

#endif /* TPOOL_JOB_TYPE_H */

