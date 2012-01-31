#include "UIPluginLoader.h"
#include <QPluginLoader>
#include <iostream>

UIPluginLoader::UIPluginLoader(IUIBuilder* _container, NodeManager* _manager) :
    m_Container(_container), m_Manager(_manager) {
}

void UIPluginLoader::find(const QString& path) {
    m_PluginsDir = QDir(path);
    m_FoundPlugins.clear();
    QFileInfoList list = m_PluginsDir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QPluginLoader loader(fileInfo.absoluteFilePath());
        QObject* plugin = loader.instance();
        loader.dumpObjectInfo();
        //TODO log / verbose mode
        //std::cerr << loader.errorString().toStdString() << std::endl;
        if (plugin) {
            m_FoundPlugins[fileInfo.absoluteFilePath()] = plugin;
        }
    }
}

QObject* UIPluginLoader::load(const QString& filename) {
    QPluginLoader loader(m_PluginsDir.absoluteFilePath(filename));
    QObject *plugin = loader.instance();
    if (plugin) {
        DukeXUIPlugin *iUI = qobject_cast<DukeXUIPlugin *> (plugin);
        if (iUI && m_Container) {
            iUI->initialize(m_Container, m_Manager);
            return plugin;
        }
    }
    return NULL;
}

QObjectList UIPluginLoader::loadAll() {
    QObjectList list;
    QMap<QString, QObject*>::const_iterator it = m_FoundPlugins.begin();
    while (it != m_FoundPlugins.end()) {
        QObject* plug = load(it.key());
        if (plug)
            list << plug;
        it++;
    }
    return list;
}

void UIPluginLoader::unload(const QString& filename) {
    QPluginLoader loader(m_PluginsDir.absoluteFilePath(filename));
    QObject *plugin = loader.instance();
    if (plugin) {
        DukeXUIPlugin *iUI = qobject_cast<DukeXUIPlugin *> (plugin);
        if (iUI && m_Container) {
            m_Container->closeUI(plugin);
        }
    }
    delete plugin;
}

void UIPluginLoader::unloadAll() {
    QMap<QString, QObject*>::const_iterator it = m_FoundPlugins.begin();
    while (it != m_FoundPlugins.end()) {
        unload(it.key());
        it++;
    }
}
