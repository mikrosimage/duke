#include "UIAbstractGraphicsItem.h"
#include "UIGraphicsTrack.h"
#include "UITracksScene.h"
#include <QGraphicsView>

UIAbstractGraphicsItem::UIAbstractGraphicsItem() :
    m_width(0), m_height(0), m_group(NULL) {
    setFlags(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
}

UIAbstractGraphicsItem::~UIAbstractGraphicsItem() {
}

UITracksScene* UIAbstractGraphicsItem::scene() {
    return qobject_cast<UITracksScene*> (QGraphicsItem::scene());
}

QRectF UIAbstractGraphicsItem::boundingRect() const {
    return QRectF(0, 0, (qreal) m_width, (qreal) m_height);
}

void UIAbstractGraphicsItem::setWidth(qint64 width) {
    prepareGeometryChange();
    m_width = width;
}

void UIAbstractGraphicsItem::setHeight(qint64 height) {
    prepareGeometryChange();
    m_height = height;
}

qint32 UIAbstractGraphicsItem::trackNumber() {
    if (parentItem()) {
        UIGraphicsTrack* graphicsTrack = qgraphicsitem_cast<UIGraphicsTrack*> (parentItem());
        if (graphicsTrack)
            return graphicsTrack->trackNumber();
    }
    return -1;
}

void UIAbstractGraphicsItem::setTrack(UIGraphicsTrack* track) {
    setParentItem(track);
    emit trackChanged(track);
}

UIGraphicsTrack* UIAbstractGraphicsItem::track() {
    return qgraphicsitem_cast<UIGraphicsTrack*> (parentItem());
}

void UIAbstractGraphicsItem::setStartPos(qint64 position) {
    QGraphicsItem::setPos((qreal) position, 0);
}

qint64 UIAbstractGraphicsItem::startPos() const {
    return qRound64(QGraphicsItem::pos().x());
}

void UIAbstractGraphicsItem::resize(qint64 newSize, From from) {
    if (newSize < 1) {
        newSize = 1;
    }
    if (from == END) {
        setWidth(newSize);
    } else {
        qint64 diff = m_width - newSize;
        qint64 newPos = startPos() + diff;
        if (newPos >= 0) {
            setStartPos(newPos);
            setWidth(newSize);
        }
    }
}

QColor UIAbstractGraphicsItem::itemColor() {
    return m_itemColor;
}

void UIAbstractGraphicsItem::setColor(const QColor &color) {
    m_itemColor = color;
}

void UIAbstractGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    Q_UNUSED(event);
}

void UIAbstractGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    //    if (resizeZone(event->pos()))
    //        setCursor(Qt::SizeHorCursor);
    //    else
    //        setCursor(Qt::OpenHandCursor);
}

void UIAbstractGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    //    if (resizeZone(event->pos()))
    //        setCursor(Qt::SizeHorCursor);
    //    else
    //        setCursor(Qt::ClosedHandCursor);
}

void UIAbstractGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent*) {
    //    setCursor(Qt::OpenHandCursor);
}

bool UIAbstractGraphicsItem::resizeZone(const QPointF& position) {
    // FIXME: UGLY
    QTransform transform = scene()->views()[0]->transform().inverted();
    // Map the resize zone distance from the view to the item coordinates.
    QLineF line = transform.map(QLineF(0, 0, 7, 0));
    if (position.x() < line.x2() || position.x() > (boundingRect().width() - line.x2())) {
        return true;
    }
    return false;
}

UIAbstractGraphicsItem* UIAbstractGraphicsItem::groupItem() {
    return m_group;
}

void UIAbstractGraphicsItem::group(UIAbstractGraphicsItem* item) {
    Q_ASSERT(item);
    if (m_group)
        ungroup();
    item->m_group = this;
    m_group = item;
}

void UIAbstractGraphicsItem::ungroup() {
    if (m_group == NULL)
        return;
    m_group->m_group = NULL;
    m_group = NULL;
}

qint64 UIAbstractGraphicsItem::width() const {
    return m_width;
}
