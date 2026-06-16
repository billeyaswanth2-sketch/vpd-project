#include "power_transmission.h"

// CAPL original:
//   on sysvar brake::brake {
//     if (@brake::brake==1) { brake=1; write("brake is pressed"); }
//     else                  { brake=0; write("brake is unpressed"); }
//   }
void PowerTransmission::onSysVarBrake() {
    if (sysvar.brake == 1) {
        brake    = 1;
        last_log = "brake is pressed";
    } else {
        brake    = 0;
        last_log = "brake is unpressed";
    }
}

// CAPL original:
//   on sysvar gear::gear { if/else chain 0-6 }
void PowerTransmission::onSysVarGear() {
    switch (sysvar.gear) {
        case 1:  gear = 1; last_log = "first gear";   break;
        case 2:  gear = 2; last_log = "second gear";  break;
        case 3:  gear = 3; last_log = "third gear";   break;
        case 4:  gear = 4; last_log = "fourth gear";  break;
        case 5:  gear = 5; last_log = "fifth gear";   break;
        case 6:  gear = 6; last_log = "Reverse gear"; break;
        default: gear = 0; last_log = "Neutral";      break;
    }
}

// CAPL original:
//   on timer powertrans_time {
//     powertrans_msg.brake_signal = brake;
//     powertrans_msg.gear_signal  = gear;
//     output(powertrans_msg);
//   }
void PowerTransmission::onTimer() {
    powertrans_msg.brake_signal = brake;
    powertrans_msg.gear_signal  = gear;
}

void PowerTransmission::onStart() {
    // setTimerCyclic(powertrans_time, 200) → handled by test harness
}
