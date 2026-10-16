/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef TASKMON_TASK_TASK_HPP
#define TASKMON_TASK_TASK_HPP

#include "taskmon/TaskBase.hpp"

#ifdef USE_LIBPROC2
#include <libproc2/pids.h>

struct pids_stack;
struct pids_info;

namespace taskmon {

    enum TaskPidsItemIndex {
        IDX_PID = 0, IDX_TID, IDX_PPID, IDX_STATE,
        IDX_TICS_USER, IDX_TICS_SYSTEM, IDX_TICS_USER_C, IDX_TICS_SYSTEM_C, IDX_TICS_BEGAN,
        IDX_PRIORITY, IDX_NICE, IDX_RSS, IDX_MEM_VIRT_PGS, IDX_MEM_RES_PGS, IDX_MEM_SHR_PGS,
        IDX_MEM_CODE_PGS, IDX_MEM_DATA_PGS, IDX_VM_SIZE, IDX_VM_RSS_LOCKED, IDX_VM_RSS, IDX_VM_DATA,
        IDX_VM_STACK, IDX_VM_EXE, IDX_VM_LIB, IDX_PRIORITY_RT, IDX_SCHED_CLASS, IDX_VSIZE_BYTES,
        IDX_FLT_MIN, IDX_FLT_MAJ, IDX_FLT_MIN_C, IDX_FLT_MAJ_C,
        IDX_PGRP, IDX_SESSION, IDX_NLWP, IDX_TGID, IDX_TPGID, IDX_PROCESSOR,
        IDX_CMDLINE_V,
        NUM_IDX
    };

    extern enum pids_item items[NUM_IDX];
#else
struct PROCTAB;
struct proc_t;

namespace taskmon {
#endif

    class Task : public TaskBase
    {
	friend class TaskBase;
    protected:

        struct Watch
        {
            std::string name;
            int pid;
        };
        typedef std::map<boost::int32_t, Watch> TaskWatches;
        TaskWatches watches;

    	double sysClockPeriodInMuS;

        /* Handler for the watch operation
         */
        virtual void watch(std::string const& name, boost::int32_t pid);
        virtual void removeWatchFromName(std::string const& name);
        virtual void removeWatchFromPID(boost::int32_t pid);

#ifdef USE_LIBPROC2
    	void processProcStack(bool watch_all, TaskStats& stats, pids_stack *pid_stack, pids_info *info);
#else
        PROCTAB* proctab;
        void processProcInfo(bool watch_all, TaskStats& stats, proc_t const& proc_info);
#endif

    public:
        Task(std::string const& name = "taskmon::Task", TaskCore::TaskState initial_state = Stopped);

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to Running. If it returns false, then the component will
         * stay in Stopped. Otherwise, it goes into Running and updateHook()
         * will be called.
         */
        bool startHook();

        /** This hook is called by Orocos when the component is in the Running
         * state, at each activity step. Here, the activity gives the "ticks"
         * when the hook should be called.
         *
         * The error(), exception() and fatal() calls, when called in this hook,
         * allow to get into the associated RunTimeError, Exception and
         * FatalError states.
         *
         * In the first case, updateHook() is still called, and recover() allows
         * you to go back into the Running state.  In the second case, the
         * errorHook() will be called instead of updateHook(). In Exception, the
         * component is stopped and recover() needs to be called before starting
         * it again. Finally, FatalError cannot be recovered.
         */
        void updateHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Running to Stopped after stop() has been called.
         */
        void stopHook();
    };
}

#endif
