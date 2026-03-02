
#include "ProcessModel.h"
#include <algorithm>



ProcessModel::~ProcessModel() = default;

void WaterTank::setParams(float radius, float height, float maxInflow, float maxOutflow) {
    radius_ = radius;
    height_ = height;
    maxInflow_ = maxInflow;
    area_ = 3.14f * radius_ * radius_; // A = πr²
    PV_ = 0.0f; // start empty
    // convert cubic millimetres to litres: 1 L = 1e6 mm³
    volume_ = area_ * height_ * 0.000001f; // V in litres
    reset();
}
void WaterTank::reset() {
    PV_ = 0.0f; // start empty 
    WaterLevel_ = 0.0f; 
    inflow      = 0.0f;
    outflow     = 0.0f;
    rampUp_     = true;

}

float WaterTank::update(float *u, float *dt) {
    //outflow is ramped up or down to simulate changes in water level.

    float dtminutes = (*dt) / 60.0f; // convert time step from seconds to minutes for flow rates in liters per minute

    if (outflow >= maxOutflow_) {
        rampUp_ = false;
        outflow = maxOutflow_;
    } else if (outflow <= 0.0f) {
        rampUp_ = true;
        outflow = 0.0f;
    };
     if (rampUp_) {
        outflow += 0.1f; // ramp up outflow, liters per second
    } else {
        outflow -= 0.1f; // ramp down outflow, liters per second
    };

// Inflow is scaled based on the control input u.
    if (*u > 0.0f) {
        inflow = maxInflow_/(*u)*100.0f; // scale inflow to percentage of max inflow
    }
    inflow = std::clamp(*u, 0.0f, maxInflow_); // ensure inflow is within bounds

    float dVolume = (inflow - outflow) * dtminutes; // change in volume liters = (inflow - outflow) * time step in seconds
    WaterLevel_ = std::clamp(WaterLevel_ + dVolume, 0.0f, volume_); // keep within 

    if (height_ > 0.0f && volume_ > 0.0f) {
        PV_ = (WaterLevel_ / volume_) * height_;     // mm
        PV_ = std::clamp(PV_, 0.0f, height_);
    } else {
        PV_ = 0.0f;
    }

    return PV_;
}
























