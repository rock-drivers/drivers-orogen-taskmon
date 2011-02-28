#ifndef TASKMON_TYPES_HPP
#define TASKMON_TYPES_HPP

#include <base/time.h>
#include <linux/taskstats.h>
namespace taskmon
{
    /** This structure holds information about the runtime statistics of an OS
     * task (i.e. thread)
     *
     * It simply adds some more data to the stats returned by the Linux
     * kernel
     */
    struct TaskStats
    {
        /** The time at which these statistics have been received */
        base::Time time;
        /** The name given to watch() */
        std::string name;
        /** The statistics themselves */
        taskstats stats;
    };
}

#endif

