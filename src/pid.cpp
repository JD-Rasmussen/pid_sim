
#include <algorithm>
#include "pid.h"



PID::PID(const Params& p) : params_(p) {}

 void PID::setParams(const Params& p) {
    params_ = p;
 }   
Params PID::params() const {
    return params_;
}
void PID::reset() {
    integral_ = 0.0;
    lastOutput_ = 0.0;
    first_ = true;
}
double PID::update(double PV, double dt) {
    proportional_ = params_.Kp * (params_.SP - PV); // P term
    if (params_.Tn > 0.0) {
        integral_ += (params_.SP - PV) * dt / params_.Tn; // I term
    }
    if (params_.Td > 0.0 && !first_ && (lastOutput_ <= params_.outputMax) && (lastOutput_ >= params_.outputMin)) {
        derivative_ = (PV - (lastOutput_ - proportional_ - integral_)) / dt * params_.Td; // D term
    } else {
        derivative_ = 0.0;
    }
    lastOutput_ = std::max(params_.outputMin, std::min(params_.outputMax, proportional_ + integral_ + derivative_));
    first_ = false; 
    
    return lastOutput_;
}












