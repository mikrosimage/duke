#ifndef UIGRAPHICSTRACK_H
#define UIGRAPHICSTRACK_H

#include "TimelineTypes.h"
#include <QGraphicsWidget>
#include <QList>

// forward declaration
class UIAbstractGraphicsItem;
class UIAbstractGraphicsMediaItem;

class UIGraphicsTrack : public QGraphicsWidget {

    Q_OBJECT

public:
    enum {
        Type = UserType + 2
    };

public:
    UIGraphicsTrack(TrackType type, quint32 trackNumber, QGraphicsItem *parent = 0);

public:
    virtual int type() const;
    TrackType mediaType();
    void setHeight(int height);
    int height();
    void setTrackEnabled(bool enabled);
    bool isEnabled();
    quint32 trackNumber();
    QList<UIAbstractGraphicsItem*> childs();

private:
    TrackType m_type;
    quint32 m_trackNumber;
    bool m_enabled;
};

#endif // UIGRAPHICSTRACK_H
