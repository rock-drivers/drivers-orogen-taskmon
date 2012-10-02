/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <proc/readproc.h>

using namespace taskmon;

Task::Task(std::string const& name, TaskCore::TaskState initial_state)
    : TaskBase(name, initial_state)
    , proctab(NULL)
{
}


void Task::watch(std::string const& name, boost::int32_t pid)
{
    TaskWatches::iterator watch_it = watches.find(pid);
    if (watch_it != watches.end())
    {
        std::cout << "renaming " << watch_it->second.name << " to " << name << std::endl;
        watch_it->second.name = name;
        return;
    }

    Watch watch;
    watch.name = name;
    watch.pid = pid;
    watches.insert(std::make_pair(pid, watch));
}

void Task::removeWatchFromName(std::string const& name)
{
    for (TaskWatches::iterator it = watches.begin(); it != watches.end(); ++it)
    {
        if (it->second.name == name)
        {
            std::cout << "removing watch " << it->second.name << std::endl;
            watches.erase(it);
        }
    }
}

void Task::removeWatchFromPID(boost::int32_t pid)
{
    TaskWatches::iterator it = watches.find(pid);
    if (it != watches.end())
    {
        std::cout << "removing watch " << it->second.name << std::endl;
        watches.erase(it);
    }
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

// bool Task::configureHook()
// {
//     if (! TaskBase::configureHook())
//         return false;
//     return true;
// }

bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;

    sysClockPeriodInMuS = 1e6 / static_cast<double>(sysconf(_SC_CLK_TCK));
    return true;
}

void Task::processProcInfo(bool watch_all, TaskStats& stats, proc_t const& proc_info)
{
    TaskWatches::const_iterator watch_it = watches.find(proc_info.tid);
    if (!watch_all && watch_it == watches.end())
	return;

    stats.tasks.resize(stats.tasks.size() + 1);
    TaskInfo& task_info(stats.tasks.back());

    if (watch_it != watches.end())
	task_info.name = watch_it->second.name;

    task_info.tid = proc_info.tid;
    task_info.ppid = proc_info.ppid;
    task_info.state = proc_info.state;

    task_info.utime  = base::Time::fromMicroseconds(sysClockPeriodInMuS * proc_info.utime);
    task_info.stime  = base::Time::fromMicroseconds(sysClockPeriodInMuS * proc_info.stime);
    task_info.cutime = base::Time::fromMicroseconds(sysClockPeriodInMuS * proc_info.cutime);
    task_info.cstime = base::Time::fromMicroseconds(sysClockPeriodInMuS * proc_info.cstime);
    task_info.start_time = base::Time::fromMicroseconds(sysClockPeriodInMuS * proc_info.start_time);
    task_info.priority = proc_info.priority;
    task_info.nice = proc_info.nice;
    task_info.rss = proc_info.rss;
    task_info.size = proc_info.size;
    task_info.resident = proc_info.resident;
    task_info.share = proc_info.share;
    task_info.trs = proc_info.trs;
    task_info.lrs = proc_info.lrs;
    task_info.drs = proc_info.drs;
    task_info.dt = proc_info.dt;
    task_info.vm_size = proc_info.vm_size;
    task_info.vm_lock = proc_info.vm_lock;
    task_info.vm_rss = proc_info.vm_rss;
    task_info.vm_data = proc_info.vm_data;
    task_info.vm_stack = proc_info.vm_stack;
    task_info.vm_exe = proc_info.vm_exe;
    task_info.vm_lib = proc_info.vm_lib;
    task_info.rtprio = proc_info.rtprio;
    task_info.sched = proc_info.sched;
    task_info.vsize = proc_info.vsize;
    task_info.min_flt = proc_info.min_flt;
    task_info.maj_flt = proc_info.maj_flt;
    task_info.cmin_flt = proc_info.cmin_flt;
    task_info.cmaj_flt = proc_info.cmaj_flt;
    task_info.pgrp = proc_info.pgrp;
    task_info.session = proc_info.session;
    task_info.nlwp = proc_info.nlwp;
    task_info.tgid = proc_info.tgid;
    task_info.tpgid = proc_info.tpgid;
    task_info.processor = proc_info.processor;

    char** cmdline_element = proc_info.cmdline;
    if (cmdline_element)
    {
	while (*cmdline_element)
	{
	    task_info.cmdline.push_back(std::string(*cmdline_element));
	    ++cmdline_element;
	}
    }
}

void Task::updateHook()
{
    TaskBase::updateHook();

    bool watch_all = _watch_all.get();

    TaskStats stats;
    stats.time = base::Time::now();
    
    proctab = openproc(PROC_FILLMEM | PROC_FILLCOM | PROC_FILLSTAT | PROC_FILLSTATUS);
    proc_t proc_info;
    memset(&proc_info, 0, sizeof(proc_info));
    proc_t task_info;
    memset(&task_info, 0, sizeof(task_info));
    while (readproc(proctab, &proc_info) != NULL)
    {
	processProcInfo(watch_all, stats, proc_info);
	while(readtask(proctab, &proc_info, &task_info) != NULL)
	    processProcInfo(watch_all, stats, task_info);
    }
    stats.end_time = base::Time::now();
    closeproc(proctab);
    
    _stats.write(stats);
}

// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
void Task::stopHook()
{
    TaskBase::stopHook();
}
// void Task::cleanupHook()
// {
//     TaskBase::cleanupHook();
// }

