#include "UIGraphicsTrack.h"
#include "UITracksView.h"

UIGraphicsTrack::UIGraphicsTrack(TrackType type, quint32 trackNumber, QGraphicsItem *parent) :
    QGraphicsWidget(parent) {
    m_type = type;
    m_trackNumber = trackNumber;
    m_enabled = true;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setContentsMargins(0, 0, 0, 0);
    setZValue(1);
}

int UIGraphicsTrack::type() const {
    return Type;
}

void UIGraphicsTrack::setHeight(int height) {
    setPreferredHeight(height);
    adjustSize();
    updateGeometry();
}

int UIGraphicsTrack::height() {
    return preferredHeight();
}

void UIGraphicsTrack::setTrackEnabled(bool enabled) {
    if (enabled == m_enabled)
        return;
    m_enabled = enabled;
}

bool UIGraphicsTrack::isEnabled() {
    return m_enabled;
}

quint32 UIGraphicsTrack::trackNumber() {
    return m_trackNumber;
}

TrackType UIGraphicsTrack::mediaType() {
    return m_type;
}

QList<UIAbstractGraphicsItem*> UIGraphicsTrack::childs() {
    QList<UIAbstractGraphicsItem*> list;
    QList<QGraphicsItem*> items = childItems();
    UIAbstractGraphicsItem* item;
    for (int i = 0; i < items.count(); ++i) {
        item = dynamic_cast<UIAbstractGraphicsItem*> (items.at(i));
        if (!item)
            continue;
        list.append(item);
    }
    return list;
}
