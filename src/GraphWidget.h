#pragma once

#include <QWidget>
#include "SimulationLogger.h"

class GraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget() = default;

    void updateGraph(const SimulationData& data);
    
    // Set the viewing window (static time period)
    void setTimeWindow(float startTime, float duration);
    float getViewStartTime() const { return viewStartTime_; }
    float getViewDuration() const { return viewDuration_; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    SimulationData data_;

    // auto-scroll helpers
    void updateViewStartForAutoFollow();
    float minTime_ = 0.0f;
    float maxTime_ = 1.0f;
    float minValue_ = 0.0f;
    float maxValue_ = 100.0f;
    
    // View window (static time period)
    float viewStartTime_ = 0.0f;
    float viewDuration_ = 10.0f;  // default 10 seconds
    bool useViewWindow_ = true;   // whether to use the view window or auto-scale
    bool autoFollow_ = false;     // if true, window tracks the newest data
    
    // Bounds cache
    bool boundsNeedUpdate_ = true;
    
    void calculateBounds();

public:
    void setAutoFollow(bool enable) { autoFollow_ = enable; boundsNeedUpdate_ = true; update(); }
    bool autoFollow() const { return autoFollow_; }
};
