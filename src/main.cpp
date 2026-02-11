#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QElapsedTimer>
#include <array>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "pid.h"
#include "PIDUI.h"



int main(int argc, char *argv[]) {

    const size_t NUM_PIDS = 4; // number of PID controllers
    std::array<PID, NUM_PIDS> pids;
    Params params[NUM_PIDS]; // array to hold parameters for each PID controller
    int activePIDIndex = 0; // index of the currently selected pid controller


    // init
    for (size_t i = 0; i < NUM_PIDS; ++i) {
        pids[i] = PID(); // Initialize each PID controller
        params[i].SP = 100.0 + i * 10.0; // Set different setpoints for each PID
        params[i].Kp = 1.0;     // Set different proportional gains
        params[i].Tn = 120.0;    // Set different integral times
        params[i].Td = 0.0;    // Set different derivative times
        pids[i].setParams(params[i]);
        pids[i].reset(); // Reset each PID controller to clear any previous state
    }

    QApplication app(argc, argv);

    // Main window
    QWidget window;
    window.setWindowTitle("PID Sim");
    window.resize(800, 600);

    //pid panel 
    auto *layout = new QVBoxLayout(&window);
    auto* paramsPanel = new PidParamsPanel(&window);
    layout->addWidget(paramsPanel);
    auto* indexSpin = new QSpinBox(&window);
    indexSpin->setRange(0, NUM_PIDS - 1);
    layout->addWidget(indexSpin);
    paramsPanel->setParams(pids[activePIDIndex].params());

    auto *label  = new QLabel("Tick: 0");
    layout->addWidget(label);

    QObject::connect(indexSpin, QOverload<int>::of(&QSpinBox::valueChanged), [&](int index) {
        activePIDIndex = index;
        paramsPanel->setParams(pids[activePIDIndex].params());
        window.setWindowTitle(QString("PID Sim — Editing #%1").arg(index));
    });

    QObject::connect(paramsPanel, &PidParamsPanel::paramsEdited, [&](const Params& p) {
        pids[activePIDIndex].setParams(p);
    });



    QElapsedTimer clock;
    clock.start();

    float accumulator = 0.0;
    const float fixed_dt = 0.010;   // 10 ms “physics” step

    QTimer timer;
    timer.setTimerType(Qt::PreciseTimer);
    timer.start(10);                 // UI heartbeat request (not a guarantee)
    int tickCount = 0;

    QObject::connect(&timer, &QTimer::timeout, [&] {
        const double dt_real = clock.restart() / 1000.0;  // seconds
        accumulator += dt_real;

        int substeps = 0;
        const int maxSubsteps = NUM_PIDS+1;   // safety guard
        while (accumulator >= fixed_dt && substeps < maxSubsteps) { 
            for (auto& pid : pids) {   
                float pv = 50.0; // placeholder for actual PV when enviroment is added
                float u  = pid.update(pv, fixed_dt);
            // add output later
            }

            ++tickCount;
            accumulator -= fixed_dt;
            ++substeps;
        }
        label->setText(QString("Tick: %1").arg(tickCount));
    });
    window.show();
    return app.exec();
}