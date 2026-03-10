#include "GraphWidget.h"
#include <QPainter>
#include <QResizeEvent>
#include <cmath>
#include <algorithm>

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 300);
    setStyleSheet("background-color: white;");
}

void GraphWidget::updateGraph(const SimulationData& data) {
    data_ = data;
    calculateBounds();
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

    // Calculate time bounds
    minTime_ = *std::min_element(data_.timeSteps.begin(), data_.timeSteps.end());
    maxTime_ = *std::max_element(data_.timeSteps.begin(), data_.timeSteps.end());
    
    if (minTime_ == maxTime_) {
        minTime_ -= 0.1f;
        maxTime_ += 0.1f;
    }

    // Calculate value bounds for both PV and U
    float minPV = *std::min_element(data_.pvValues.begin(), data_.pvValues.end());
    float maxPV = *std::max_element(data_.pvValues.begin(), data_.pvValues.end());
    float minU = *std::min_element(data_.uValues.begin(), data_.uValues.end());
    float maxU = *std::max_element(data_.uValues.begin(), data_.uValues.end());

    minValue_ = std::min(minPV, minU);
    maxValue_ = std::max(maxPV, maxU);

    // Add padding
    float padding = (maxValue_ - minValue_) * 0.1f;
    if (padding == 0.0f) padding = 1.0f;
    
    minValue_ -= padding;
    maxValue_ += padding;
}

void GraphWidget::paintEvent(QPaintEvent *event) {
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

    // Value labels
    for (int i = 0; i <= numValueGrids; ++i) {
        float value = minValue_ + ((numValueGrids - i) / (float)numValueGrids) * (maxValue_ - minValue_);
        int y = height - margin - (i * plotHeight) / numValueGrids;
        painter.drawText(5, y - 8, margin - 10, 16, Qt::AlignRight | Qt::AlignVCenter, 
                        QString::number(value, 'f', 1));
    }

    // Draw axis titles
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(width / 2 - 30, height - 10, 60, 20, Qt::AlignCenter, "Time (s)");
    painter.save();
    painter.translate(15, height / 2);
    painter.rotate(-90);
    painter.drawText(-30, 0, 60, 20, Qt::AlignCenter, "Value");
    painter.restore();

    // Draw data lines
    if (!data_.timeSteps.empty() && data_.timeSteps.size() > 1) {
        // Draw PV line (blue)
        painter.setPen(QPen(Qt::blue, 2));
        for (size_t i = 1; i < data_.timeSteps.size(); ++i) {
            float x1 = margin + ((data_.timeSteps[i-1] - minTime_) / (maxTime_ - minTime_)) * plotWidth;
            float y1 = height - margin - ((data_.pvValues[i-1] - minValue_) / (maxValue_ - minValue_)) * plotHeight;
            float x2 = margin + ((data_.timeSteps[i] - minTime_) / (maxTime_ - minTime_)) * plotWidth;
            float y2 = height - margin - ((data_.pvValues[i] - minValue_) / (maxValue_ - minValue_)) * plotHeight;
            
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }

        // Draw U line (red)
        painter.setPen(QPen(Qt::red, 2));
        for (size_t i = 1; i < data_.timeSteps.size(); ++i) {
            float x1 = margin + ((data_.timeSteps[i-1] - minTime_) / (maxTime_ - minTime_)) * plotWidth;
            float y1 = height - margin - ((data_.uValues[i-1] - minValue_) / (maxValue_ - minValue_)) * plotHeight;
            float x2 = margin + ((data_.timeSteps[i] - minTime_) / (maxTime_ - minTime_)) * plotWidth;
            float y2 = height - margin - ((data_.uValues[i] - minValue_) / (maxValue_ - minValue_)) * plotHeight;
            
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
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
    update();
}

