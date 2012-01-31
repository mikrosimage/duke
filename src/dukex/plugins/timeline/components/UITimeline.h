#ifndef UITIMELINE_H
#define UITIMELINE_H

#include "TimelineTypes.h"
#include "player.pb.h"
#include "ui_Timeline.h"
#include <dukexgui/UIWidget.h>

// forward declaration
class UITracksScene;
class UITracksView;
class UITracksControls;
class UITracksRuler;
class UITimelineControls;
class NodeManager;

class UITimeline : public UIWidget {

    Q_OBJECT
    Q_DISABLE_COPY(UITimeline)

public:
    UITimeline(NodeManager*);
    virtual ~UITimeline() {
    }

public:
    UITracksView* tracksView();
    const UITracksView* tracksView() const;
    UITracksScene* tracksScene();
    UITracksRuler* tracksRuler();
    void update(::google::protobuf::serialize::SharedHolder sharedholder);

public slots:
    void changeZoom(int factor);
    void zoomIn();
    void zoomOut();
    void setDuration(int duration);
    void frameChanged(qint64 pos);
    void framerateChanged(double framerate);

private:
    Ui::Timeline m_ui;
    UITracksView* m_tracksView;
    UITracksScene* m_tracksScene;
    UITracksRuler* m_tracksRuler;
    UITracksControls* m_tracksControls;
    UITimelineControls* m_timelineControls;
    NodeManager* m_manager;
    size_t m_zoom;
};

#endif // UITIMELINE_H
