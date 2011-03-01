/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <linux/taskstats.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

using namespace taskmon;

Task::Task(std::string const& name, TaskCore::TaskState initial_state)
    : TaskBase(name, initial_state)
    , netlink_socket(NULL)
{
}


void Task::watch(std::string const& name, boost::int32_t pid)
{
    nl_msg *msg = 0;
    if (netlink_socket)
    {
        msg = nlmsg_alloc();
        genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, netlink_family, 0, 
                NLM_F_REQUEST, TASKSTATS_CMD_GET, TASKSTATS_GENL_VERSION);
        nla_put_u32(msg, TASKSTATS_CMD_ATTR_PID, pid);
    }

    Watch watch;
    watch.name = name;
    watch.request_msg = msg;
    watches.insert(make_pair(pid, watch));
}

void Task::removeWatchFromName(std::string const& name)
{
    for (TaskWatches::iterator it = watches.begin(); it != watches.end(); ++it)
    {
        if (it->second.name == name)
        {
            nlmsg_free(it->second.request_msg);
            watches.erase(it);
        }
    }
}

void Task::removeWatchFromPID(boost::int32_t pid)
{
    TaskWatches::iterator it = watches.find(pid);
    if (it != watches.end())
    {
        nlmsg_free(it->second.request_msg);
        watches.erase(it);
    }
}

void Task::receive(nl_msg* msg)
{
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    struct nlattr *main[__TASKSTATS_TYPE_MAX+1];
    struct nlattr *actual[__TASKSTATS_TYPE_MAX+1];

    // Validate message and parse attributes
    int err = genlmsg_parse(nlh, 0, main, __TASKSTATS_TYPE_MAX, NULL);
    if (err != 0)
        throw std::runtime_error(std::string("invalid message received: ") + nl_geterror(err));

    if (main[TASKSTATS_TYPE_AGGR_PID]) {
        int err = nla_parse_nested(actual, __TASKSTATS_TYPE_MAX, main[TASKSTATS_TYPE_AGGR_PID], NULL);
        if (err != 0)
            throw std::runtime_error(std::string("invalid message received: ") + nl_geterror(err));

        if (actual[TASKSTATS_TYPE_STATS])
        {
            void* data = nla_data(actual[TASKSTATS_TYPE_STATS]);
            taskstats* stats = reinterpret_cast<taskstats*>(data);

            TaskStats result;
            result.time = base::Time::now();
            TaskWatches::const_iterator it =
                watches.find(stats->ac_pid);
            if (it != watches.end())
                result.name = it->second.name;

            result.stats = *stats;

            _stats.write(result);
        }
    }
}

int Task::receiveCallback(struct nl_msg *msg, void *arg)
{
    static_cast<Task*>(arg)->receive(msg);
    return NL_OK;
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

    netlink_socket = nl_socket_alloc();
    if (!netlink_socket)
        return false;

    genl_connect(netlink_socket);

    int err = nl_socket_modify_cb(netlink_socket, NL_CB_VALID, NL_CB_CUSTOM, &receiveCallback, this);
    if (0 != err)
        return false;

    nl_socket_set_nonblocking(netlink_socket);
    nl_socket_disable_auto_ack(netlink_socket);
    nl_socket_disable_seq_check(netlink_socket);

    netlink_family = genl_ctrl_resolve(netlink_socket, TASKSTATS_GENL_NAME);
    if (netlink_family < 0)
        return false;

    // Create the messages for all pending watches
    TaskWatches watches = this->watches;
    this->watches.clear();
    for (TaskWatches::iterator it = watches.begin(); it != watches.end(); ++it)
    {
        if (it->second.request_msg) this->watches.insert(*it);
        else
            watch(it->second.name, it->first);
    }

    return true;
}

void Task::sendRequests()
{
    for (TaskWatches::const_iterator it = watches.begin(); it != watches.end(); ++it)
    {
        nl_msg* msg = it->second.request_msg;
        nlmsg_hdr(msg)->nlmsg_seq = nl_socket_use_seq(netlink_socket);
        nl_send_auto_complete(netlink_socket, msg);
    }
}

void Task::updateHook()
{
    TaskBase::updateHook();
    
    if (watches.empty())
        return;

    sendRequests();
    while(true)
    {
        int ret = nl_recvmsgs_default(netlink_socket);
        if (ret == 0)
            break;
        else if (ret <= 0)
        {
            if (ret != -NLE_FAILURE) // this is received when a PID is invalid, ignore
                throw std::runtime_error(std::string("cannot read stats: ") + nl_geterror(ret));
        }
    }
}

// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
void Task::stopHook()
{
    nl_socket_free(netlink_socket);
    netlink_socket = 0;

    for (TaskWatches::iterator it = watches.begin(); it != watches.end(); ++it)
    {
        nlmsg_free(it->second.request_msg);
        it->second.request_msg = 0;
    }

    TaskBase::stopHook();
}
// void Task::cleanupHook()
// {
//     TaskBase::cleanupHook();
// }

