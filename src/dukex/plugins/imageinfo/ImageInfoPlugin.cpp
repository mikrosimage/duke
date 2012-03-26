#include "ImageInfoPlugin.h"
#include "UIImageInfo.h"
#include <iostream>
#include <QtGui>
#include <QtDebug>

ImageInfoPlugin::ImageInfoPlugin() :
    mDockWidget(NULL), mAction(NULL) {
}

// DukeXUIPlugin
QStringList ImageInfoPlugin::describe() const {
    return QStringList() << tr("Author: nro") << tr("Version: 0.0.1 - 2012");
}

void ImageInfoPlugin::initialize(IUIBuilder* _builder, NodeManager* _manager) {
    if (!_builder || !_manager)
        return;

    // window
    UIImageInfo * widget = new UIImageInfo(_manager);
    mDockWidget = _builder->createWindow(this, Qt::RightDockWidgetArea, true);
    mDockWidget->setWindowTitle("Image Info");
    mDockWidget->setWidget(widget);
    mDockWidget->adjustSize();
    mDockWidget->hide();
    // register as observer
    _builder->addObserver(this, widget);
    // action
    mAction = _builder->createAction(this, "menuDisplay");
    mAction->setText("Image &Info");
    mAction->setShortcut(Qt::Key_I);
    mAction->setCheckable(true);
    mAction->setChecked(false);
    connect(mAction, SIGNAL(triggered(bool)), this, SLOT(actionTriggered(bool)));
    connect(mDockWidget, SIGNAL(visibilityChanged(bool)), mAction, SLOT(setChecked(bool)));
}

void ImageInfoPlugin::actionTriggered(bool _b) {
    if (!mAction || !mDockWidget)
        return;
    mDockWidget->setVisible(_b);
    if(_b && mDockWidget->isTopLevel())
        mDockWidget->adjustSize();
}

QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(ImageInfo, ImageInfoPlugin)
QT_END_NAMESPACE
