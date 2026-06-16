#include "bcm.h"

// CAPL original:
//   on sysvar ignition::ignition {
//     if (@ignition::ignition==1)                                    → Accessory
//     else if (@ignition::ignition==2)                               → ON
//     else if (@ignition::ignition==3 && brake==1 && gear==0)        → Start
//     else                                                           → Parking
//   }
void BCM::onSysVarIgnition() {
    if (sysvar.ignition == 1) {
        engine_signal = 0;
        ignition      = 1;
        last_log      = "Accessory mode";
    } else if (sysvar.ignition == 2) {
        engine_signal = 0;
        ignition      = 2;
        last_log      = "ignition MODE";
    } else if (sysvar.ignition == 3 && sysvar.brake == 1 && sysvar.gear == 0) {
        sysvar.engine  = 1;   // replaces @engine::engine = 1
        ignition       = 3;
        last_log       = "start mode";
    } else {
        engine_signal = 0;
        ignition      = 0;
        last_log      = "parking mode";
    }
}

// CAPL original:
//   on timer vehiclepower_time {
//     vehiclepower_msg.ignition_signal = ignition;
//     output(vehiclepower_msg);
//   }
void BCM::onTimer() {
    vehiclepower_msg.ignition_signal = ignition;
}

void BCM::onStart() {
    // setTimerCyclic(vehiclepower_time, 200) → handled by test harness
}
