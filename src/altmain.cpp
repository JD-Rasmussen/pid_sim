#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include "pid.h"
#include "PIDUI.h"
#include "ProcessModel.h"
#include "simulation.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Build PID with initial params
    Params p; p.SP = 100.0f; p.Kp = 1.0f; p.Tn = 120.0f; p.Td = 0.0f;
    PID pid(p); pid.reset();  // your API                                        

    // Build plant
    WaterTank tank;
    tank.setParams(/*radius=*/100.0f, /*height=*/1000.0f,
                   /*maxInflow=*/50.0f, /*maxOutflow=*/10.0f);
    tank.reset();                                                                  

    // Orchestrator
    Simulation sim(pid, tank, /*fixed_dt=*/0.010f);

    // UI
    QWidget window;
    window.setWindowTitle("PID + Plant");
    auto* layout = new QVBoxLayout(&window);
    auto* panel  = new PidParamsPanel(&window);
    layout->addWidget(panel);
    auto* label  = new QLabel("t=0, SP=0, PV=0, CV=0, e=0", &window);
    layout->addWidget(label);

    // push PID params into panel and let user edit
    panel->setParams(pid.params());                                              
    QObject::connect(panel, &PidParamsPanel::paramsEdited, &window, [&](const Params& np) {
        sim.pid().setParams(np); // live-edit controller                              
    });

    // Show readings each step (later you can feed a chart)
    QObject::connect(&sim, &Simulation::stepped, &window,
        [&](float t, float sp, float pv, float cv, float e) {
            label->setText(QString("t=%1  SP=%2  PV=%3  CV=%4  e=%5")
                           .arg(t, 0, 'f', 3).arg(sp).arg(pv).arg(cv).arg(e));
        });

    // Optional: time-based SP
    sim.setSetpointFunc([](float t) -> float {
        if (t < 2.0f) return 0.0f;
        if (t < 12.0f) return 10.0f;
        return 10.0f + 0.5f * (t - 12.0f);
    });

    sim.start(10);
    window.show();
    return app.exec();
}