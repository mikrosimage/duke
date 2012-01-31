#ifndef IUIBUILDER_H
#define IUIBUILDER_H

#include "UIWidget.h"
#include <QObject>
#include <QList>
#include <QMultiMap>

class IUIBuilder {

public:
    typedef boost::shared_ptr<IUIBuilder> ptr;

public:
    IUIBuilder() {
    }
    virtual ~IUIBuilder() {
    }

public:
//    virtual QMenu* createMenu(QObject* _plugin, const QString & _title) = 0;
//    virtual QWidget* createWindow(QObject* _plugin, const Qt::DockWidgetArea & _area, const QString & _title) = 0;
    virtual bool createWindow(QObject* _plugin, UIWidget* _widget, const Qt::DockWidgetArea & _area, const QString & _title) = 0;
//    virtual QDeclarativeItem* createQMLWindow(QObject* _plugin, const QUrl &qmlfile, const Qt::DockWidgetArea & _area, const QString & _title) = 0;

public:
    virtual void closeUI(QObject* _plug) {
        QList<QObject*> values = m_LoadedUIElements.values(_plug);
        for (int i = 0; i < values.size(); ++i)
            delete values.at(i);
    }

protected:
    QMultiMap<QObject*, QObject*> m_LoadedUIElements;
};

#endif // IUIBUILDER_H
