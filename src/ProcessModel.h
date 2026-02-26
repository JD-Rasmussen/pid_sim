
#pragma once



class ProcessModel{

    public:

        virtual void reset();
        virtual float update(float *u, float *dt);

    private:
        float PV_; // Process Variable returned by the model

};



class WaterTank : public ProcessModel {

    public:

        void setParams(float radius, float height, float maxInflow, float maxOutflow);
        void reset() override;
        float update(float *u, float *dt) override;

    private:
        float maxInflow_{100.0f}; // maximum inflow rate  liters per minute
        float maxOutflow_{80.0f}; // maximum outflow rate liters per minute
        float PV_; // current water level milimeters (process variable)
        float radius_{100.0f}; // radius of the tank in millimeters
        float area_; // cross-sectional area of the tank in mm²
        float WaterLevel_; // current water level in the tank
        float height_{1000.0f}; // height of the tank in millimeters
        float volume_; // liters = area * height * 0.001 (to convert mm³ to liters)
        bool rampUp_; // flag to indicate if the outfor is ramping up or down
        float outflow; // current outflow rate in liters per dt
        float inflow; // current inflow rate in liters per dt
};