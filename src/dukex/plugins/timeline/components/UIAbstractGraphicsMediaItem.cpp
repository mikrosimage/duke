#include "UIAbstractGraphicsMediaItem.h"
#include "UITracksView.h"
#include "UITracksScene.h"
#include "UIGraphicsTrack.h"

UIAbstractGraphicsMediaItem::UIAbstractGraphicsMediaItem() :
    m_muted(false) {
    setWidth(20);
}

UIAbstractGraphicsMediaItem::~UIAbstractGraphicsMediaItem() {
    ungroup();
}

void UIAbstractGraphicsMediaItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
}

bool UIAbstractGraphicsMediaItem::hasResizeBoundaries() const {
    return true;
}

qint64 UIAbstractGraphicsMediaItem::maxBegin() const {
    return 0;
}

qint64 UIAbstractGraphicsMediaItem::maxEnd() const {
    return 200;
}

qint64 UIAbstractGraphicsMediaItem::begin() const {
    return 0;
}

qint64 UIAbstractGraphicsMediaItem::end() const {
    return 100;
}

qint64 UIAbstractGraphicsMediaItem::itemHeight() const {
    return 20;
}

qint32 UIAbstractGraphicsMediaItem::zSelected() const {
    return 100;
}

qint32 UIAbstractGraphicsMediaItem::zNotSelected() const {
    return 50;
}

void UIAbstractGraphicsMediaItem::setStartPos(qint64 position) {
    emit moved(position);
    UIAbstractGraphicsItem::setStartPos(position);
}
