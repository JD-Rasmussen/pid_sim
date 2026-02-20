
#include "ProcessModel.h"
#include <algorithm>



void WaterTank::setParams(float radius, float height, float maxInflow, float maxOutflow) {
    radius_ = radius;
    height_ = height;
    maxInflow_ = maxInflow;
    area_ = 3.14f * radius_ * radius_; // A = πr²
    PV_ = 0.0f; // start empty
    volume_ = area_ * height_; // V = A * h
    reset();
}

void WaterTank::reset() {
    PV_ = 0.0f; // start empty
}

float WaterTank::update(float *u, float *dt) {
    float inflow = std::min(maxInflow_, std::max(0.0f, *u)); // inflow rate (e.g., liters per second)
    
    float outflow = maxOutflow_*0.5f; // outflow proportional to current level (e.g., due to gravity)

    float dVolume = (inflow - outflow * 0.001f) * (*dt); // change in volume liters = (inflow - outflow) * time step in seconds
    WaterLevel_ += dVolume; // update water level in liters
    if (PV_ <= 0.0f && dVolume < 0.0f) {
        PV_ += (dVolume / volume_) * height_; // update water level in mm based on volume change
        PV_ = std::max(0.0f, std::min(height_, PV_)); // prevent negative water level and ensure it doesn't exceed tank height
    };

    return PV_;
}
























