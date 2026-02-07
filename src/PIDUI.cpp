
#include "PIDUI.h"
#include <QFormLayout>
#include <QDoubleSpinBox>

PidParamsPanel::PidParamsPanel(QWidget* parent)
    : QWidget(parent)
{
    // Create spinboxes
    sp_ = new QDoubleSpinBox(this);
    kp_ = new QDoubleSpinBox(this);
    tn_ = new QDoubleSpinBox(this);
    td_ = new QDoubleSpinBox(this);

    // Reasonable ranges/steps (tune later)
    sp_->setRange(-1e6, 1e6); sp_->setDecimals(3); sp_->setSingleStep(1.0);
    kp_->setRange(0.0, 1e6);  kp_->setDecimals(3); kp_->setSingleStep(0.1);
    tn_->setRange(0.0, 1e6);  tn_->setDecimals(3); tn_->setSingleStep(0.1); // 0 => no I
    td_->setRange(0.0, 1e6);  td_->setDecimals(3); td_->setSingleStep(0.01); // 0 => no D

    // Layout
    auto *form = new QFormLayout();
    form->addRow("Index is above ↑", new QWidget(this)); // placeholder msg
    form->addRow("SP:", sp_);
    form->addRow("Kp:", kp_);
    form->addRow("Tn (s):", tn_);
    form->addRow("Td (s):", td_);
    setLayout(form);

    // When user edits any field, emit paramsEdited with the current values
    auto emitIfUserEdit = [this] {
        if (updatingFromModel_) return;
        Params p;
        p.SP = sp_->value();
        p.Kp = kp_->value();
        p.Tn = tn_->value();
        p.Td = td_->value();
        emit paramsEdited(p);
    };

    connect(sp_, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [emitIfUserEdit]{ emitIfUserEdit(); });
    connect(kp_, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [emitIfUserEdit]{ emitIfUserEdit(); });
    connect(tn_, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [emitIfUserEdit]{ emitIfUserEdit(); });
    connect(td_, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [emitIfUserEdit]{ emitIfUserEdit(); });
}

Params PidParamsPanel::getParams() const {
    Params p;
    p.SP = sp_->value();
    p.Kp = kp_->value();
    p.Tn = tn_->value();
    p.Td = td_->value();
    return p;
}

void PidParamsPanel::setParams(const Params& p) {
    updatingFromModel_ = true;
    sp_->setValue(p.SP);
    kp_->setValue(p.Kp);
    tn_->setValue(p.Tn);
    td_->setValue(p.Td);
    updatingFromModel_ = false;
}