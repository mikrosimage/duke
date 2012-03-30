#ifndef UITIMELINECONTROLS_H
#define UITIMELINECONTROLS_H

#include <QWidget>
#include "ui_TimelineControls.h"

class UITimelineControls : public QWidget {

    Q_OBJECT

public:
    UITimelineControls(QWidget *parent = 0);

public slots:
    void frameChanged(qint64 pos);
    void framerateChanged(double f);
    void on_frameSpinBox_valueChanged(int);
    void on_framerateSpinBox_valueChanged(double);

signals:
    void frameControlChanged(qint64);
    void framerateControlChanged(double);

private:
    Ui::TimelineControls m_ui;
};

#endif // UITIMELINECONTROLS_H
