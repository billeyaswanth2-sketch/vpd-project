#include <gtest/gtest.h>
#include "../src/sys_vars.h"
#include "../src/bcm.h"
#include "../src/engine.h"
#include "../src/power_transmission.h"

// ─────────────────────────────────────────────────────────────────────────────
// Test fixture: resets all sysvars before every test (replaces CAPL's
// testWaitForTimeout + fresh environment between test cases)
// ─────────────────────────────────────────────────────────────────────────────
class VehicleTest : public ::testing::Test {
protected:
    BCM              bcm;
    Engine           engine;
    PowerTransmission pt;

    void SetUp() override {
        sysvar = SysVars{};   // reset all to 0
    }
};

// =============================================================================
// tc_brake  (CAPL testcase tc_brake)
// =============================================================================

// CAPL test step 1: setsysvar=1 → brake pressed
TEST_F(VehicleTest, Brake_Step1_BrakePressed) {
    sysvar.brake = 1;
    pt.onSysVarBrake();

    EXPECT_EQ(pt.brake, 1)        << "brake internal state should be 1";
    EXPECT_EQ(pt.last_log, "brake is pressed");
}

// Extra: verify brake=0 (released) is handled
TEST_F(VehicleTest, Brake_Released) {
    sysvar.brake = 0;
    pt.onSysVarBrake();

    EXPECT_EQ(pt.brake, 0);
    EXPECT_EQ(pt.last_log, "brake is unpressed");
}

// Timer propagates brake state to CAN message
TEST_F(VehicleTest, Brake_TimerOutputsCorrectSignal) {
    sysvar.brake = 1;
    pt.onSysVarBrake();
    pt.onTimer();

    EXPECT_EQ(pt.powertrans_msg.brake_signal, 1);
}

// =============================================================================
// tc_gear  (CAPL testcase tc_gear — steps 1-6)
// =============================================================================

// Step 1: gear=0 → Neutral
TEST_F(VehicleTest, Gear_Step1_Neutral) {
    sysvar.gear = 0;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 0);
    EXPECT_EQ(pt.last_log, "Neutral");
}

// Step 2: gear=1 → First gear
TEST_F(VehicleTest, Gear_Step2_FirstGear) {
    sysvar.gear = 1;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 1);
    EXPECT_EQ(pt.last_log, "first gear");
}

// Step 3: gear=2 → Second gear
TEST_F(VehicleTest, Gear_Step3_SecondGear) {
    sysvar.gear = 2;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 2);
    EXPECT_EQ(pt.last_log, "second gear");
}

// Step 4: gear=3 → Third gear
TEST_F(VehicleTest, Gear_Step4_ThirdGear) {
    sysvar.gear = 3;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 3);
    EXPECT_EQ(pt.last_log, "third gear");
}

// Step 5: gear=4 → Fourth gear
TEST_F(VehicleTest, Gear_Step5_FourthGear) {
    sysvar.gear = 4;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 4);
    EXPECT_EQ(pt.last_log, "fourth gear");
}

// Step 6: gear=5 → Fifth gear
TEST_F(VehicleTest, Gear_Step6_FifthGear) {
    sysvar.gear = 5;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 5);
    EXPECT_EQ(pt.last_log, "fifth gear");
}

// Extra: gear=6 → Reverse
TEST_F(VehicleTest, Gear_ReverseGear) {
    sysvar.gear = 6;
    pt.onSysVarGear();

    EXPECT_EQ(pt.gear, 6);
    EXPECT_EQ(pt.last_log, "Reverse gear");
}

// Timer propagates gear state to CAN message
TEST_F(VehicleTest, Gear_TimerOutputsCorrectSignal) {
    sysvar.gear = 3;
    pt.onSysVarGear();
    pt.onTimer();

    EXPECT_EQ(pt.powertrans_msg.gear_signal, 3);
}

// =============================================================================
// tc_ignition_key  (CAPL testcase tc_ignition_key — steps 1-4)
// =============================================================================

// Step 1: ignition=0 → Lock/Parking mode
TEST_F(VehicleTest, Ignition_Step1_LockMode) {
    sysvar.ignition = 0;
    bcm.onSysVarIgnition();

    EXPECT_EQ(bcm.ignition, 0);
    EXPECT_EQ(bcm.last_log, "parking mode");
}

// Step 2: ignition=1 → Accessory mode
TEST_F(VehicleTest, Ignition_Step2_AccessoryMode) {
    sysvar.ignition = 1;
    bcm.onSysVarIgnition();

    EXPECT_EQ(bcm.ignition, 1);
    EXPECT_EQ(bcm.engine_signal, 0)  << "engine should be OFF in accessory mode";
    EXPECT_EQ(bcm.last_log, "Accessory mode");
}

// Step 3: ignition=2 → ON mode
TEST_F(VehicleTest, Ignition_Step3_ONMode) {
    sysvar.ignition = 2;
    bcm.onSysVarIgnition();

    EXPECT_EQ(bcm.ignition, 2);
    EXPECT_EQ(bcm.engine_signal, 0)  << "engine should be OFF in ignition ON mode";
    EXPECT_EQ(bcm.last_log, "ignition MODE");
}

// Step 4: ignition=3 + brake=1 + gear=0 → Start mode, engine turns ON
TEST_F(VehicleTest, Ignition_Step4_StartMode_EngineStarts) {
    sysvar.brake    = 1;
    sysvar.gear     = 0;
    sysvar.ignition = 3;
    bcm.onSysVarIgnition();

    EXPECT_EQ(bcm.ignition, 3);
    EXPECT_EQ(sysvar.engine, 1)  << "engine sysvar should be set to 1 (ON)";
    EXPECT_EQ(bcm.last_log, "start mode");
}

// Step 4 guard: ignition=3 but brake NOT pressed → should NOT start engine
TEST_F(VehicleTest, Ignition_Step4_StartMode_NoBrake_EngineShouldNotStart) {
    sysvar.brake    = 0;   // brake not pressed
    sysvar.gear     = 0;
    sysvar.ignition = 3;
    bcm.onSysVarIgnition();

    EXPECT_NE(bcm.ignition, 3)       << "start mode must not activate without brake";
    EXPECT_EQ(sysvar.engine, 0)      << "engine must remain OFF";
}

// Step 4 guard: ignition=3 but gear NOT in Neutral → should NOT start engine
TEST_F(VehicleTest, Ignition_Step4_StartMode_GearNotNeutral_EngineShouldNotStart) {
    sysvar.brake    = 1;
    sysvar.gear     = 2;   // in 2nd gear
    sysvar.ignition = 3;
    bcm.onSysVarIgnition();

    EXPECT_NE(bcm.ignition, 3)  << "start mode must not activate unless gear is Neutral";
    EXPECT_EQ(sysvar.engine, 0) << "engine must remain OFF";
}

// BCM timer outputs correct ignition signal to CAN message
TEST_F(VehicleTest, BCM_TimerOutputsIgnitionSignal) {
    sysvar.ignition = 2;
    bcm.onSysVarIgnition();
    bcm.onTimer();

    EXPECT_EQ(bcm.vehiclepower_msg.ignition_signal, 2);
}

// =============================================================================
// Engine module tests
// =============================================================================

// Engine turns ON when engine=1, gear=0, brake=1
TEST_F(VehicleTest, Engine_TurnsOn_WhenConditionsMet) {
    sysvar.engine = 1;
    sysvar.gear   = 0;
    sysvar.brake  = 1;
    engine.onSysVarEngine();

    EXPECT_EQ(engine.engine_state, 1);
    EXPECT_EQ(engine.last_log, "engine_ON");
}

// Engine stays OFF when gear is not Neutral
TEST_F(VehicleTest, Engine_StaysOff_WhenGearNotNeutral) {
    sysvar.engine = 1;
    sysvar.gear   = 1;   // in 1st gear
    sysvar.brake  = 1;
    engine.onSysVarEngine();

    EXPECT_EQ(engine.engine_state, 0);
    EXPECT_EQ(engine.last_log, "engine_OFF");
}

// Engine stays OFF when brake not pressed
TEST_F(VehicleTest, Engine_StaysOff_WhenBrakeNotPressed) {
    sysvar.engine = 1;
    sysvar.gear   = 0;
    sysvar.brake  = 0;
    engine.onSysVarEngine();

    EXPECT_EQ(engine.engine_state, 0);
}

// Engine timer propagates state to CAN message
TEST_F(VehicleTest, Engine_TimerOutputsEngineSignal) {
    sysvar.engine = 1;
    sysvar.gear   = 0;
    sysvar.brake  = 1;
    engine.onSysVarEngine();
    engine.onTimer();

    EXPECT_EQ(engine.engine_msg.engine_signal, 1);
}

// =============================================================================
// Integration: full start sequence (BCM → engine sysvar → Engine module)
// =============================================================================
TEST_F(VehicleTest, Integration_FullStartSequence) {
    // 1. Brake pressed
    sysvar.brake = 1;
    pt.onSysVarBrake();
    EXPECT_EQ(pt.brake, 1);

    // 2. Gear in Neutral
    sysvar.gear = 0;
    pt.onSysVarGear();
    EXPECT_EQ(pt.gear, 0);

    // 3. Turn ignition to Start
    sysvar.ignition = 3;
    bcm.onSysVarIgnition();
    EXPECT_EQ(bcm.ignition, 3);
    EXPECT_EQ(sysvar.engine, 1) << "BCM must set engine sysvar to 1";

    // 4. Engine module reacts to engine sysvar
    engine.onSysVarEngine();
    EXPECT_EQ(engine.engine_state, 1) << "Engine module must turn ON";

    // 5. CAN messages all carry correct signals after timer ticks
    bcm.onTimer();
    engine.onTimer();
    pt.onTimer();

    EXPECT_EQ(bcm.vehiclepower_msg.ignition_signal,    3);
    EXPECT_EQ(engine.engine_msg.engine_signal,          1);
    EXPECT_EQ(pt.powertrans_msg.brake_signal,           1);
    EXPECT_EQ(pt.powertrans_msg.gear_signal,            0);
}
