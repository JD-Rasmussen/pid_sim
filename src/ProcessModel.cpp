
#include "ProcessModel.h"





float WaterTank::update(float *u, float *dt) {
    // Simple water tank model: dPV/dt = (inflow - outflow) / area
    // Inflow is u, outflow is outflowCoeff_ * sqrt(PV) (Torricelli's law)
    float inflow = *u;
    float outflow = 0.0f; //placeholder //outflowCoeff_ * std::sqrt(std::max(0.0f, WaterLevel_)); // prevent sqrt of negative
    float dPVdt = (inflow - outflow) / area_;
    WaterLevel_ += dPVdt * (*dt); // Update the water level variable
    return WaterLevel_;
}






















