#include <QApplication>
//#include <QWidget>
//#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
//#include <QDoubleSpinBox>

#include "pid_panel.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("PID Simulator");
    window.resize(800, 600);

    auto *layout = new QVBoxLayout(&window);
    auto *panel  = new PidPanel(&window);
    layout->addWidget(panel);

    int counter = 0;

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        counter++;
        double kp = panel->kp();
        double ki = panel->ki();
        double kd = panel->kd();
        panel->setTickText(
            QString("Tick: %1 | Kp=%2 Ki=%3 Kd=%4")
                .arg(counter)
                .arg(kp)
                .arg(ki)
                .arg(kd)
        );
    });
    timer.start(100);

    window.show();
    return app.exec();
}