#ifndef UITRACKCONTROLS_H
#define UITRACKCONTROLS_H

#include <QWidget>
#include "ui_TrackControls.h"

// forward declaration
class UIGraphicsTrack;

class UITrackControls : public QWidget {

    Q_OBJECT

public:
    UITrackControls(UIGraphicsTrack* track, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private slots:
    void setTrackDisabled(bool disable);
    void trackNameDoubleClicked();
    void fxButtonClicked();

private:
    void updateTextLabels();

private:
    Ui::TrackControls m_ui;
    UIGraphicsTrack *m_track;
    QString m_title;
};

#endif // UITRACKCONTROLS_H
