// testpid.cpp
#include <cassert>
#include <cmath>
#include <iostream>
#include "pid.h" 

// Helper for approximate float compare
static bool approx(float a, float b, float eps = 1e-3f) {
    return std::fabs(a - b) <= eps;
}

static void test_proportional_and_clamp() {
    Params p;
    p.SP = 10.0f;
    p.Kp = 2.0f;
    p.Tn = 0.0f;
    p.Td = 0.0f;
    p.outputMin = 0.0f;
    p.outputMax = 100.0f;

    PID pid{p};

    // P-only inside limits
    {
        float PV = 5.0f;          // e = 5
        float u  = pid.update(PV, 0.1f);
        assert(approx(u, 10.0f)); // 2 * 5
    }

    // Clamp to max
    {
        float PV = -50.0f; // e = 60 -> P = 120
        float u  = pid.update(PV, 0.1f);
        assert(approx(u, 100.0f));
    }

    // Clamp to min
    {
        float PV = 250.0f; // e = -240 -> P = -480
        float u  = pid.update(PV, 0.1f);
        assert(approx(u, 0.0f));
    }
}

int main() {
    test_proportional_and_clamp();
    std::cout << "[OK] Minimal test(s) passed.\n";
    return 0;
}