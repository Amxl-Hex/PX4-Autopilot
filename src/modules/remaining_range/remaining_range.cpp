// src/modules/remaining_range/remaining_range.cpp
#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <lib/mathlib/mathlib.h>

using namespace time_literals;

class RemainingRange : public ModuleBase<RemainingRange>, public px4::ScheduledWorkItem
{
public:
    RemainingRange();
    ~RemainingRange() override;

    static int task_spawn(int argc, char *argv[]);
    static int print_usage(const char *reason = nullptr);

    bool init();

private:
    void Run() override; // ← Called by work queue

    bool _should_exit{false};
    static constexpr float SCALE_FACTOR_M_PER_PERCENT = 100.0f;
    uORB::Subscription _battery_sub{ORB_ID(battery_status)};
};

RemainingRange::RemainingRange()
    : ModuleBase<RemainingRange>(),
      ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::nav_and_controllers)
{
}

RemainingRange::~RemainingRange()
{
    Stop();
}

bool RemainingRange::init()
{
    ScheduleOnInterval(1_s); // Run every 1 second
    return true;
}

void RemainingRange::Run()
{
    if (_should_exit) {
        ScheduleClear();
        return;
    }

    if (_battery_sub.updated()) {
        battery_status_s battery{};
        _battery_sub.copy(&battery);

        if (battery.connected && battery.remaining >= 0.0f && battery.remaining <= 100.0f) {
            float range_m = battery.remaining * SCALE_FACTOR_M_PER_PERCENT;
            // 👇 This WILL appear in your Linux terminal
            PX4_INFO("Remaining range = %.1f m", (double)range_m);
        }
    }
}

int RemainingRange::task_spawn(int argc, char *argv[])
{
    RemainingRange *instance = new RemainingRange();

    if (!instance) {
        PX4_ERR("alloc failed");
        return -ENOMEM;
    }

    _object.store(instance);
    _task_id = task_id_is_work_queue;

    if (instance->init()) {
        return PX4_OK;
    }

    delete instance;
    _object.store(nullptr);
    _task_id = -1;
    return PX4_ERROR;
}

int RemainingRange::print_usage(const char *reason)
{
    if (reason) {
        PX4_WARN("%s\n", reason);
    }
    PRINT_MODULE_DESCRIPTION("Background range estimator.");
    PRINT_MODULE_USAGE_NAME("remaining_range", "estimator");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_COMMAND("stop");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
    return 0;
}

extern "C" __EXPORT int remaining_range_main(int argc, char *argv[])
{
    return RemainingRange::main(argc, argv,
                                RemainingRange::task_spawn,
                                nullptr,
                                RemainingRange::print_usage);
}
