#pragma once

#include <vector>
#include <array>

struct SimulationData {
    std::vector<float> timeSteps;   // Time values for x-axis
    std::vector<float> pvValues;    // Process Variable values
    std::vector<float> uValues;     // Control signal values
};

class SimulationLogger {
public:
    SimulationLogger() = default;

    void addDataPoint(float timeStep, float pv, float u) {
        data_.timeSteps.push_back(timeStep);
        data_.pvValues.push_back(pv);
        data_.uValues.push_back(u);
    }

    void reset() {
        data_.timeSteps.clear();
        data_.pvValues.clear();
        data_.uValues.clear();
    }

    const SimulationData& data() const {
        return data_;
    }

    SimulationData& data() {
        return data_;
    }

private:
    SimulationData data_;
};
