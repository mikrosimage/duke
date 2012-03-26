#include "TimelinePlugin.h"
#include "UITimeline.h"
#include <iostream>
#include <QtGui>

// DukeXUIPlugin
QStringList TimelinePlugin::describe() const {
    return QStringList() << tr("Author: nro") << tr("Version: 0.0.1 - 2011");
}

void TimelinePlugin::initialize(IUIBuilder* _builder, NodeManager* _manager) {
    if (!_builder || !_manager)
            return;

    // window
    UITimeline* timeline = new UITimeline(_manager);
    QDockWidget * dockwidget = _builder->createWindow(this, Qt::BottomDockWidgetArea, false);
    dockwidget->setWindowTitle("Timeline");
    dockwidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dockwidget->setWidget(timeline);
    dockwidget->adjustSize();

    // register as observer
    _builder->addObserver(this, timeline);
}

QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(timeline, TimelinePlugin)
QT_END_NAMESPACE
