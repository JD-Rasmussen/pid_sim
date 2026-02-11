#pragma once

struct Params{
    float SP{0.0};
 //   float PV{0.0};
    float Kp{1.0};
    float Tn{0.0}; 
    float Td{0.0};
    float outputMin{0.0}; 
    float outputMax{0.0};
};

class PID{

    public:
        //construct
        PID() = default;
        explicit PID(const Params& p);


        void setParams(const Params& p);
        Params params() const;
        
        void reset();
        float update(float PV, float dt);

    private:
        Params params_{};
        float error_{0.0};
        float proportional_{0.0};
        float integral_{0.0};
        float derivative_{0.0};
        float lastOutput_{0.0};
        bool first_{true};

};