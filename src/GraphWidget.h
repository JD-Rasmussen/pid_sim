#pragma once

#include <QWidget>
#include "SimulationLogger.h"

class GraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget() = default;

    void updateGraph(const SimulationData& data);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    SimulationData data_;
    float minTime_ = 0.0f;
    float maxTime_ = 1.0f;
    float minValue_ = 0.0f;
    float maxValue_ = 100.0f;
    
    void calculateBounds();
};
