#include "GraphWidget.h"
#include <QPainter>
#include <QResizeEvent>
#include <cmath>
#include <algorithm>
#include <limits>

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 300);
    setStyleSheet("background-color: white;");
}

// update starting time so that the newest data point sits at the right edge of the view window
void GraphWidget::updateViewStartForAutoFollow() {
    if (data_.timeSteps.empty())
        return;
    float latest = data_.timeSteps.back();
    viewStartTime_ = latest - viewDuration_;
    if (viewStartTime_ < 0.0f) viewStartTime_ = 0.0f;
}

void GraphWidget::setTimeWindow(float startTime, float duration) {
    viewStartTime_ = startTime;
    viewDuration_ = duration;
    boundsNeedUpdate_ = true;  // Mark bounds as needing recalculation
    update();  // Trigger repaint
}

void GraphWidget::updateGraph(const SimulationData& data) {
    data_ = data;
    if (autoFollow_) {
        updateViewStartForAutoFollow();
    }
    boundsNeedUpdate_ = true;  // Mark bounds as needing recalculation
    update();  // Trigger repaint
}

void GraphWidget::calculateBounds() {
    if (data_.timeSteps.empty()) {
        minTime_ = 0.0f;
        maxTime_ = 1.0f;
        minValue_ = 0.0f;
        maxValue_ = 100.0f;
        return;
    }

    // auto-follow should update viewStartTime_ before we compute bounds
    if (autoFollow_ && useViewWindow_) {
        updateViewStartForAutoFollow();
    }

    // Use the view window for time bounds if enabled
    if (useViewWindow_) {
        minTime_ = viewStartTime_;
        maxTime_ = viewStartTime_ + viewDuration_;
    } else {
        // Auto-scale to all available data
        minTime_ = *std::min_element(data_.timeSteps.begin(), data_.timeSteps.end());
        maxTime_ = *std::max_element(data_.timeSteps.begin(), data_.timeSteps.end());
        
        if (minTime_ == maxTime_) {
            minTime_ -= 0.1f;
            maxTime_ += 0.1f;
        }
    }

    // Calculate value bounds for both PV and U within the view window
    float minPV = std::numeric_limits<float>::max();
    float maxPV = std::numeric_limits<float>::lowest();
    float minU = std::numeric_limits<float>::max();
    float maxU = std::numeric_limits<float>::lowest();
    
    // Only consider data points within the view window
    for (size_t i = 0; i < data_.timeSteps.size(); ++i) {
        if (data_.timeSteps[i] >= minTime_ && data_.timeSteps[i] <= maxTime_) {
            minPV = std::min(minPV, data_.pvValues[i]);
            maxPV = std::max(maxPV, data_.pvValues[i]);
            minU = std::min(minU, data_.uValues[i]);
            maxU = std::max(maxU, data_.uValues[i]);
        }
    }
    
    // Handle case where no data points are in the window
    if (minPV == std::numeric_limits<float>::max()) {
        minValue_ = 0.0f;
        maxValue_ = 100.0f;
        return;
    }

    minValue_ = std::min(minPV, minU);
    maxValue_ = std::max(maxPV, maxU);

    // Add padding
    float padding = (maxValue_ - minValue_) * 0.1f;
    if (padding == 0.0f) padding = 1.0f;
    
    minValue_ -= padding;
    maxValue_ += padding;
}

void GraphWidget::paintEvent(QPaintEvent *event) {
    // Recalculate bounds only if needed (data changed or time window changed)
    if (boundsNeedUpdate_) {
        calculateBounds();
        boundsNeedUpdate_ = false;
    }
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int width = this->width();
    int height = this->height();
    int margin = 50;
    int plotWidth = width - 2 * margin;
    int plotHeight = height - 2 * margin;

    // Draw background
    painter.fillRect(rect(), Qt::white);

    // Draw axes
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(margin, height - margin, width - margin, height - margin);  // X-axis
    painter.drawLine(margin, margin, margin, height - margin);  // Y-axis

    // Draw grid
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    
    // Vertical grid lines (time)
    int numTimeGrids = 5;
    for (int i = 0; i <= numTimeGrids; ++i) {
        int x = margin + (i * plotWidth) / numTimeGrids;
        painter.drawLine(x, margin, x, height - margin);
    }

    // Horizontal grid lines (values)
    int numValueGrids = 5;
    for (int i = 0; i <= numValueGrids; ++i) {
        int y = height - margin - (i * plotHeight) / numValueGrids;
        painter.drawLine(margin, y, width - margin, y);
    }

    // Draw axis labels
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8));
    
    // Time labels
    for (int i = 0; i <= numTimeGrids; ++i) {
        float time = minTime_ + (i / (float)numTimeGrids) * (maxTime_ - minTime_);
        int x = margin + (i * plotWidth) / numTimeGrids;
        painter.drawText(x - 20, height - margin + 20, 40, 20, Qt::AlignHCenter, 
                        QString::number(time, 'f', 2));
    }

    // Value labels (top=highest, bottom=lowest)
    for (int i = 0; i <= numValueGrids; ++i) {
        float value = minValue_ + ((numValueGrids - i) / (float)numValueGrids) * (maxValue_ - minValue_);
        // compute y from top down instead of bottom up
        int y = margin + (i * plotHeight) / numValueGrids;
        painter.drawText(5, y - 8, margin - 10, 16, Qt::AlignRight | Qt::AlignVCenter, 
                        QString::number(value, 'f', 1));
    }

    // Draw axis titles
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(width / 2 - 30, height - 25, 60, 20, Qt::AlignCenter, "Time (s)");
    painter.save();
    painter.translate(15, height / 2);
    painter.rotate(-90);
    painter.drawText(-30, 0, 60, 20, Qt::AlignCenter, "Value");
    painter.restore();

    // Draw data lines (only within view window)
    if (!data_.timeSteps.empty() && data_.timeSteps.size() > 1) {
        float timeRange = maxTime_ - minTime_;
        if (timeRange <= 0) timeRange = 1.0f;
        float valueRange = maxValue_ - minValue_;
        if (valueRange == 0.0f) valueRange = 1.0f;
        
        // Draw PV line (blue)
        painter.setPen(QPen(Qt::blue, 2));
        bool lastPointValid = false;
        for (size_t i = 0; i < data_.timeSteps.size(); ++i) {
            if (data_.timeSteps[i] >= minTime_ && data_.timeSteps[i] <= maxTime_) {
                float x = margin + ((data_.timeSteps[i] - minTime_) / timeRange) * plotWidth;
                float y = height - margin - ((data_.pvValues[i] - minValue_) / valueRange) * plotHeight;
                
                if (lastPointValid && i > 0) {
                    float x_prev = margin + ((data_.timeSteps[i-1] - minTime_) / timeRange) * plotWidth;
                    float y_prev = height - margin - ((data_.pvValues[i-1] - minValue_) / valueRange) * plotHeight;
                    painter.drawLine(QPointF(x_prev, y_prev), QPointF(x, y));
                }
                lastPointValid = true;
            } else {
                lastPointValid = false;
            }
        }

        // Draw U line (red)
        painter.setPen(QPen(Qt::red, 2));
        lastPointValid = false;
        for (size_t i = 0; i < data_.timeSteps.size(); ++i) {
            if (data_.timeSteps[i] >= minTime_ && data_.timeSteps[i] <= maxTime_) {
                float x = margin + ((data_.timeSteps[i] - minTime_) / timeRange) * plotWidth;
                float y = height - margin - ((data_.uValues[i] - minValue_) / valueRange) * plotHeight;
                
                if (lastPointValid && i > 0) {
                    float x_prev = margin + ((data_.timeSteps[i-1] - minTime_) / timeRange) * plotWidth;
                    float y_prev = height - margin - ((data_.uValues[i-1] - minValue_) / valueRange) * plotHeight;
                    painter.drawLine(QPointF(x_prev, y_prev), QPointF(x, y));
                }
                lastPointValid = true;
            } else {
                lastPointValid = false;
            }
        }
    }

    // Draw legend
    painter.setFont(QFont("Arial", 9));
    painter.setPen(Qt::blue);
    painter.drawLine(width - 160, 20, width - 120, 20);
    painter.drawText(width - 110, 15, 100, 20, Qt::AlignLeft, "PV (Process Variable)");

    painter.setPen(Qt::red);
    painter.drawLine(width - 160, 45, width - 120, 45);
    painter.drawText(width - 110, 40, 100, 20, Qt::AlignLeft, "U (Control Signal)");
}

void GraphWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // Don't recalculate bounds on resize, just repaint with cached bounds
    update();
}

