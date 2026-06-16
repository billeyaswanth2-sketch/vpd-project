#include "engine.h"

// CAPL original:
//   on sysvar engine::engine {
//     if (@engine::engine==1 && @gear::gear==0 && @brake::brake==1)
//       engine_state=1; write("engine_ON");
//     else
//       engine_state=0; write("engine_OFF");
//   }
void Engine::onSysVarEngine() {
    if (sysvar.engine == 1 && sysvar.gear == 0 && sysvar.brake == 1) {
        engine_state = 1;
        last_log = "engine_ON";
    } else {
        engine_state = 0;
        last_log = "engine_OFF";
    }
}

// CAPL original:
//   on timer engine_time {
//     engine_msg.engine_signal = engine_state;
//     output(engine_msg);
//   }
void Engine::onTimer() {
    engine_msg.engine_signal = engine_state;
    // output(engine_msg) → message is now readable via engine_msg
}

void Engine::onStart() {
    // setTimerCyclic(engine_time, 200) → handled by test harness
}
