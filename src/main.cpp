#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("PID Simulator");
    window.resize(800, 600);

    auto *layout = new QVBoxLayout(&window);
    auto *label  = new QLabel("Tick: 0");
    layout->addWidget(label);

    int counter = 0;

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        counter++;
        label->setText(QString("Tick: %1").arg(counter));
    });
    timer.start(100);

    window.show();
    return app.exec();
}