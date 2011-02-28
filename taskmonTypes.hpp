#ifndef TASKMON_TYPES_HPP
#define TASKMON_TYPES_HPP

#include <base/time.h>
#include <linux/taskstats.h>
namespace taskmon
{
    struct TaskStats
    {
        base::Time time;
        taskstats stats;
    };
}

#endif

