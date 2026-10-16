#include <gtest/gtest.h>
#include "../tasks/Task.hpp"
#include <unistd.h>

#ifdef USE_LIBPROC2
#include <libproc2/pids.h>

// Inherit to expose protected methods and properties for testing
class TestableTask : public taskmon::Task {
public:
    using taskmon::Task::Task; // inherit constructor
    using taskmon::Task::processProcStack;
    using taskmon::Task::watch;
};

TEST(TaskTest, processProcStack_WatchAll_TracksProcess) {
    TestableTask task("test_task");
    taskmon::TaskStats stats;
    
    // Initialize libproc2 to fetch data for the current process
    pids_info *info = nullptr;
    int num_items = sizeof(taskmon::items) / sizeof(enum pids_item);
    
    ASSERT_EQ(0, procps_pids_new(&info, taskmon::items, num_items));
    
    pids_stack *stack = nullptr;
    
    // Get the process stack and feed it to our method
    bool found = false;
    while ((stack = procps_pids_get(info, PIDS_FETCH_TASKS_ONLY))) {
        int tid = PIDS_VAL(taskmon::IDX_TID, s_int, stack, info);
        if (tid > 0) { // Just take the first valid process we find
            task.processProcStack(true, stats, stack, info); // watch_all = true
            found = true;
            break;
        }
    }
    
    procps_pids_unref(&info);
    
    ASSERT_TRUE(found) << "No valid processes found by libproc2.";
    ASSERT_EQ(1, stats.tasks.size());
    EXPECT_GT(stats.tasks[0].tid, 0);
}

TEST(TaskTest, processProcStack_WatchFalse_IgnoresUnregisteredProcess) {
    TestableTask task("test_task");
    taskmon::TaskStats stats;
    
    pids_info *info = nullptr;
    int num_items = sizeof(taskmon::items) / sizeof(enum pids_item);
    ASSERT_EQ(0, procps_pids_new(&info, taskmon::items, num_items));
    
    pids_stack *stack = procps_pids_get(info, PIDS_FETCH_TASKS_ONLY);
    ASSERT_NE(stack, nullptr);
    
    // watch_all = false, and we haven't called task.watch()
    task.processProcStack(false, stats, stack, info);
    
    procps_pids_unref(&info);
    
    // Should be empty because it wasn't in the watch list
    EXPECT_EQ(0, stats.tasks.size());
}
#endif
