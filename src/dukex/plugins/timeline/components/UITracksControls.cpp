#include "UITracksControls.h"
#include "UITrackControls.h"
#include "UIGraphicsTrack.h"
#include <QVBoxLayout>

UITracksControls::UITracksControls(QWidget* parent) :
    QScrollArea(parent) {
    // Never show the scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setContentsMargins(0, 0, 0, 0);
    setFrameStyle(QFrame::NoFrame);

    m_centralWidget = new QWidget();
    m_centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_layout = new QVBoxLayout;
    m_layout->setSizeConstraint(QLayout::SetMinimumSize);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_separator = new QWidget;
    m_separator->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_separator->setMinimumHeight(5);
    m_separator->setMaximumHeight(5);
    m_layout->addWidget(m_separator);

    m_centralWidget->setLayout(m_layout);
    setWidget(m_centralWidget);
}

void UITracksControls::addVideoTrack(UIGraphicsTrack* track) {
    UITrackControls* item = new UITrackControls(track, m_centralWidget);
    item->setMinimumWidth(108);
    item->setMinimumHeight(track->height());
    item->setContentsMargins(0, 0, 0, 0);
    m_layout->insertWidget(0, item);
}

void UITracksControls::addAudioTrack(UIGraphicsTrack* track) {
    UITrackControls* item = new UITrackControls(track, m_centralWidget);
    item->setMinimumWidth(108);
    item->setMinimumHeight(track->height());
    item->setContentsMargins(0, 0, 0, 0);
    m_layout->insertWidget(-1, item);
}

void UITracksControls::removeVideoTrack() {
    QLayoutItem* item = m_layout->takeAt(0);
    if (item->widget())
        delete item->widget();
    delete item;

    // Force the central widget to shrink
    m_layout->activate();
    m_centralWidget->setGeometry(0, 0, 1, 1);
}

void UITracksControls::removeAudioTrack() {
    QLayoutItem* item = m_layout->takeAt(m_layout->count() - 1);
    if (item->widget())
        delete item->widget();
    delete item;

    // Force the central widget to shrink
    m_layout->activate();
    m_centralWidget->setGeometry(0, 0, 1, 1);
}

void UITracksControls::clear() {
    m_layout->removeWidget(m_separator);
    QLayoutItem* child;
    while ((child = m_layout->takeAt(0)) != 0) {
        if (child->widget())
            delete child->widget();
        delete child;
    }
    m_layout->addWidget(m_separator);

    // Force the central widget to shrink
    m_layout->activate();
    m_centralWidget->setGeometry(0, 0, 1, 1);
}
