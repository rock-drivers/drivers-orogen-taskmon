/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

#ifdef USE_LIBPROC2
#include <libproc2/pids.h>
namespace taskmon {

enum pids_item items[NUM_IDX] = {
    PIDS_ID_PID, PIDS_ID_TID, PIDS_ID_PPID, PIDS_STATE,
    PIDS_TICS_USER, PIDS_TICS_SYSTEM, PIDS_TICS_USER_C, PIDS_TICS_SYSTEM_C, PIDS_TICS_BEGAN,
    PIDS_PRIORITY, PIDS_NICE, PIDS_RSS, PIDS_MEM_VIRT_PGS, PIDS_MEM_RES_PGS, PIDS_MEM_SHR_PGS,
    PIDS_MEM_CODE_PGS, PIDS_MEM_DATA_PGS, PIDS_VM_SIZE, PIDS_VM_RSS_LOCKED, PIDS_VM_RSS, PIDS_VM_DATA,
    PIDS_VM_STACK, PIDS_VM_EXE, PIDS_VM_LIB, PIDS_PRIORITY_RT, PIDS_SCHED_CLASS, PIDS_VSIZE_BYTES,
    PIDS_FLT_MIN, PIDS_FLT_MAJ, PIDS_FLT_MIN_C, PIDS_FLT_MAJ_C,
    PIDS_ID_PGRP, PIDS_ID_SESSION, PIDS_NLWP, PIDS_ID_TGID, PIDS_ID_TPGID, PIDS_PROCESSOR,
    PIDS_CMDLINE_V
};
#else
#include <proc/readproc.h>
namespace taskmon {
#endif

Task::Task(std::string const& name, TaskCore::TaskState initial_state)
    : TaskBase(name, initial_state)
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

bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;

    sysClockPeriodInMuS = 1e6 / static_cast<double>(sysconf(_SC_CLK_TCK));
    return true;
}

#ifdef USE_LIBPROC2
void Task::processProcStack(bool watch_all, TaskStats& stats, pids_stack *pid_stack, pids_info *info) {

    auto tid = PIDS_VAL(IDX_TID, s_int, pid_stack, info);
    TaskWatches::const_iterator watch_it = watches.find(tid);

    if (!watch_all && watch_it == watches.end())
       	return;

    stats.tasks.resize(stats.tasks.size() + 1);
    TaskInfo& task_info(stats.tasks.back());

    if (watch_it != watches.end())
       	task_info.name = watch_it->second.name;

    task_info.tid = tid;
    task_info.ppid = PIDS_VAL(IDX_PPID, s_int, pid_stack, info);
    task_info.state = PIDS_VAL(IDX_STATE, s_ch, pid_stack, info);

    task_info.utime  = base::Time::fromMicroseconds(sysClockPeriodInMuS * PIDS_VAL(IDX_TICS_USER, ull_int, pid_stack, info));
    task_info.stime  = base::Time::fromMicroseconds(sysClockPeriodInMuS * PIDS_VAL(IDX_TICS_SYSTEM, ull_int, pid_stack, info));

    task_info.cutime = base::Time::fromMicroseconds(sysClockPeriodInMuS * (PIDS_VAL(IDX_TICS_USER_C, ull_int, pid_stack, info) - PIDS_VAL(IDX_TICS_USER, ull_int, pid_stack, info)));
    task_info.cstime = base::Time::fromMicroseconds(sysClockPeriodInMuS * (PIDS_VAL(IDX_TICS_SYSTEM_C, ull_int, pid_stack, info) - PIDS_VAL(IDX_TICS_SYSTEM, ull_int, pid_stack, info)));
    task_info.start_time = base::Time::fromMicroseconds(sysClockPeriodInMuS * PIDS_VAL(IDX_TICS_BEGAN, ull_int, pid_stack, info));

    task_info.priority = PIDS_VAL(IDX_PRIORITY, s_int, pid_stack, info);
    task_info.nice = PIDS_VAL(IDX_NICE, s_int, pid_stack, info);
    task_info.rss = PIDS_VAL(IDX_RSS, ul_int, pid_stack, info);
    task_info.size = PIDS_VAL(IDX_MEM_VIRT_PGS, ul_int, pid_stack, info);
    task_info.resident = PIDS_VAL(IDX_MEM_RES_PGS, ul_int, pid_stack, info);
    task_info.share = PIDS_VAL(IDX_MEM_SHR_PGS, ul_int, pid_stack, info);
    task_info.trs = PIDS_VAL(IDX_MEM_CODE_PGS, ul_int, pid_stack, info);
    task_info.lrs = 0; // Not available in libproc2
    task_info.drs = PIDS_VAL(IDX_MEM_DATA_PGS, ul_int, pid_stack, info);
    task_info.dt = 0; // Not available in libproc2
    task_info.vm_size = PIDS_VAL(IDX_VM_SIZE, ul_int, pid_stack, info);
    task_info.vm_lock = PIDS_VAL(IDX_VM_RSS_LOCKED, ul_int, pid_stack, info);
    task_info.vm_rss = PIDS_VAL(IDX_VM_RSS, ul_int, pid_stack, info);
    task_info.vm_data = PIDS_VAL(IDX_VM_DATA, ul_int, pid_stack, info);
    task_info.vm_stack = PIDS_VAL(IDX_VM_STACK, ul_int, pid_stack, info);
    task_info.vm_exe = PIDS_VAL(IDX_VM_EXE, ul_int, pid_stack, info);
    task_info.vm_lib = PIDS_VAL(IDX_VM_LIB, ul_int, pid_stack, info);
    task_info.rtprio = PIDS_VAL(IDX_PRIORITY_RT, s_int, pid_stack, info);
    task_info.sched = PIDS_VAL(IDX_SCHED_CLASS, s_int, pid_stack, info);
    task_info.vsize = PIDS_VAL(IDX_VSIZE_BYTES, ul_int, pid_stack, info);

    task_info.min_flt = PIDS_VAL(IDX_FLT_MIN, ul_int, pid_stack, info);
    task_info.maj_flt = PIDS_VAL(IDX_FLT_MAJ, ul_int, pid_stack, info);
    task_info.cmin_flt = PIDS_VAL(IDX_FLT_MIN_C, ul_int, pid_stack, info) - PIDS_VAL(IDX_FLT_MIN, ul_int, pid_stack, info);
    task_info.cmaj_flt = PIDS_VAL(IDX_FLT_MAJ_C, ul_int, pid_stack, info) - PIDS_VAL(IDX_FLT_MAJ, ul_int, pid_stack, info);

    task_info.pgrp = PIDS_VAL(IDX_PGRP, s_int, pid_stack, info);
    task_info.session = PIDS_VAL(IDX_SESSION, s_int, pid_stack, info);
    task_info.nlwp = PIDS_VAL(IDX_NLWP, s_int, pid_stack, info);
    task_info.tgid = PIDS_VAL(IDX_TGID, s_int, pid_stack, info);
    task_info.tpgid = PIDS_VAL(IDX_TPGID, s_int, pid_stack, info);
    task_info.processor = PIDS_VAL(IDX_PROCESSOR, s_int, pid_stack, info);

    char** cmdline_element = PIDS_VAL(IDX_CMDLINE_V, strv, pid_stack, info);
    if (cmdline_element)
    {
        while (*cmdline_element)
        {
            task_info.cmdline.push_back(std::string(*cmdline_element));
            ++cmdline_element;
        }
    }
}
#else
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
#endif

void Task::updateHook()
{
    TaskBase::updateHook();

    bool watch_all = _watch_all.get();

    TaskStats stats;
    stats.time = base::Time::now();

#ifdef USE_LIBPROC2
    int num_items = sizeof(items) / sizeof(enum pids_item);
    struct pids_info *info = NULL;

    if (procps_pids_new(&info, items, num_items) < 0) {
        return;
    }

    struct pids_stack *stack;

    while ((stack = procps_pids_get(info, PIDS_FETCH_THREADS_TOO))) {
       processProcStack(watch_all, stats, stack, info);
    }
    procps_pids_unref(&info);
#else
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
    closeproc(proctab);
#endif

    stats.end_time = base::Time::now();

    _stats.write(stats);
}

void Task::stopHook()
{
    TaskBase::stopHook();
}

} // namespace taskmon
