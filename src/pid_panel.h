#pragma once

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QDoubleSpinBox;

class PidPanel : public QWidget {

public:
    explicit PidPanel(QWidget *parent = nullptr);

private:
    QVBoxLayout    *m_mainLayout;
    QLabel         *m_tickLabel;
    QDoubleSpinBox *m_kpInput;
    QDoubleSpinBox *m_kiInput;
    QDoubleSpinBox *m_kdInput;

public:
    // simple getters so you can read values from outside later
    double kp() const;
    double ki() const;
    double kd() const;
    void   setTickText(const QString &text);
};