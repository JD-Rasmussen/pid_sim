
#pragma once
#include <QWidget>
#include <QDoubleSpinBox>
#include "pid.h"



class PidParamsPanel : public QWidget {
    Q_OBJECT
public:
    explicit PidParamsPanel(QWidget* parent = nullptr);

    // read/write all at once
    Params getParams() const;
    void   setParams(const Params& p);

signals:
    void paramsEdited(const Params& p);  // emitted when user changes any field

private:
    QDoubleSpinBox* sp_;
    QDoubleSpinBox* kp_;
    QDoubleSpinBox* tn_;
    QDoubleSpinBox* td_;
    bool updatingFromModel_{false};      // guard to avoid feedback loops
};