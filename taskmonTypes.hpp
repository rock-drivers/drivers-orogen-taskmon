#ifndef TASKMON_TYPES_HPP
#define TASKMON_TYPES_HPP

#include <base/Time.hpp>
#include <vector>

namespace taskmon
{
    struct TaskInfo
    {
        /** The name given to watch() */
        std::string name;

	int
	    tid,		// (special)       task id, the POSIX thread ID (see also: tgid)
	    ppid;		// stat,status     pid of parent process
	char
	    state;		// stat,status     single-char code for process state (S=sleeping)
	base::Time
	    utime,		// stat            user-mode CPU time accumulated by process
	    stime,		// stat            kernel-mode CPU time accumulated by process
	    cutime,		// stat            cumulative utime of process and reaped children
	    cstime,		// stat            cumulative stime of process and reaped children
	    start_time;	// stat            start time of process -- seconds since 1-1-70
	long
	    priority,	// stat            kernel scheduling priority
	    nice,		// stat            standard unix nice level of process
	    rss,		// stat            resident set size from /proc/#/stat (pages)
	    size,		// statm           total # of pages of memory
	    resident,	// statm           number of resident set (non-swapped) pages (4k)
	    share,		// statm           number of pages of shared (mmap'd) memory
	    trs,		// statm           text resident set size
	    lrs,		// statm           shared-lib resident set size
	    drs,		// statm           data resident set size
	    dt;		// statm           dirty pages
	unsigned long
	    vm_size,        // status          same as vsize in kb
	    vm_lock,        // status          locked pages in kb
	    vm_rss,         // status          same as rss in kb
	    vm_data,        // status          data size
	    vm_stack,       // status          stack size
	    vm_exe,         // status          executable size
	    vm_lib,         // status          library size (all pages, not just used ones)
	    rtprio,		// stat            real-time priority
	    sched,		// stat            scheduling class
	    vsize,		// stat            number of pages of virtual memory ...
	    min_flt,	// stat            number of minor page faults since process start
	    maj_flt,	// stat            number of major page faults since process start
	    cmin_flt,	// stat            cumulative min_flt of process and child processes
	    cmaj_flt;	// stat            cumulative maj_flt of process and child processes
	int
	    pgrp,		// stat            process group id
	    session,	// stat            session id
	    nlwp,		// stat,status     number of threads, or 0 if no clue
	    tgid,		// (special)       task group ID, the POSIX PID (see also: tid)
	    tpgid,		// stat            terminal process group id
	    processor;      // stat            current (or most recent?) CPU

	std::vector<std::string> cmdline;
    };

    /** This structure holds information about the runtime statistics of an OS
     * task (i.e. thread)
     *
     * It simply adds some more data to the stats returned by the Linux
     * kernel
     */
    struct TaskStats
    {
        /** The time at which the task started to gather these statistics */
        base::Time time;
        /** The time at which the task finished to gather these statistics */
        base::Time end_time;

	std::vector<TaskInfo> tasks;
    };

}

#endif

