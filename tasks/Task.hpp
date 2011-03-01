/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef TASKMON_TASK_TASK_HPP
#define TASKMON_TASK_TASK_HPP

#include "taskmon/TaskBase.hpp"

struct nl_sock;
struct nl_msg;

namespace taskmon {
    class Task : public TaskBase
    {
	friend class TaskBase;
    protected:

        struct Watch
        {
            std::string name;
            nl_msg* request_msg;
        };
        typedef std::map<boost::int32_t, Watch> TaskWatches;
        TaskWatches watches;


        /* Handler for the watch operation
         */
        virtual void watch(std::string const& name, boost::int32_t pid);
        virtual void removeWatchFromName(std::string const& name);
        virtual void removeWatchFromPID(boost::int32_t pid);

        nl_sock* netlink_socket;
        int netlink_family;

        void receive(nl_msg* msg);
        static int receiveCallback(struct nl_msg *msg, void *arg);

    public:
        Task(std::string const& name = "taskmon::Task", TaskCore::TaskState initial_state = Stopped);

        /** This hook is called by Orocos when the state machine transitions
         * from PreOperational to Stopped. If it returns false, then the
         * component will stay in PreOperational. Otherwise, it goes into
         * Stopped.
         *
         * It is meaningful only if the #needs_configuration has been specified
         * in the task context definition with (for example):
         *
         *   task_context "TaskName" do
         *     needs_configuration
         *     ...
         *   end
         */
        // bool configureHook();

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

        /** This hook is called by Orocos when the component is in the
         * RunTimeError state, at each activity step. See the discussion in
         * updateHook() about triggering options.
         *
         * Call recovered() to go back in the Runtime state.
         */
        // void errorHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Running to Stopped after stop() has been called.
         */
        void stopHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to PreOperational, requiring the call to configureHook()
         * before calling start() again.
         */
        // void cleanupHook();
    };
}

#endif

