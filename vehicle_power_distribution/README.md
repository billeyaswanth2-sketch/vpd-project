# Vehicle Power Distribution — C++ / Google Test

Converted from CAPL (CANoe) to C++ with Google Test.

## Project Structure

```
vehicle_power_distribution/
├── src/
│   ├── sys_vars.h / .cpp        ← shared sysvar:: namespace
│   ├── bcm.h / .cpp             ← Bcm.can
│   ├── engine.h / .cpp          ← engine.can
│   └── power_transmission.h/.cpp← power_transmission.can
├── tests/
│   └── test_vehicle.cpp         ← testcases.can (all 3 test cases + integration)
├── CMakeLists.txt
└── .vscode/
    ├── settings.json
    └── tasks.json
```

## CAPL → C++ Mapping

| CAPL | C++ |
|------|-----|
| `sysvar::ignition::ignition` | `sysvar.ignition` (global struct) |
| `on sysvar X { }` | `Module::onSysVarX()` method |
| `on timer X { }` | `Module::onTimer()` method |
| `@brake::brake` | `sysvar.brake` |
| `output(msg)` | message struct updated in `onTimer()` |
| `testStepPass / Fail` | `EXPECT_EQ / EXPECT_TRUE` |
| `testWaitForSysVar` | direct sysvar assignment (no async needed) |

## Prerequisites

- CMake ≥ 3.14
- A C++17 compiler (MSVC / GCC / Clang)
- Internet access on first build (downloads GoogleTest automatically)

## VS Code Extensions to Install

1. **C/C++** (ms-vscode.cpptools)
2. **CMake Tools** (ms-vscode.cmake-tools)
3. **Test Explorer UI** (hbenl.vscode-test-explorer)
4. **C++ TestMate** (matepek.vscode-catch2-test-adapter)

## Build & Run

```bash
# From repo root
cmake -S . -B build
cmake --build build
cd build && ctest --output-on-failure
```

Or in VS Code: `Ctrl+Shift+P` → **Tasks: Run Test Task**

The Testing panel (flask icon in sidebar) shows each test pass/fail inline.

## Test Cases Covered

### tc_brake
- ✅ Brake pressed (sysvar=1) → internal state=1, log="brake is pressed"
- ✅ Brake released (sysvar=0) → internal state=0
- ✅ Timer outputs correct brake_signal to CAN message

### tc_gear
- ✅ Steps 1-6: Neutral, 1st, 2nd, 3rd, 4th, 5th gear
- ✅ Reverse gear
- ✅ Timer outputs gear_signal to CAN message

### tc_ignition_key
- ✅ Step 1: ignition=0 → Parking/Lock mode
- ✅ Step 2: ignition=1 → Accessory mode (engine OFF)
- ✅ Step 3: ignition=2 → ON mode (engine OFF)
- ✅ Step 4: ignition=3 + brake=1 + gear=0 → Start mode, engine sysvar=1
- ✅ Guard: no start if brake not pressed
- ✅ Guard: no start if gear not in Neutral

### Engine module
- ✅ Turns ON only when engine=1 + gear=0 + brake=1
- ✅ Stays OFF if gear not Neutral or brake not pressed
- ✅ Timer propagates engine_state to CAN message

### Integration
- ✅ Full sequence: brake → neutral → ignition start → engine ON → all CAN messages correct
