#ifndef UIABSTRACTGRAPHICSITEM_H
#define UIABSTRACTGRAPHICSITEM_H

#include "TimelineTypes.h"
#include <QGraphicsItem>

// forward declaration
class UIGraphicsTrack;
class UITracksScene;

class UIAbstractGraphicsItem : public QObject, public QGraphicsItem {

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum From {
        BEGINNING, END
    };

public:
    UIAbstractGraphicsItem();
    virtual ~UIAbstractGraphicsItem();

public:
    virtual int type() const = 0;
    virtual TrackType trackType() const = 0;
    virtual QRectF boundingRect() const;
    virtual bool expandable() const = 0;
    virtual bool moveable() const = 0;
    virtual qint64 itemHeight() const = 0;
    virtual qint64 begin() const = 0;
    virtual qint64 end() const = 0;
    virtual qint32 zSelected() const = 0;
    virtual qint32 zNotSelected() const = 0;
    UITracksScene* scene();
    qint32 trackNumber();
    UIGraphicsTrack* track();
    virtual void setStartPos(qint64 position);
    qint64 startPos() const;
    void setColor(const QColor& color);
    QColor itemColor();
    void resize(qint64 newSize, From from);
    UIAbstractGraphicsItem* groupItem();
    void group(UIAbstractGraphicsItem* item);
    void ungroup();
    qint64 width() const;
    void setWidth(qint64 width);
    void setHeight(qint64 height);

public slots:
    void setTrack(UIGraphicsTrack* track);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual bool hasResizeBoundaries() const = 0;
    virtual qint64 maxBegin() const = 0;
    virtual qint64 maxEnd() const = 0;
    QColor m_itemColor;

protected slots:
    bool resizeZone(const QPointF& position);

signals:
    void moved(qint64 pos);
    void trackChanged(UIGraphicsTrack *tracks);

protected:
    static const quint32 RounderRectRadius = 3;

private:
    qint64 m_width;
    qint64 m_height;
    UIAbstractGraphicsItem* m_group;
};

#endif // UIABSTRACTGRAPHICSITEM_H
