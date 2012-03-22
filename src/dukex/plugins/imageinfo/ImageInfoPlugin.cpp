#include "ImageInfoPlugin.h"
#include "UIImageInfo.h"
#include <iostream>
#include <QtGui>

// DukeXUIPlugin
QStringList ImageInfoPlugin::describe() const {
    return QStringList() << tr("Author: nro") << tr("Version: 0.0.1 - 2012");
}

void ImageInfoPlugin::initialize(IUIBuilder* _builder, NodeManager* _manager) {
    if (!_builder)
        return;
    UIImageInfo* imageInfo = new UIImageInfo(_manager);
    _builder->createWindow(this, imageInfo, Qt::LeftDockWidgetArea, "ImageInfo", true);

//    QMenu* menu = new QMenu("Image Info");
//    menu->addAction("test01");
//    menu->addAction("test02");
//    menu->addAction("test03");
//    _builder->createMenu(this, menu, "menuDisplay");
}

QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(ImageInfo, ImageInfoPlugin)
QT_END_NAMESPACE
