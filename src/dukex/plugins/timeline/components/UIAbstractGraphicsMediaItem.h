#ifndef UIABSTRACTGRAPHICSMEDIAITEM_H
#define UIABSTRACTGRAPHICSMEDIAITEM_H

#include "UIAbstractGraphicsItem.h"

// forward declaration
class UITracksView;
class UITrackWorkflow;

class UIAbstractGraphicsMediaItem : public UIAbstractGraphicsItem {

    Q_OBJECT

public:
    UIAbstractGraphicsMediaItem();
    virtual ~UIAbstractGraphicsMediaItem();

public:
    virtual qint64 begin() const;
    virtual qint64 end() const;
    virtual qint64 itemHeight() const;
    virtual qint32 zSelected() const;
    virtual qint32 zNotSelected() const;
    virtual void setStartPos(qint64 position);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    virtual bool hasResizeBoundaries() const;
    virtual qint64 maxBegin() const;
    virtual qint64 maxEnd() const;

signals:
    void split(UIAbstractGraphicsMediaItem* self, qint64 frame);

private:
    bool m_muted;
};

#endif // UIABSTRACTGRAPHICSMEDIAITEM_H
