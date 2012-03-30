#include "UITimelineControls.h"

UITimelineControls::UITimelineControls(QWidget *parent) :
    QWidget(parent) {
    m_ui.setupUi(this);
}

void UITimelineControls::frameChanged(qint64 pos) {
    m_ui.frameSpinBox->blockSignals(true);
    m_ui.frameSpinBox->setValue(pos);
    m_ui.frameSpinBox->blockSignals(false);
}

void UITimelineControls::framerateChanged(double f) {
    m_ui.framerateSpinBox->blockSignals(true);
    m_ui.framerateSpinBox->setValue(f);
    m_ui.framerateSpinBox->blockSignals(false);
}

void UITimelineControls::on_frameSpinBox_valueChanged(int frame) {
    emit frameControlChanged(frame);
}

void UITimelineControls::on_framerateSpinBox_valueChanged(double framerate) {
    emit framerateControlChanged(framerate);
}
