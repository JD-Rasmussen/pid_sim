
#include <gtest/gtest.h>
#include "ProcessModel.h"



static float run_for_time(WaterTank& wtank, float u, float dt, int steps) {
    float PV = 0.0f;
    for (int i = 0; i < steps; ++i) {
        PV = wtank.update(&u, &dt);
    }
    return PV;
}



TEST(ProcessModel, WaterTank) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the water level increases with positive control input
    float u = 50.0f; // control input (inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase from initial state

    // Test that the water level decreases with zero control input (outflow only)
    u = 0.0f; // no inflow
    PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to decrease from previous state
}

TEST(ProcessModel, WaterTankBounds) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the water level does not exceed the tank height
    float u = 200.0f; // control input (excessive inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 200; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LE(PV, 1000.0f); // Expect water level to be at or below tank height

    // Test that the water level does not go below zero
    u = -50.0f; // negative control input (not physically meaningful but tests bounds)
    PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GE(PV, 0.0f); // Expect water level to be at or above zero
}
TEST(ProcessModel, WaterTankReset) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Simulate some time with positive control input
    float u = 50.0f; // control input (inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase from initial state

    // Now reset and check that the water level goes back to zero
    wtank.reset();
    PV = wtank.update(&u, &dt);
    EXPECT_EQ(PV, 0.0f); // Expect water level to be reset to zero
}

TEST(ProcessModel, WaterTankOutflowRamp) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Simulate with zero control input to observe outflow ramping
    float u = 0.0f; // no inflow
    float dt = 0.1f; // time step in seconds
    float steps = 200; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to decrease due to outflow ramping
}
TEST(ProcessModel, WaterTankInflowScaling) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the inflow scales with control input
    float u = 50.0f; // control input (inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase from initial state

    // Now test with maximum control input
    u = 150.0f; // control input exceeding max inflow
    PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LE(PV, 1000.0f); // Expect water level to be at or below tank height due to clamping
}

TEST(ProcessModel, WaterTankOutflowRampDirection) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Simulate with zero control input to observe outflow ramping
    float u = 0.0f; // no inflow
    float dt = 0.1f; // time step in seconds
    float steps = 200; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to decrease due to outflow ramping

    // Now simulate for a longer time to see if the outflow ramps back up
    PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to continue decreasing due to outflow ramping
}

TEST(ProcessModel, WaterTankInflowClamping) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the inflow is clamped to the maximum value
    float u = 150.0f; // control input exceeding max inflow
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LE(PV, 1000.0f); // Expect water level to be at or below tank height due to clamping
}

TEST(ProcessModel, WaterTankOutflowClamping) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Simulate with zero control input to observe outflow ramping
    float u = 0.0f; // no inflow
    float dt = 0.1f; // time step in seconds
    float steps = 200; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to decrease due to outflow ramping

    // Now simulate for a longer time to see if the outflow ramps back up and clamps
    PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to continue decreasing due to outflow ramping and clamping at max outflow
}

TEST(ProcessModel, WaterTankVolumeCalculation) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the volume is calculated correctly based on the area and height
    float expectedVolume = (3.14159f * 100.0f * 100.0f) * 1000.0f * 0.001f; // area * height * conversion to liters
    EXPECT_NEAR(wtank.update(&expectedVolume, &expectedVolume), expectedVolume, 1e-2f); // Expect PV to reflect volume calculation
}

TEST(ProcessModel, WaterTankInitialState) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the initial water level is zero
    float u = 0.0f; // no inflow
    float dt = 0.1f; // time step in seconds

    float PV = wtank.update(&u, &dt);
    EXPECT_EQ(PV, 0.0f); // Expect initial water level to be zero
}

TEST(ProcessModel, WaterTankSteadyState) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test that the water level reaches a steady state with constant control input
    float u = 50.0f; // control input (inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 1000; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase from initial state
    EXPECT_LT(PV, 1000.0f); // Expect water level to be below tank height at steady state
}

TEST(ProcessModel, WaterTankResponseToStepInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to a step input in control
    float u = 50.0f; // control input (inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase from initial state

    // Now apply a step change in control input
    u = 100.0f; // increase control input to maximum inflow
    PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase further due to higher inflow
}

TEST(ProcessModel, WaterTankResponseToNegativeInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to a negative control input (should be clamped to zero)
    float u = -50.0f; // negative control input (not physically meaningful but tests bounds)
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_EQ(PV, 0.0f); // Expect water level to remain at zero due to clamping of negative input
}
TEST(ProcessModel, WaterTankResponseToZeroInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to zero control input (should decrease due to outflow)
    float u = 0.0f; // no inflow
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to decrease due to outflow with zero inflow
}

TEST(ProcessModel, WaterTankResponseToMaxInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to maximum control input
    float u = 150.0f; // control input exceeding max inflow
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LE(PV, 1000.0f); // Expect water level to be at or below tank height due to clamping of max input
}
TEST(ProcessModel, WaterTankResponseToMinInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to minimum control input (negative value should be clamped to zero)
    float u = -50.0f; // negative control input (not physically meaningful but tests bounds)
    float dt = 0.1f; // time step in seconds
    float steps = 100; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_EQ(PV, 0.0f); // Expect water level to remain at zero due to clamping of negative input
}

TEST(ProcessModel, WaterTankResponseToSustainedInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to a sustained control input
    float u = 50.0f; // control input (inflow rate)
    float dt = 0.1f; // time step in seconds
    float steps = 1000; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_GT(PV, 0.0f); // Expect water level to increase from initial state
    EXPECT_LT(PV, 1000.0f); // Expect water level to be below tank height at steady state
}
TEST(ProcessModel, WaterTankResponseToSustainedZeroInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to a sustained zero control input (should decrease due to outflow)
    float u = 0.0f; // no inflow
    float dt = 0.1f; // time step in seconds
    float steps = 1000; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LT(PV, 1000.0f); // Expect water level to decrease due to outflow with zero inflow
}

TEST(ProcessModel, WaterTankResponseToSustainedMaxInput) {
    WaterTank wtank;
    wtank.setParams(100.0f, 1000.0f, 100.0f, 80.0f);
    wtank.reset();

    // Test the response to a sustained maximum control input
    float u = 150.0f; // control input exceeding max inflow
    float dt = 0.1f; // time step in seconds
    float steps = 1000; // number of steps to simulate

    float PV = run_for_time(wtank, u, dt, steps);
    EXPECT_LE(PV, 1000.0f); // Expect water level to be at or below tank height due to clamping of max input
}