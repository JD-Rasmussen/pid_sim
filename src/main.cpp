#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QElapsedTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Main window
    QWidget window;
    window.setWindowTitle("PID Sim");
    window.resize(800, 600);

    // Layout + label to show a “simulation value”
    auto *layout = new QVBoxLayout(&window);
    auto *label  = new QLabel("Tick: 0");
    layout->addWidget(label);

    QElapsedTimer clock;
    clock.start();

    double accumulator = 0.0;
    const double fixed_dt = 0.010;   // 10 ms “physics” step

    QTimer timer;
    timer.setTimerType(Qt::PreciseTimer);
    timer.start(10);                 // UI heartbeat request (not a guarantee)
    int tickCount = 0;

    QObject::connect(&timer, &QTimer::timeout, [&] {
        const double dt_real = clock.restart() / 1000.0;  // seconds
        accumulator += dt_real;

        int substeps = 0;
        const int maxSubsteps = 5;   // safety guard

        while (accumulator >= fixed_dt && substeps < maxSubsteps) {
            // simulateOneStep(fixed_dt);   // PID.update(..., fixed_dt); plant.step(..., fixed_dt);
            ++tickCount;
            accumulator -= fixed_dt;
            ++substeps;
        }
        label->setText(QString("Tick: %1").arg(tickCount));
    });
    window.show();
    return app.exec();
}