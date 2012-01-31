#ifndef UITRACKSCONTROLS_H
#define UITRACKSCONTROLS_H

#include <QScrollArea>

// forward declaration
class QVBoxLayout;
class UIGraphicsTrack;

class UITracksControls : public QScrollArea {

    Q_OBJECT

public:
    UITracksControls(QWidget* parent = 0);

public slots:
    void addVideoTrack(UIGraphicsTrack* track);
    void addAudioTrack(UIGraphicsTrack* track);
    void removeVideoTrack();
    void removeAudioTrack();
    void clear();

private:
    QWidget* m_centralWidget;
    QWidget* m_separator;
    QVBoxLayout* m_layout;
};

#endif // UITRACKSCONTROLS_H
