#ifndef UIPLUGINDIALOG_H
#define UIPLUGINDIALOG_H

#include "ui_plugindialog.h"
#include "UIPluginLoader.h"
#include <QDialog>
#include <QIcon>
#include <QDir>

class UIPluginDialog : public QDialog {
Q_OBJECT

public:
    UIPluginDialog(QWidget*, IUIBuilder*, NodeManager*, const QString&);

private:
    void populate();
    void addItems(QTreeWidgetItem* pluginItem, const char *interfaceName, const QStringList &features);

private slots:
    void load(bool);

private:
    Ui::pluginDialog ui;
    UIPluginLoader m_Loader;
    QIcon m_interfaceIcon;
    QIcon m_featureIcon;
};

#endif // UIPLUGINDIALOG_H
