
#pragma once





class ProcessModel
{

    public:
        ProcessModel() = default;
        explicit ProcessModel();

        void setParams();
        void reset();
        float update(float sp, float dt);

    private:
        float PV_; // Process Variable returned by the model

};
