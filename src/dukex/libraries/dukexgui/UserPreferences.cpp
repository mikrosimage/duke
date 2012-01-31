#include "UserPreferences.h"
#include <QtGui>
#include <boost/foreach.hpp>
#include <iostream>

UserPreferences::UserPreferences() :
    m_Settings("/tmp/dukex/dukex.ini", QSettings::IniFormat) {
}

void UserPreferences::loadShortcuts(QObject *pObject) {
    m_Settings.beginGroup("/Shortcuts/" + pObject->objectName());

    QList<QAction *> actions = pObject->findChildren<QAction *> ();
    QListIterator<QAction *> iter(actions);
    while (iter.hasNext()) {
        QAction *pAction = iter.next();
        if (pAction->objectName().isEmpty())
            continue;
        const QString& sKey = '/' + pAction->objectName();
        const QString& sValue = m_Settings.value('/' + sKey).toString();
        if (sValue.isEmpty())
            continue;
        pAction->setShortcut(QKeySequence(sValue));
    }
    m_Settings.endGroup();
}

void UserPreferences::saveShortcuts(QObject *pObject) {
    m_Settings.beginGroup("/Shortcuts/" + pObject->objectName());

    QList<QAction *> actions = pObject->findChildren<QAction *> ();
    QListIterator<QAction *> iter(actions);
    while (iter.hasNext()) {
        QAction *pAction = iter.next();
        if (pAction->objectName().isEmpty())
            continue;
        const QString& sKey = '/' + pAction->objectName();
        const QString& sValue = QString(pAction->shortcut());
        if (!sValue.isEmpty())
            m_Settings.setValue(sKey, sValue);
        else if (m_Settings.contains(sKey))
            m_Settings.remove(sKey);
    }
    m_Settings.endGroup();
}

void UserPreferences::loadFileHistory() {
    m_vRecentFiles.clear();
    m_Settings.beginGroup("/FileHistory");

    QStringList keys = m_Settings.childKeys();
    QListIterator<QString> iter(keys);
    while (iter.hasNext()) {
        QString key = iter.next();
        if (key.isEmpty())
            continue;
        const QString& sKey = '/' + key;
        const QString& sValue = m_Settings.value('/' + sKey).toString();
        if (sValue.isEmpty())
            continue;
        addToHistory(sValue.toStdString());
    }

    m_Settings.endGroup();
}

void UserPreferences::saveFileHistory() {
    m_Settings.beginGroup("/FileHistory");

    size_t index = 0;
    BOOST_REVERSE_FOREACH(const std::string& s, m_vRecentFiles)
                {
                    const QString& sKey = QString("/file") + QString::number(index++);
                    const QString& sValue = s.c_str();
                    if (!sValue.isEmpty())
                        m_Settings.setValue(sKey, sValue);
                    else if (m_Settings.contains(sKey))
                        m_Settings.remove(sKey);
                }

    m_Settings.endGroup();
}

const std::vector<std::string> & UserPreferences::history() {
    return m_vRecentFiles;
}

const std::string UserPreferences::history(size_t i) {
    if (i > m_vRecentFiles.size())
        return "";
    return m_vRecentFiles.at(i);
}

void UserPreferences::addToHistory(const std::string & name) {
    removeFromHistory(name);
    m_vRecentFiles.insert(m_vRecentFiles.begin(), name);
    if (m_vRecentFiles.size() > m_MaxRecentFiles)
        m_vRecentFiles.resize(m_MaxRecentFiles);
}

void UserPreferences::removeFromHistory(const std::string & name) {
    for (size_t i = 0; i < m_vRecentFiles.size(); ++i) {
        if (m_vRecentFiles[i] == name) {
            m_vRecentFiles.erase(m_vRecentFiles.begin() + i);
            --i;
        }
    }
}
