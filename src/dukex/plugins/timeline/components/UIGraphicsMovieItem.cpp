#include "UIGraphicsMovieItem.h"
#include "UITracksView.h"
#include "UITracksScene.h"
#include "UITimeline.h"
#include <QTime>

UIGraphicsMovieItem::UIGraphicsMovieItem(const QString& path, qint64 recin, qint64 recout, qint64 srcin, qint64 srcout) {
    QString tooltip(tr(""
                    "<p style='white-space:pre'>"
                    "%1<br>"
                    "<b>rec</b>[%2-%3]  "
                    "<b>src</b>[%4-%5]"
                    "</p>").arg(path).arg(recin).arg(recout).arg(srcin).arg(srcout));
    setToolTip(tooltip);
}

int UIGraphicsMovieItem::type() const {
    return Type;
}

bool UIGraphicsMovieItem::expandable() const {
    return false;
}

bool UIGraphicsMovieItem::moveable() const {
    return true;
}

TrackType UIGraphicsMovieItem::trackType() const{
    return VideoTrack;
}

void UIGraphicsMovieItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->save();
    paintRect(painter, option);
    painter->restore();
    painter->save();
    paintTitle(painter, option);
    painter->restore();
}

void UIGraphicsMovieItem::paintRect(QPainter* painter, const QStyleOptionGraphicsItem* option) {
    QRectF drawRect;
    bool drawRound;

    // Disable the matrix transformations
    painter->setWorldMatrixEnabled(false);
    painter->setRenderHint(QPainter::Antialiasing);

    // Get the transformations required to map the text on the viewport
    QList<QGraphicsView *> views = scene()->views();
    QTransform viewPortTransform = views[0]->viewportTransform();
    //QTransform viewPortTransform = Timeline::getInstance()->tracksView()->viewportTransform();

    // Determine if a drawing optimization can be used
    if (option->exposedRect.left() > UIAbstractGraphicsItem::RounderRectRadius && option->exposedRect.right() < boundingRect().right() - UIAbstractGraphicsItem::RounderRectRadius) {
        // Optimized: paint only the exposed (horizontal) area
        drawRect = QRectF(option->exposedRect.left(), boundingRect().top(), option->exposedRect.right(), boundingRect().bottom());
        drawRound = false;
    } else {
        // Unoptimized: the item must be fully repaint
        drawRect = boundingRect();
        drawRound = true;
    }

    // Do the transformation
    QRectF mapped = deviceTransform(viewPortTransform).mapRect(drawRect);
    QLinearGradient gradient(mapped.topLeft(), mapped.bottomLeft());
    gradient.setColorAt(0, QColor::fromRgb(78, 78, 78));
    gradient.setColorAt(0.4, QColor::fromRgb(72, 72, 72));
    gradient.setColorAt(0.4, QColor::fromRgb(50, 50, 50));
    gradient.setColorAt(1, QColor::fromRgb(45, 45, 45));
    painter->setPen(Qt::black);
    painter->setBrush(QBrush(gradient));

    if (drawRound)
        painter->drawRoundedRect(mapped, UIAbstractGraphicsItem::RounderRectRadius, UIAbstractGraphicsItem::RounderRectRadius);
    else
        painter->drawRect(mapped);

    if (itemColor().isValid()) {
        QRectF mediaColorRect = mapped.adjusted(3, 2, -3, -2);
        painter->setPen(QPen(itemColor(), 2));
        painter->drawLine(mediaColorRect.topLeft(), mediaColorRect.topRight());
    }

    if (isSelected()) {
        setZValue(zSelected());
        painter->setPen(Qt::yellow);
        painter->setBrush(Qt::NoBrush);
        mapped.adjust(0, 0, 0, -1);
        if (drawRound)
            painter->drawRoundedRect(mapped, UIAbstractGraphicsItem::RounderRectRadius, UIAbstractGraphicsItem::RounderRectRadius);
        else
            painter->drawRect(mapped);
    } else
        setZValue(zNotSelected());
}

void UIGraphicsMovieItem::paintTitle(QPainter* painter, const QStyleOptionGraphicsItem* option) {
    Q_UNUSED(option);

    // Disable the matrix transformations
    painter->setWorldMatrixEnabled(false);

    // Setup the font
    QFont f = painter->font();
    f.setPointSize(8);
    painter->setFont(f);

    // Initiate the font metrics calculation
    QFontMetrics fm(painter->font());
    QString text = QString::number(pos().x()) + "-" + QString::number(pos().x() + width());

    // Get the transformations required to map the text on the viewport
    //QTransform viewPortTransform = Timeline::getInstance()->tracksView()->viewportTransform();
    QList<QGraphicsView *> views = scene()->views();
    QTransform viewPortTransform = views[0]->viewportTransform();

    // Do the transformation
    QRectF mapped = deviceTransform(viewPortTransform).mapRect(boundingRect());
    // Create an inner rect
    mapped.adjust(2, 2, -2, -2);

    painter->setPen(Qt::white);
    painter->drawText(mapped, Qt::AlignVCenter, fm.elidedText(text, Qt::ElideRight, mapped.width()));
}
