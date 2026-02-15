#include <gtest/gtest.h>
#include "pid.h"

// Helper: run PID for N steps with constant PV
static float run_for_steps(PID& pid, float PV, float dt, int steps) {
    float u = 0.0f;
    for (int i = 0; i < steps; ++i) u = pid.update(PV, dt);
    return u;
}

TEST(PID, ProportionalOnlyAndClamp) {
    Params p;
    p.SP = 10.0f;
    p.Kp = 2.0f;
    p.Tn = 0.0f; // I disabled
    p.Td = 0.0f; // D disabled
    p.outputMin = 0.0f;
    p.outputMax = 100.0f;

    PID pid{p};

    // P term inside range
    EXPECT_FLOAT_EQ(pid.update(/*PV=*/5.0f, /*dt=*/0.1f), 10.0f);

    // Clamp high
    EXPECT_FLOAT_EQ(pid.update(/*PV=*/-50.0f, 0.1f), 100.0f);

    // Clamp low
    EXPECT_FLOAT_EQ(pid.update(/*PV=*/250.0f, 0.1f), 0.0f);
}

TEST(PID, IntegralAfterTnApproxEqualsP) {
    Params p;
    p.SP = 1.0f;
    p.Kp = 4.0f;
    p.Tn = 10.0f;   // seconds
    p.Td = 0.0f;
    p.outputMin = -1e9f;
    p.outputMax = +1e9f;

    PID pid{p};
    const float PV = 0.5f;                 // e = 0.5
    const float P  = p.Kp * (p.SP - PV);   // = 2.0

    const float dt = 0.1f;
    const int steps = static_cast<int>(p.Tn / dt + 0.5f); // ≈ 10 s

    float u = run_for_steps(pid, PV, dt, steps);

    // After ~Tn seconds at constant error: I ≈ P -> u ≈ 2P
    EXPECT_NEAR(u, 2.0f * P, 2e-2f);
}

TEST(PID, DtInvarianceSameTotalTimeSameOutput) {
    Params p;
    p.SP = 1.0f; p.Kp = 2.0f; p.Tn = 20.0f; p.Td = 0.0f;
    p.outputMin = -1e9f; p.outputMax = +1e9f;

    PID a{p}, b{p};
    const float PV = 0.8f;

    float ua = run_for_steps(a, PV, 0.01f, 1000); // 10 s
    float ub = run_for_steps(b, PV, 0.5f,    20); // 10 s

    EXPECT_NEAR(ua, ub, 1e-2f);
}