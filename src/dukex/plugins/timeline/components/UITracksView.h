#ifndef UITRACKSVIEW_H
#define UITRACKSVIEW_H

#include "TimelineTypes.h"
#include "UIAbstractGraphicsMediaItem.h"
#include "UIGraphicsCursorItem.h"
#include <QGraphicsView>

// forward declaration
class QGraphicsLinearLayout;
class ItemPosition;

class UITracksView : public QGraphicsView {

    Q_OBJECT

public:
    enum Action {
        None, Move, Resize,
    };

public:
    UITracksView(QGraphicsScene *scene, QWidget *parent = 0);

public:
    void setDuration(int duration);
    int duration() const ;
    int tracksHeight() const;
    void setCursorPos(qint64 pos);
    qint64 cursorPos();
    UIGraphicsCursorItem *tracksCursor() const;
    void setScale(double scaleFactor);
    void createLayout();

public slots:
    void clear();
    void addItem(qint64 start, qint64 width);
    void removeItem(UIAbstractGraphicsItem *item);
    Action currentAction() const;

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);

private slots:
    void ensureCursorVisible();
    QRectF visibleRect();
    void updateDuration();
    void cleanUnusedTracks();

private:
    void addTrack(TrackType type);
    void removeVideoTrack();
    void removeAudioTrack();
    void cleanTracks(TrackType type);
    UIGraphicsTrack *getTrack(TrackType type, unsigned int number);
    void moveItem(UIAbstractGraphicsItem *item, qint32 track, qint64 time);
    ItemPosition findPosition(UIAbstractGraphicsItem *item, qint32 track, qint64 time);

signals:
    void zoomIn();
    void zoomOut();
    void durationChanged(int duration);
    void videoTrackAdded(UIGraphicsTrack *track);
    void audioTrackAdded(UIGraphicsTrack *track);
    void videoTrackRemoved();
    void audioTrackRemoved();

private:
    QGraphicsScene *m_scene;
    int m_tracksHeight;
    int m_projectDuration;
    UIGraphicsCursorItem *m_cursorLine;
    QGraphicsLinearLayout *m_layout;
    qint32 m_numVideoTrack;
    qint32 m_numAudioTrack;
    QGraphicsWidget *m_separator;
    Action m_action;
    int m_actionRelativeX;
    UIAbstractGraphicsItem::From m_actionResizeType;
    UIAbstractGraphicsItem *m_actionItem;
    UIGraphicsTrack *m_lastKnownTrack;
};

// ItemPosition definition
class ItemPosition {
public:
    ItemPosition() :
        track(-1), time(-1) {
    }
    ItemPosition(qint32 _track, qint64 _time) {
        track = _track;
        time = _time;
    }
public:
    bool isValid() const {
        if (track < 0 || time < 0)
            return false;
        return true;
    }
public:
    qint32 track;
    qint64 time;
};

#endif // UITRACKSVIEW_H
