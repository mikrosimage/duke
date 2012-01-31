#include "TimelinePlugin.h"
#include "UITimeline.h"
#include <iostream>
#include <QtGui>

// DukeXUIPlugin
QStringList TimelinePlugin::describe() const {
    return QStringList() << tr("Author: nro") << tr("Version: 0.0.1 - 2011");
}

void TimelinePlugin::initialize(IUIBuilder* _builder, NodeManager* _manager) {
    if (!_builder)
        return;
    UITimeline* timeline = new UITimeline(_manager);
    _builder->createWindow(this, timeline, Qt::BottomDockWidgetArea, "Timeline");
}

QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(timeline, TimelinePlugin)
QT_END_NAMESPACE
