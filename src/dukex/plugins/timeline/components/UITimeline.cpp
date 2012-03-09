#include "UITimeline.h"
#include "components/UITracksView.h"
#include "components/UITracksScene.h"
#include "components/UITracksControls.h"
#include "components/UITracksRuler.h"
#include "components/UITimelineControls.h"
#include <dukexcore/dkxNodeManager.h>
#include <dukexcore/nodes/TransportNode.h>
#include <dukexcore/nodes/PlaylistNode.h>
#include <dukexcore/nodes/InfoNode.h>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QTimer>

#define MINZOOMRATIO 0
#define MAXZOOMRATIO 13

UITimeline::UITimeline(NodeManager* _manager) :
    m_manager(_manager), m_zoom(4) {
    m_ui.setupUi(this);

    m_tracksScene = new UITracksScene(this);
    m_tracksView = new UITracksView(m_tracksScene, m_ui.tracksFrame);
    m_tracksView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tracksView->scale(1, 1);

    QHBoxLayout* tracksViewLayout = new QHBoxLayout();
    tracksViewLayout->setContentsMargins(0, 0, 0, 0);
    m_ui.tracksFrame->setLayout(tracksViewLayout);
    tracksViewLayout->addWidget(m_tracksView);

    m_tracksRuler = new UITracksRuler(tracksView(), this);
    QHBoxLayout* tracksRulerLayout = new QHBoxLayout();
    tracksRulerLayout->setContentsMargins(0, 0, 0, 0);
    m_ui.rulerFrame->setLayout(tracksRulerLayout);
    tracksRulerLayout->addWidget(m_tracksRuler);

    m_tracksControls = new UITracksControls(this);
    QHBoxLayout* tracksControlsLayout = new QHBoxLayout();
    tracksControlsLayout->setContentsMargins(0, 0, 0, 0);
    m_ui.controlsFrame->setLayout(tracksControlsLayout);
    tracksControlsLayout->addWidget(m_tracksControls);

    m_timelineControls = new UITimelineControls(this);
    QHBoxLayout* timelineControlsLayout = new QHBoxLayout();
    timelineControlsLayout->setContentsMargins(0, 0, 0, 0);
    m_ui.infoWidget->setLayout(timelineControlsLayout);
    timelineControlsLayout->addWidget(m_timelineControls);

    changeZoom(5);
    setDuration(0);

    // Scroll
    connect(m_tracksView->horizontalScrollBar(), SIGNAL( valueChanged(int) ), m_tracksRuler, SLOT( moveRuler(int) ));
    connect(m_tracksView->verticalScrollBar(), SIGNAL( valueChanged(int) ), m_tracksControls->verticalScrollBar(), SLOT( setValue(int) ));
    connect(m_tracksControls->verticalScrollBar(), SIGNAL( valueChanged(int) ), m_tracksView->verticalScrollBar(), SLOT( setValue(int) ));
    // Zoom
    connect(m_tracksView, SIGNAL(zoomIn()), this, SLOT(zoomIn()));
    connect(m_tracksView, SIGNAL(zoomOut()), this, SLOT(zoomOut()));
    // Project duration change
    connect(m_tracksView, SIGNAL( durationChanged(int) ), this, SLOT( setDuration(int) ));
    // Tracks controls: Add/Remove track
    connect(m_tracksView, SIGNAL( videoTrackAdded(UIGraphicsTrack*) ), m_tracksControls, SLOT( addVideoTrack(UIGraphicsTrack*) ));
    connect(m_tracksView, SIGNAL( audioTrackAdded(UIGraphicsTrack*) ), m_tracksControls, SLOT( addAudioTrack(UIGraphicsTrack*) ));
    connect(m_tracksView, SIGNAL(videoTrackRemoved()), m_tracksControls, SLOT(removeVideoTrack()));
    connect(m_tracksView, SIGNAL(audioTrackRemoved()), m_tracksControls, SLOT(removeAudioTrack()));
    // Cursor position
    connect(m_tracksView->tracksCursor(), SIGNAL(cursorPositionChanged(qint64)), this, SLOT(frameChanged(qint64)));
    connect(m_tracksView->tracksCursor(), SIGNAL(cursorPositionChanged(qint64)), m_timelineControls, SLOT(frameChanged(qint64)));
    connect(m_tracksRuler, SIGNAL(frameChanged(qint64)), m_tracksView->tracksCursor(), SLOT(frameChanged(qint64)));
    connect(m_tracksRuler, SIGNAL(frameChanged(qint64)), m_timelineControls, SLOT(frameChanged(qint64)));
    connect(m_tracksRuler, SIGNAL(frameChanged(qint64)), this, SLOT(frameChanged(qint64)));
    connect(m_timelineControls, SIGNAL(frameControlChanged(qint64)), this, SLOT(frameChanged(qint64)));
    connect(m_timelineControls, SIGNAL(frameControlChanged(qint64)), this, SLOT(setFocus()));
    // Framerate
    connect(m_timelineControls, SIGNAL( framerateControlChanged(double) ), m_tracksRuler, SLOT( framerateChanged(double) ));
    connect(m_timelineControls, SIGNAL( framerateControlChanged(double) ), this, SLOT( framerateChanged(double) ));
    connect(m_timelineControls, SIGNAL( framerateControlChanged(double) ), this, SLOT( setFocus() ));

    m_tracksView->createLayout();

    // Starting Timer : to update cache state every N ms
//    m_timerID = QObject::startTimer(40);
//    QTimer::singleShot(1000, this, SLOT(launchUpdateLoop()));
}

UITimeline::~UITimeline() {
    QObject::killTimer(m_timerID);
}

UITracksView* UITimeline::tracksView() {
    return m_tracksView;
}

const UITracksView* UITimeline::tracksView() const {
    return m_tracksView;
}

UITracksScene* UITimeline::tracksScene() {
    return m_tracksScene;
}

UITracksRuler* UITimeline::tracksRuler() {
    return m_tracksRuler;
}

void UITimeline::update(::google::protobuf::serialize::SharedHolder sharedholder) {
    using namespace ::duke::protocol;
    // do not update position when mouse button is down (==scrub)
    if (::google::protobuf::serialize::isType<Transport>(*sharedholder) && QApplication::mouseButtons() == Qt::NoButton) {
        const Transport t = ::google::protobuf::serialize::unpackTo<Transport>(*sharedholder);
        switch (t.type()) {
            case Transport_TransportType_PLAY:
            case Transport_TransportType_STOP:
            case Transport_TransportType_STORE:
            case Transport_TransportType_CUE_FIRST:
            case Transport_TransportType_CUE_LAST:
            case Transport_TransportType_CUE_STORED:
                break;
            case Transport_TransportType_CUE:
                if (t.cue().cueclip())
                    return;
                if (t.cue().cuerelative())
                    return;
                m_tracksView->setCursorPos(t.cue().value());
                m_timelineControls->frameChanged(t.cue().value());
                break;
        }
    } else if (::google::protobuf::serialize::isType<Playlist>(*sharedholder)) {
        m_tracksControls->clear();
        m_tracksView->clear();
        const Playlist & p = ::google::protobuf::serialize::unpackTo<Playlist>(*sharedholder);
        qint64 lastFrame = 0;
        if (p.track_size() > 0) {
            for (int i = 0; i < p.track_size(); ++i) {
                const Track & t = p.track(i);
                if (t.clip_size() > 0) {
                    for (int j = 0; j < t.clip_size(); ++j) {
                        const Clip & c = t.clip(j);
                        QString path = "N/A";
                        qint64 srcin = -1;
                        qint64 srcout = -1;
                        if(c.has_media()){
                            const ::duke::protocol::Media& m = c.media();
                            path = QString::fromStdString(m.filename());
                            if(m.has_source()){
                                srcin = m.source().first();
                                srcout = m.source().last();
                            }
                        }
                        m_tracksView->addItem(path, c.record().first(), c.record().last(), srcin, srcout); //TODO < handle track number here
                    }
                    qint64 clipLastFrame = t.clip(t.clip_size() - 1).record().last();
                    if (clipLastFrame > lastFrame)
                        lastFrame = clipLastFrame;
                }
            }
        }
        setDuration(lastFrame);
        fit();
    }
    else if (::google::protobuf::serialize::isType<Info>(*sharedholder)) {
        const Info & info = ::google::protobuf::serialize::unpackTo<Info>(*sharedholder);
        if (info.IsInitialized()){
//            info.PrintDebugString();
            if(info.has_cachestate()){
                mCachedFrames.clear();
                for(int i = 0; i < info.cachestate().track_size(); ++i){
                    const ::duke::protocol::Info_CacheState_TrackCache& t = info.cachestate().track(i);
                    for(int j = 0; j < t.range_size(); ++j){
                        const ::duke::protocol::FrameRange& r = t.range(j);
                        for(size_t k = r.first(); k < r.last(); ++k){
                            mCachedFrames.insert(k);
                        }
                    }
                }
                m_tracksRuler->setCacheState(mCachedFrames);
            }
        }
    }
}

void UITimeline::frameChanged(qint64 pos) {
    INode::ptr n = m_manager->nodeByName("fr.mikrosimage.dukex.transport");
    if (n.get() != NULL) {
        TransportNode::ptr t = boost::dynamic_pointer_cast<TransportNode>(n);
        if (t.get() != NULL) {
            t->gotoFrame(pos);
        }
    }
}

void UITimeline::framerateChanged(double framerate) {
    INode::ptr n = m_manager->nodeByName("fr.mikrosimage.dukex.playlist");
    if (n.get() != NULL) {
        PlaylistNode::ptr p = boost::dynamic_pointer_cast<PlaylistNode>(n);
        if (p.get() != NULL) {
            p->setFramerate(framerate);
        }
    }
}

void UITimeline::fit() {
    double factor = m_tracksView->duration() - m_tracksView->visibleRect().width();
    if (factor < 0.) {
        while (factor < 0 && m_zoom > MINZOOMRATIO) {
            zoomOut();
            factor = m_tracksView->duration() - m_tracksView->visibleRect().width();
        }
    } else {
        while (factor > 0 && m_zoom < MAXZOOMRATIO) {
            zoomIn();
            factor = m_tracksView->duration() - m_tracksView->visibleRect().width();
        }
    }
}

void UITimeline::changeZoom(int factor) {
    m_tracksRuler->setPixelPerMark(factor);
    double scale = (double) FRAME_SIZE / m_tracksRuler->comboScale[factor];
    m_tracksView->setScale(scale);
}

void UITimeline::zoomIn() {
    if (m_zoom < MAXZOOMRATIO)
        m_zoom++;
    changeZoom(m_zoom);
}

void UITimeline::zoomOut() {
    if (m_zoom > MINZOOMRATIO)
        m_zoom--;
    changeZoom(m_zoom);
}

void UITimeline::setDuration(int duration) {
    m_tracksView->setDuration(duration);
    m_tracksRuler->setDuration(duration);
}

// private
void UITimeline::launchUpdateLoop(){
    m_timerID = QObject::startTimer(40);
}

// private
void UITimeline::timerEvent(QTimerEvent *event) {
    INode::ptr n = m_manager->nodeByName("fr.mikrosimage.dukex.info");
    if (n.get() != NULL) {
        InfoNode::ptr p = boost::dynamic_pointer_cast<InfoNode>(n);
        if (p.get() != NULL) {
            p->callCurrentCacheState();
        }
    }
    event->accept();
}
