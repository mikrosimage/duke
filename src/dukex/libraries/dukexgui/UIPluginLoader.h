#ifndef UIPLUGINLOADER_H
#define UIPLUGINLOADER_H

#include <QDir>
#include <QMap>
#include "IUIPluginInterfaces.h"

class UIPluginLoader {

public:
    UIPluginLoader(IUIBuilder*, NodeManager*);

public:
    void find(const QString&);
    QObject* load(const QString&);
    QObjectList loadAll();
    void unload(const QString&);
    void unloadAll();

public:
    const QMap<QString, QObject*>& foundPlugins() const {
        return m_FoundPlugins;
    }
    QString path() const {
        return m_PluginsDir.absolutePath();
    }

private:
    QDir m_PluginsDir;
    QMap<QString, QObject*> m_FoundPlugins;
    IUIBuilder* m_Container;
    NodeManager* m_Manager;
};

#endif // UIPLUGINLOADER_H
