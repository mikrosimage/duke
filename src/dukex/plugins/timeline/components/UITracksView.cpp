#include "UITracksView.h"
#include "UIGraphicsMovieItem.h"
#include "UIGraphicsCursorItem.h"
#include "UIGraphicsTrack.h"
#include <QGraphicsLinearLayout>
#include <QScrollBar>

UITracksView::UITracksView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent), m_scene(scene) {
    m_tracksHeight = 20;
    m_numAudioTrack = 0;
    m_numVideoTrack = 0;
    m_lastKnownTrack = NULL;
    m_action = None;
    m_actionRelativeX = -1;
    m_actionItem = NULL;
    setMouseTracking(true);
    setAcceptDrops(true);
    setContentsMargins(0, 0, 0, 0);
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setCacheMode(QGraphicsView::CacheBackground);
    m_cursorLine = new UIGraphicsCursorItem(QPen(QColor(220, 30, 30)));
    m_scene->addItem(m_cursorLine);
    connect(m_cursorLine, SIGNAL(cursorMoved(qint64)), this, SLOT(ensureCursorVisible()));
}

int UITracksView::duration() const {
    return m_projectDuration;
}

int UITracksView::tracksHeight() const {
    return m_tracksHeight;
}

UIGraphicsCursorItem * UITracksView::tracksCursor() const {
    return m_cursorLine;
}

void UITracksView::createLayout() {
    m_layout = new QGraphicsLinearLayout(Qt::Vertical);
    m_layout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setPreferredWidth(0);
    QGraphicsWidget *container = new QGraphicsWidget();
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    container->setContentsMargins(0, 0, 0, 0);
    container->setLayout(m_layout);
    addTrack(VideoTrack);
    m_separator = new QGraphicsWidget();
    m_separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_separator->setPreferredHeight(5);
    m_layout->insertItem(1, m_separator);
    addTrack(AudioTrack);
    m_scene->addItem(container);
    setSceneRect(m_layout->contentsRect());
}

void UITracksView::addTrack(TrackType type) {
    UIGraphicsTrack *track = new UIGraphicsTrack(type, type == VideoTrack ? m_numVideoTrack : m_numAudioTrack);
    track->setHeight(tracksHeight());
    m_layout->insertItem(type == VideoTrack ? 0 : 1000, track);
    m_layout->activate();
    m_cursorLine->setHeight(m_layout->contentsRect().height());
    m_scene->invalidate(); // Redraw the background
    if (type == VideoTrack) {
        m_numVideoTrack++;
        emit videoTrackAdded(track);
    } else {
        m_numAudioTrack++;
        emit audioTrackAdded(track);
    }
}

void UITracksView::removeVideoTrack() {
    Q_ASSERT(m_numVideoTrack > 0);
    QGraphicsLayoutItem *item = m_layout->itemAt(0);
    m_layout->removeItem(item);
    m_layout->activate();
    m_scene->invalidate(); // Redraw the background
    m_cursorLine->setHeight(m_layout->contentsRect().height());
    m_numVideoTrack--;
    emit videoTrackRemoved();
    delete item;
}

void UITracksView::removeAudioTrack() {
    Q_ASSERT(m_numAudioTrack > 0);
    QGraphicsLayoutItem *item = m_layout->itemAt(m_layout->count() - 1);
    m_layout->removeItem(item);
    m_layout->activate();
    m_scene->invalidate(); // Redraw the background
    m_cursorLine->setHeight(m_layout->contentsRect().height());
    m_numAudioTrack--;
    emit audioTrackRemoved();
    delete item;
}

void UITracksView::clear() {
    m_layout->removeItem(m_separator);
    while (m_layout->count() > 0)
        delete m_layout->itemAt(0);
    m_layout->addItem(m_separator);
    m_numAudioTrack = 0;
    m_numVideoTrack = 0;
    addTrack(VideoTrack);
    addTrack(AudioTrack);
    updateDuration();
}

void UITracksView::addItem(qint64 start, qint64 width) {
    qint32 track = 0;
    //    if (track + 1 >= m_numVideoTrack) {
    //        int nbTrackToAdd = (track + 2) - m_numVideoTrack;
    //        for (int i = 0; i < nbTrackToAdd; ++i)
    //            addTrack(VideoTrack);
    //    }
    UIGraphicsMovieItem *mediaItem = NULL;
    mediaItem = new UIGraphicsMovieItem();
    mediaItem->setHeight(mediaItem->itemHeight());
    mediaItem->setTrack(getTrack(VideoTrack, track));
    mediaItem->setWidth(width);
    mediaItem->setStartPos(start);
    moveItem(mediaItem, track, start);
    updateDuration();
}

void UITracksView::moveItem(UIAbstractGraphicsItem *item, qint32 track, qint64 time) {
    // Add missing tracks
    if (item->trackType() == AudioTrack) {
        while (track >= m_numAudioTrack)
            addTrack(AudioTrack);
    } else if (item->trackType() == VideoTrack) {
        while (track >= m_numVideoTrack)
            addTrack(VideoTrack);
    }
    ItemPosition p = findPosition(item, track, time);
    if (p.isValid() && item->groupItem()) {
        bool validPosFound = false;
        // Add missing tracks for the target
        if (item->groupItem()->trackType() == AudioTrack) {
            while (p.track >= m_numAudioTrack)
                addTrack(AudioTrack);
        } else if (item->groupItem()->trackType() == VideoTrack) {
            while (p.track >= m_numVideoTrack)
                addTrack(VideoTrack);
        }
        // Search a position for the linked item
        ItemPosition p2 = findPosition(item->groupItem(), track, time);
        // Add missing tracks for the source
        if (item->trackType() == AudioTrack) {
            while (p2.track >= m_numAudioTrack)
                addTrack(AudioTrack);
        } else if (item->trackType() == VideoTrack) {
            while (p2.track >= m_numVideoTrack)
                addTrack(VideoTrack);
        }
        if (p.time == p2.time && p.track == p2.track)
            validPosFound = true;
        else {
            // We did not find a valid position for the two items.
            if (p.time == time && p.track == track) {
                // The primary item has found a position that match the request.
                // Ask it to try with the position of the linked item.
                p = findPosition(item, p2.track, p2.time);
                if (p.time == p2.time && p.track == p2.track)
                    validPosFound = true;
            } else if (p2.time == time && p2.track == track) {
                // The linked item has found a position that match the request.
                // Ask it to try with the position of the primary item.
                p2 = findPosition(item->groupItem(), p.track, p.time);
                if (p.time == p2.time && p.track == p2.track)
                    validPosFound = true;
            }
        }
        if (validPosFound) {
            // We've found a valid position that fit for the two items.
            // Move the primary item to the target destination.
            item->setStartPos(p.time);
            item->setTrack(getTrack(item->trackType(), p.track));
            // Move the linked item to the target destination.
            item->groupItem()->setStartPos(p2.time);
            item->groupItem()->setTrack(getTrack(item->groupItem()->trackType(), p2.track));
        }
    } else {
        if (p.isValid()) {
            item->setStartPos(p.time);
            item->setTrack(getTrack(item->trackType(), p.track));
        }
    }
}

ItemPosition UITracksView::findPosition(UIAbstractGraphicsItem *item, qint32 track, qint64 time) {
    // Create a fake item for computing collisions
    QGraphicsRectItem *chkItem = new QGraphicsRectItem(item->boundingRect());
    chkItem->setParentItem(getTrack(item->trackType(), track));
    chkItem->setPos(time, 0);
    QGraphicsItem *oldParent = item->parentItem();
    qreal oldPos = item->startPos();
    // Check for vertical collisions
    bool continueSearch = true;
    while (continueSearch) {
        QList<QGraphicsItem*> colliding = chkItem->collidingItems(Qt::IntersectsItemShape);
        bool itemCollision = false;
        for (int i = 0; i < colliding.size(); ++i) {
            UIAbstractGraphicsMediaItem *currentItem = dynamic_cast<UIAbstractGraphicsMediaItem*> (colliding.at(i));
            if (currentItem && currentItem != item) {
                qint32 trackId = currentItem->trackNumber();
                Q_ASSERT(trackId >= 0);
                // Collision with an item of the same type
                itemCollision = true;
                if (trackId > track) {
                    if (track < 1) {
                        chkItem->setParentItem(oldParent);
                        continueSearch = false;
                        break;
                    }
                    track -= 1;
                } else if (trackId <= track) {
                    int higherTrack = 0;
                    if (item->trackType() == VideoTrack)
                        higherTrack = m_numVideoTrack;
                    else if (item->trackType() == AudioTrack)
                        higherTrack = m_numAudioTrack;
                    if (track >= higherTrack - 1) {
                        chkItem->setParentItem(oldParent);
                        continueSearch = false;
                        break;
                    }
                    track += 1;
                }
                Q_ASSERT(getTrack(item->trackType(), track) != NULL);
                chkItem->setParentItem(getTrack(item->trackType(), track));
            }
        }
        if (!itemCollision)
            continueSearch = false;
    }
    // Check for horizontal collisions
    chkItem->setPos(qMax(time, (qint64) 0), 0);
    UIAbstractGraphicsMediaItem *hItem = NULL;
    QList<QGraphicsItem*> collide = chkItem->collidingItems(Qt::IntersectsItemShape);
    for (int i = 0; i < collide.count(); ++i) {
        hItem = dynamic_cast<UIAbstractGraphicsMediaItem*> (collide.at(i));
        if (hItem && hItem != item)
            break;
    }
    if (hItem && hItem != item) {
        qreal newpos;
        // Evaluate a possible solution
        if (chkItem->pos().x() > hItem->pos().x())
            newpos = hItem->pos().x() + hItem->boundingRect().width();
        else
            newpos = hItem->pos().x() - chkItem->boundingRect().width();

        if (newpos < 0 || newpos == hItem->pos().x())
            chkItem->setPos(oldPos, 0); // Fail
        else {
            // A solution may be found
            chkItem->setPos(qRound64(newpos), 0);
            QList<QGraphicsItem*> collideAgain = chkItem->collidingItems(Qt::IntersectsItemShape);
            for (int i = 0; i < collideAgain.count(); ++i) {
                UIAbstractGraphicsMediaItem *currentItem = dynamic_cast<UIAbstractGraphicsMediaItem*> (collideAgain.at(i));
                if (currentItem && currentItem != item) {
                    chkItem->setPos(oldPos, 0); // Fail
                    break;
                }
            }
        }
    }
    UIGraphicsTrack *t = static_cast<UIGraphicsTrack*> (chkItem->parentItem());
    ItemPosition p;
    p.time = chkItem->pos().x();
    if (t)
        p.track = t->trackNumber();
    else
        p.track = -1;
    delete chkItem;
    return p;
}

void UITracksView::removeItem(UIAbstractGraphicsItem *item) {
    // Is it the same item captured by mouse events
    if (item == m_actionItem)
        m_actionItem = NULL;
    delete item;
    updateDuration();
}

void UITracksView::setDuration(int duration) {
    int diff = (int) qAbs((qreal) duration - sceneRect().width());
    if (diff * matrix().m11() > -50) {
        if (matrix().m11() < 0.4)
            setSceneRect(0, 0, (duration + 100 / matrix().m11()), sceneRect().height());
        else
            setSceneRect(0, 0, (duration + 300), sceneRect().height());
    }
    m_projectDuration = duration;
}

void UITracksView::drawBackground(QPainter *painter, const QRectF &rect) {
    // Fill the background
    painter->fillRect(rect, QBrush(palette().base()));
    // Draw the tracks separators
    painter->setPen(QPen(QColor(50, 50, 50)));
    for (int i = 0; i < m_layout->count(); ++i) {
        QGraphicsItem* gi = m_layout->itemAt(i)->graphicsItem();
        if (!gi)
            continue;
        UIGraphicsTrack* track = qgraphicsitem_cast<UIGraphicsTrack*> (gi);
        if (!track)
            continue;
        QRectF trackRect = track->mapRectToScene(track->boundingRect());
        if (track->mediaType() == VideoTrack)
            painter->drawLine(rect.left(), trackRect.top(), rect.right() + 1, trackRect.top());
        else
            painter->drawLine(rect.left(), trackRect.bottom(), rect.right() + 1, trackRect.bottom());
    }
    // Audio/Video separator
    QLinearGradient g(0, m_separator->y(), 0, m_separator->y() + m_separator->boundingRect().height());
    QColor base = palette().window().color();
    QColor end = palette().dark().color();
    g.setColorAt(0, end);
    g.setColorAt(0.1, base);
    g.setColorAt(0.9, base);
    g.setColorAt(1.0, end);
    painter->setBrush(QBrush(g));
    painter->setPen(Qt::transparent);
    painter->drawRect(rect.left(), (int) m_separator->y(), (int) rect.right() + 1, (int) m_separator->boundingRect().height());
}

void UITracksView::resizeEvent(QResizeEvent *event) {
Q_UNUSED(event);
}

void UITracksView::mousePressEvent(QMouseEvent *event) {
Q_UNUSED(event);
}

void UITracksView::mouseMoveEvent(QMouseEvent *event) {
Q_UNUSED(event);
}

void UITracksView::mouseReleaseEvent(QMouseEvent *event) {
Q_UNUSED(event);
}

void UITracksView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() == Qt::ControlModifier) {
        if (event->delta() > 0)
            emit zoomOut();
        else
            emit zoomIn();
        event->accept();
    }
    QGraphicsView::wheelEvent(event);
}

void UITracksView::dragEnterEvent(QDragEnterEvent *event) {
    Q_UNUSED(event);
}

void UITracksView::dragMoveEvent(QDragMoveEvent *event) {
    Q_UNUSED(event)
}

void UITracksView::dragLeaveEvent(QDragLeaveEvent *event) {
    Q_UNUSED(event)
}

void UITracksView::dropEvent(QDropEvent *event) {
    Q_UNUSED(event)
}

void UITracksView::setCursorPos(qint64 pos) {
    if (pos < 0)
        pos = 0;
    m_cursorLine->frameChanged(pos);
}

qint64 UITracksView::cursorPos() {
    return m_cursorLine->cursorPos();
}

void UITracksView::setScale(double scaleFactor) {
    QMatrix matrix;
    matrix.scale(scaleFactor, 1);
    //TODO update the scene scale?
    setMatrix(matrix);
    int diff = (int) (sceneRect().width() - (qreal) m_projectDuration);
    if (diff * matrix.m11() < 50) {
        if (matrix.m11() < 0.4)
            setSceneRect(0, 0, (m_projectDuration + 100 / matrix.m11()), sceneRect().height());
        else
            setSceneRect(0, 0, (m_projectDuration + 300), sceneRect().height());
    }
    centerOn(m_cursorLine);
}

void UITracksView::ensureCursorVisible() {
    if (horizontalScrollBar()->isVisible()) {
        QRectF visibleArea = visibleRect();
        ensureVisible(cursorPos(), visibleArea.y() + (visibleArea.height() / 2), 1, 1, 150, 0);
    }
}

QRectF UITracksView::visibleRect() {
    QPointF topLeft(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF bottomRight(topLeft + viewport()->rect().bottomRight());
    QMatrix reverted = matrix().inverted();
    return reverted.mapRect(QRectF(topLeft, bottomRight));
}

void UITracksView::updateDuration() {
    //TODO this should use a variant of mediaItems( const QPoint& )
    QList<QGraphicsItem*> sceneItems = m_scene->items();
    int projectDuration = 0;
    for (int i = 0; i < sceneItems.size(); ++i) {
        UIAbstractGraphicsMediaItem *item = dynamic_cast<UIAbstractGraphicsMediaItem*> (sceneItems.at(i));
        if (!item)
            continue;
        if ((item->startPos() + item->boundingRect().width()) > projectDuration)
            projectDuration = (int) (item->startPos() + item->boundingRect().width());
    }
    m_projectDuration = projectDuration;
    // Make sure that the width is not below zero
    int minimumWidth = qMax(m_projectDuration, 0);
    // PreferredWidth not working?
    m_layout->setMinimumWidth(minimumWidth);
    m_layout->setMaximumWidth(minimumWidth);
    setSceneRect(m_layout->contentsRect());
    emit durationChanged(m_projectDuration);
    // Also check for unused tracks
    cleanUnusedTracks();
}

void UITracksView::cleanTracks(TrackType type) {
    int tracksToCheck;
    int tracksToRemove = 0;
    if (type == VideoTrack)
        tracksToCheck = m_numVideoTrack;
    else
        tracksToCheck = m_numAudioTrack;
    for (int i = tracksToCheck; i > 0; --i) {
        UIGraphicsTrack *track = getTrack(type, i);
        if (!track)
            continue;
        QList<UIAbstractGraphicsItem*> items = track->childs();
        if (items.count() == 0)
            tracksToRemove++;
        else
            break;
    }
    while (tracksToRemove > 1) {
        if (type == VideoTrack)
            removeVideoTrack();
        else
            removeAudioTrack();
        tracksToRemove--;
    }
}

void UITracksView::cleanUnusedTracks() {
    cleanTracks(VideoTrack);
    cleanTracks(AudioTrack);
}

UIGraphicsTrack* UITracksView::getTrack(TrackType type, unsigned int number) {
    for (int i = 0; i < m_layout->count(); ++i) {
        QGraphicsItem *gi = m_layout->itemAt(i)->graphicsItem();
        UIGraphicsTrack *track = qgraphicsitem_cast<UIGraphicsTrack*> (gi);
        if (!track)
            continue;
        if (track->mediaType() != type)
            continue;
        if (track->trackNumber() == number)
            return track;
    }
    return NULL;
}

UITracksView::Action UITracksView::currentAction() const {
    return m_action;
}
