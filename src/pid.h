#pragma once

struct Params{
    double SP{0.0};
    double PV{0.0};
    double Kp{1.0};
    double Tn{0.0}; 
    double Td{0.0};
};

class PID{

    public:
        //construct
        PID() = default;
        explicit PID(const Params& p);


        void setParams(const Params& p);
        Params params() const;
        void reset();
        double update(double PV, double dt);

    private:
        Params params_{};
        double integral_{0.0};
        double lastOutput_{0.0};
        bool first_{true};

};