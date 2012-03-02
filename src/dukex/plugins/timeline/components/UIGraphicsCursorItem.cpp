#include "UIGraphicsCursorItem.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

UIGraphicsCursorItem::UIGraphicsCursorItem(const QPen& pen) :
    m_pen(pen), m_mouseDown(false), m_duration(0) {
    setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setCursor(QCursor(Qt::SizeHorCursor));
    setZValue(100);
    m_boundingRect = QRectF(-2, 0, 3, 0);
}

int UIGraphicsCursorItem::type() const {
    return Type;
}

QRectF UIGraphicsCursorItem::boundingRect() const {
    return m_boundingRect;
}

int UIGraphicsCursorItem::cursorPos() const {
    return (int) pos().x();
}

void UIGraphicsCursorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setPen(m_pen);
    painter->drawLine(0, 0, 0, m_boundingRect.height());
}

QVariant UIGraphicsCursorItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    //Position is changing :
    if (change == QGraphicsItem::ItemPositionChange) {
        // When the cursor is moving fast, the viewport buffer
        // is not correctly updated, forcing it now.
        scene()->update(pos().x(), pos().y(), m_boundingRect.width(), m_boundingRect.height());
        // Keep the y axis in-place.
        qreal posX = value.toPointF().x();
        if (posX < 0)
            posX = 0;
        if (posX > m_duration)
            posX = m_duration;
        return QPoint((int) posX, (int) pos().y());
    }
    //The position HAS changed, ie we released the slider, or setPos has been called.
    else if (change == QGraphicsItem::ItemPositionHasChanged) {
        if (m_mouseDown)
            emit cursorPositionChanged((qint64) pos().x());
        emit cursorMoved((qint64) pos().x());
    }
    return QGraphicsItem::itemChange(change, value);
}

void UIGraphicsCursorItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    m_mouseDown = true;
    event->accept();
    QGraphicsItem::mousePressEvent(event);
}

void UIGraphicsCursorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    m_mouseDown = false;
    event->accept();
    QGraphicsItem::mouseReleaseEvent(event);
}

void UIGraphicsCursorItem::frameChanged(qint64 newFrame) {
    setPos(newFrame, pos().y());
}

void UIGraphicsCursorItem::setHeight(int height) {
    prepareGeometryChange();
    m_boundingRect.setHeight(height);
}

void UIGraphicsCursorItem::setDuration(int duration) {
    m_duration = duration;
}
