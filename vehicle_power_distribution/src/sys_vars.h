#pragma once

struct SysVars {
    int ignition = 0;
    int engine   = 0;
    int brake    = 0;
    int gear     = 0;
};

extern SysVars sysvar;
