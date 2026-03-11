#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QElapsedTimer>
#include <array>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTabWidget>
#include <QPushButton>
#include "ProcessModel.h"
#include "pid.h"
#include "PIDUI.h"
#include "SimulationLogger.h"
#include "GraphWidget.h"
//#include <chrono>

//consider changing to a more robust event loop with std::chrono and std::thread for the physics update.
//std::shared_ptrstd::unique_ptr
//std::chrono::duration
//std::chrono::steady_clockstd::chono::system_clockstd::chrono::high_precision

 //   using std::chrono::steady_clock;
 //   using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
 
    const size_t NUM_PIDS = 4; // number of PID controllers
    std::array<PID, NUM_PIDS> pids;
    Params params[NUM_PIDS]; // array to hold parameters for each PID controller
    int activePIDIndex = 0; // index of the currently selected pid controller
    std::array<WaterTank, NUM_PIDS> watertanks; // array of water tanks for each PID controller
    std::array<SimulationLogger, NUM_PIDS> loggers; // data loggers for each PID/tank pair
    std::array<float, NUM_PIDS> u_values{}; // control signals for each PID (initialized to 0)
    
    // init
    for (size_t i = 0; i < NUM_PIDS; ++i) {
        pids[i] = PID(); // Initialize each PID controller
        params[i].SP = 100.0 + i * 10.0; // Set different setpoints for each PID
        params[i].Kp = 1.0;     // Set different proportional gains
        params[i].Tn = 120.0;    // Set different integral times
        params[i].Td = 0.0;    // Set different derivative times
        pids[i].setParams(params[i]);
        pids[i].reset(); // Reset each PID controller to clear any previous state
        watertanks[i].setParams(100.0f, 1000.0f, 100.0f, 80.0f); // Set parameters for each water tank
        watertanks[i].reset(); // Reset each water tank to initial state
    }

    QApplication app(argc, argv);

    // Main window
    QWidget window;
    window.setWindowTitle("PID Sim");
    window.resize(1400, 800);

    // Main layout with control panel and tabs
    auto *mainLayout = new QHBoxLayout(&window);
    
    // Control panel (left side)
    auto *controlPanel = new QWidget(&window);
    auto *controlLayout = new QVBoxLayout(controlPanel);
    
    auto* paramsPanel = new PidParamsPanel(&window);
    controlLayout->addWidget(paramsPanel);
    
    auto* indexSpin = new QSpinBox(&window);
    indexSpin->setRange(0, NUM_PIDS - 1);
    controlLayout->addWidget(indexSpin);
    paramsPanel->setParams(pids[activePIDIndex].params());

    // Time window controls
    auto *timeWindowLabel = new QLabel("Time Window");
    timeWindowLabel->setStyleSheet("font-weight: bold;");
    controlLayout->addWidget(timeWindowLabel);
    
    auto *startTimeLabel = new QLabel("Start Time (s):");
    auto *startTimeSpin = new QDoubleSpinBox(&window);
    startTimeSpin->setRange(0.0, 1000.0);
    startTimeSpin->setSingleStep(1.0);
    startTimeSpin->setValue(0.0);
    controlLayout->addWidget(startTimeLabel);
    controlLayout->addWidget(startTimeSpin);
    
    auto *durationLabel = new QLabel("Duration (s):");
    auto *durationSpin = new QDoubleSpinBox(&window);
    durationSpin->setRange(0.1, 1000.0);
    durationSpin->setSingleStep(1.0);
    durationSpin->setValue(10.0);
    controlLayout->addWidget(durationLabel);
    controlLayout->addWidget(durationSpin);

    auto *label  = new QLabel("Tick: 0");
    controlLayout->addWidget(label);
    
    auto *resetButton = new QPushButton("Reset Data", &window);
    controlLayout->addWidget(resetButton);
    
    auto *currentULabel = new QLabel("Current U: 0.00");
    currentULabel->setStyleSheet("font-weight: bold; color: darkred;");
    controlLayout->addWidget(currentULabel);
    
    controlLayout->addStretch();
    mainLayout->addWidget(controlPanel, 0, Qt::AlignTop);
    
    // Tab widget for graphs (right side)
    auto *tabWidget = new QTabWidget(&window);
    std::array<GraphWidget*, NUM_PIDS> graphWidgets;
    for (size_t i = 0; i < NUM_PIDS; ++i) {
        graphWidgets[i] = new GraphWidget(&window);
        tabWidget->addTab(graphWidgets[i], QString("PID #%1").arg(i));
    }
    mainLayout->addWidget(tabWidget, 1);
    
    window.setLayout(mainLayout);

    QObject::connect(indexSpin, QOverload<int>::of(&QSpinBox::valueChanged), [&](int index) {
        activePIDIndex = index;
        paramsPanel->setParams(pids[activePIDIndex].params());
        window.setWindowTitle(QString("PID Sim — Editing #%1").arg(index));
    });

    QObject::connect(paramsPanel, &PidParamsPanel::paramsEdited, [&](const Params& p) {
        pids[activePIDIndex].setParams(p);
    });
    
    QObject::connect(resetButton, &QPushButton::clicked, [&]() {
        for (size_t i = 0; i < NUM_PIDS; ++i) {
            loggers[i].reset();
            graphWidgets[i]->updateGraph(loggers[i].data());
        }
    });

    // Connect time window controls to update all graphs
    auto updateTimeWindow = [&]() {
        for (size_t i = 0; i < NUM_PIDS; ++i) {
            graphWidgets[i]->setTimeWindow(startTimeSpin->value(), durationSpin->value());
        }
    };

    QObject::connect(startTimeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), updateTimeWindow);
    QObject::connect(durationSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), updateTimeWindow);



    QElapsedTimer clock;
    clock.start();

    float accumulator = 0.0;
    const float fixed_dt = 0.010;   // 10 ms “physics” step

    QTimer timer;
    timer.setTimerType(Qt::PreciseTimer);
    timer.start(10);                 // UI heartbeat request (not a guarantee)
    int tickCount = 0;
    float totalTime = 0.0f;

    QObject::connect(&timer, &QTimer::timeout, [&] {
        const double dt_real = clock.restart() / 1000.0;  // seconds
        accumulator += dt_real;

        int substeps = 0;
        const int maxSubsteps = NUM_PIDS+1;   // safety guard
        while (accumulator >= fixed_dt && substeps < maxSubsteps) { 
            // Update each PID and its corresponding water tank
            for (size_t i = 0; i < NUM_PIDS; ++i) {
                // Apply current control signal to water tank
                float pv = watertanks[i].update(u_values[i], fixed_dt);
                
                // Compute new control signal based on current process variable
                u_values[i] = pids[i].update(pv, fixed_dt);
                
                // Log data for this PID/tank pair
                loggers[i].addDataPoint(totalTime, pv, u_values[i]);
            }

            totalTime += fixed_dt;
            ++tickCount;
            accumulator -= fixed_dt;
            ++substeps;
        }
        
        // Update all graphs
        for (size_t i = 0; i < NUM_PIDS; ++i) {
            graphWidgets[i]->updateGraph(loggers[i].data());
        }
        
        label->setText(QString("Tick: %1 | Time: %2s").arg(tickCount).arg(totalTime, 0, 'f', 2));
        currentULabel->setText(QString("Current U: %1").arg(u_values[activePIDIndex], 0, 'f', 2));
    });
    window.show();
    return app.exec();
}