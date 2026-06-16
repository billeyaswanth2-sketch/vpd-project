#pragma once
#include "sys_vars.h"
#include <string>

// Converted from: Bcm.can
// CAPL: on sysvar ignition::ignition → BCM::onSysVarIgnition()
// CAPL: on timer vehiclepower_time   → BCM::onTimer()

struct VehiclePowerMessage {
    int ignition_signal = 0;
};

class BCM {
public:
    int ignition     = 0;
    int engine_signal = 0;   // replaces $engine_signal (environment variable)
    VehiclePowerMessage vehiclepower_msg;
    std::string last_log;

    // Called when sysvar::ignition::ignition changes
    // Mode mapping:
    //   0 = Parking/Lock
    //   1 = Accessory
    //   2 = Ignition ON
    //   3 = Start (requires brake==1 AND gear==0)
    void onSysVarIgnition();

    // Called every 200ms timer tick
    void onTimer();

    void onStart();
};
