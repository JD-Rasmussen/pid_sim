
#pragma once





class ProcessModel{

    public:

        virtual void setParams();
        virtual void reset();
        virtual float update(float *u, float *dt);

    private:
        float PV_; // Process Variable returned by the model

};



class WaterTank : public ProcessModel {

    public:

        void setParams(float area, float outflowCoeff)  ;
        void reset() override;
        float update(float *u, float *dt) override;

    private:
        float area_; // cross-sectional area of the tank
        float Inflow_; // cross-sectional area of the tank
        float outflowCoeff_; // coefficient for outflow (e.g., due to gravity)
        float WaterLevel_; // current water level in the tank
        float volume_; // current volume of water in the tank
};