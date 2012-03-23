#ifndef IUIBUILDER_H
#define IUIBUILDER_H

#include "UIWidget.h"
#include <QObject>
#include <QMultiMap>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class IUIBuilder {

public:
    typedef boost::shared_ptr<IUIBuilder> ptr;

public:
    IUIBuilder() {
    }
    virtual ~IUIBuilder() {
    }

public:
    virtual bool createMenu(QObject* _plugin, QMenu* _menu, const QString & _previousAction) = 0;
    virtual bool createWindow(QObject* _plugin, UIWidget* _widget, const Qt::DockWidgetArea & _area, const QString & _title, bool floating = false) = 0;
//    virtual QDeclarativeItem* createQMLWindow(QObject* _plugin, const QUrl &qmlfile, const Qt::DockWidgetArea & _area, const QString & _title) = 0;

public:
    virtual void closeUI(QObject* _plug) = 0;

protected:
    QMultiMap<QObject*, QObject*> m_LoadedUIElements;
};

#endif // IUIBUILDER_H
