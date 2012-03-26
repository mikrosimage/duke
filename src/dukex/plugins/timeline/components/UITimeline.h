#ifndef UITIMELINE_H
#define UITIMELINE_H

#include "TimelineTypes.h"
#include "ui_Timeline.h"
#include <dukexcore/dkxIObserver.h>
#include <set>

// forward declaration
class UITracksScene;
class UITracksView;
class UITracksControls;
class UITracksRuler;
class UITimelineControls;
class NodeManager;

class UITimeline : public QWidget, public IObserver {

    Q_OBJECT
    Q_DISABLE_COPY(UITimeline)

public:
    UITimeline(NodeManager*);
    virtual ~UITimeline(){}

public:
    UITracksView* tracksView();
    const UITracksView* tracksView() const;
    UITracksScene* tracksScene();
    UITracksRuler* tracksRuler();
    void update(::google::protobuf::serialize::SharedHolder sharedholder);

public slots:
    void fit();
    void changeZoom(int factor);
    void zoomIn();
    void zoomOut();
    void setDuration(int duration);
    void frameChanged(qint64 pos);
    void framerateChanged(double framerate);

private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    Ui::Timeline m_ui;
    UITracksView* m_tracksView;
    UITracksScene* m_tracksScene;
    UITracksRuler* m_tracksRuler;
    UITracksControls* m_tracksControls;
    UITimelineControls* m_timelineControls;
    NodeManager* m_manager;
    size_t m_zoom;
    std::set<size_t> mCachedFrames;
    int m_timerID;
};

#endif // UITIMELINE_H
