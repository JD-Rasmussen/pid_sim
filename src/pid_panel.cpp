#include "pid_panel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>

PidPanel::PidPanel(QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QVBoxLayout(this)),
      m_tickLabel(new QLabel("Tick: 0", this)),
      m_kpInput(new QDoubleSpinBox(this)),
      m_kiInput(new QDoubleSpinBox(this)),
      m_kdInput(new QDoubleSpinBox(this))
{
    // Kp row
    auto *kpRow   = new QHBoxLayout();
    auto *kpLabel = new QLabel("Kp:", this);
    m_kpInput->setRange(0.0, 1000.0);
    m_kpInput->setDecimals(3);
    m_kpInput->setValue(1.0);
    kpRow->addWidget(kpLabel);
    kpRow->addWidget(m_kpInput);
    m_mainLayout->addLayout(kpRow);

    // Ki row
    auto *kiRow   = new QHBoxLayout();
    auto *kiLabel = new QLabel("Ki:", this);
    m_kiInput->setRange(0.0, 1000.0);
    m_kiInput->setDecimals(3);
    m_kiInput->setValue(0.0);
    kiRow->addWidget(kiLabel);
    kiRow->addWidget(m_kiInput);
    m_mainLayout->addLayout(kiRow);

    // Kd row
    auto *kdRow   = new QHBoxLayout();
    auto *kdLabel = new QLabel("Kd:", this);
    m_kdInput->setRange(0.0, 1000.0);
    m_kdInput->setDecimals(3);
    m_kdInput->setValue(0.0);
    kdRow->addWidget(kdLabel);
    kdRow->addWidget(m_kdInput);
    m_mainLayout->addLayout(kdRow);

    // Tick label at bottom
    m_mainLayout->addWidget(m_tickLabel);
}

double PidPanel::kp() const { return m_kpInput->value(); }
double PidPanel::ki() const { return m_kiInput->value(); }
double PidPanel::kd() const { return m_kdInput->value(); }

void PidPanel::setTickText(const QString &text) {
    m_tickLabel->setText(text);
}