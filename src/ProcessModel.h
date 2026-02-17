
#pragma once





class ProcessModel{

    public:
        ProcessModel() = default;
        explicit ProcessModel();

        void setParams();
        void reset();
        float update(float *u, float *dt);

    private:
        float PV_; // Process Variable returned by the model

};



class WaterTank : public ProcessModel {

    public:
        WaterTank() = default;
        explicit WaterTank();

        void setParams(float area, float outflowCoeff);
        void reset();
        float update(float *u, float *dt);

    private:
        float area_; // cross-sectional area of the tank
        float Inflow_; // cross-sectional area of the tank
        float outflowCoeff_; // coefficient for outflow (e.g., due to gravity)
        float WaterLevel_; // current water level in the tank
};