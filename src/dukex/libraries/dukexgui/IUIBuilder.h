#ifndef IUIBUILDER_H
#define IUIBUILDER_H

#include <dukexcore/dkxIObserver.h>
#include <QObject>
#include <QMultiMap>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QDockWidget;
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
    virtual void addObserver(QObject* _plugin, IObserver* _observer) = 0;
    virtual QAction* createAction(QObject* _plugin, const QString & _parentMenuName) = 0;
    virtual QMenu* createMenu(QObject* _plugin, const QString & _parentMenuName) = 0;
    virtual QDockWidget* createWindow(QObject* _plugin, Qt::DockWidgetArea _area, bool floating) = 0;

public:
    virtual void closeUI(QObject* _plug) = 0;

protected:
    QMultiMap<QObject*, QObject*> m_LoadedUIElements;
    QMultiMap<QObject*, IObserver*> m_RegisteredObservers;
};

#endif // IUIBUILDER_H
