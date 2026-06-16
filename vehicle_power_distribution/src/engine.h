#pragma once
#include "sys_vars.h"
#include <string>

// Converted from: engine.can
// CAPL: on sysvar engine::engine  → Engine::onSysVarEngine()
// CAPL: on timer engine_time      → Engine::onTimer()  (called every 200ms cycle)
// CAPL: on start                  → Engine::onStart()

struct EngineMessage {
    int engine_signal = 0;
};

class Engine {
public:
    int         engine_state = 0;
    EngineMessage engine_msg;
    std::string last_log;

    // Called when sysvar::engine::engine changes
    // Requires: gear==0 AND brake==1 to turn engine ON
    void onSysVarEngine();

    // Called every 200ms timer tick — copies state into CAN message
    void onTimer();

    // Initialises cyclic timer (no-op in unit tests; timer is mocked)
    void onStart();
};
