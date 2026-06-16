#pragma once
#include "sys_vars.h"
#include <string>

// Converted from: power_transmission.can
// CAPL: on sysvar brake::brake  → PowerTransmission::onSysVarBrake()
// CAPL: on sysvar gear::gear    → PowerTransmission::onSysVarGear()
// CAPL: on timer powertrans_time→ PowerTransmission::onTimer()

struct PowerTransMessage {
    int brake_signal = 0;
    int gear_signal  = 0;
};

class PowerTransmission {
public:
    int brake = 0;
    int gear  = 0;
    PowerTransMessage powertrans_msg;
    std::string last_log;

    // Called when sysvar::brake::brake changes
    void onSysVarBrake();

    // Called when sysvar::gear::gear changes
    void onSysVarGear();

    // Called every 200ms timer tick
    void onTimer();

    void onStart();
};
