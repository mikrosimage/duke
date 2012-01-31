#include "UIPluginDialog.h"
#include "IUIPluginInterfaces.h"
#include <QPushButton>
#include <iostream>

UIPluginDialog::UIPluginDialog(QWidget *parent, IUIBuilder* _builder, NodeManager* _manager, const QString &path) :
    QDialog(parent), m_Loader(_builder, _manager) {

    ui.setupUi(this);
    ui.treeWidget->setColumnWidth(0, 300);
    ui.treeWidget->setColumnWidth(1, 50);
    m_interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
    m_interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
    m_featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    m_Loader.find(path);
    populate();
}

// private slot
void UIPluginDialog::load(bool _b) {
    QPushButton *button = qobject_cast<QPushButton *> (sender());
    if (button) {
        const QString& file = button->property("fileToLoad").toString();
        if (file.isEmpty())
            return;
        if (_b) {
            m_Loader.load(file);
            button->setText("Unload");
        } else {
            m_Loader.unload(file);
            button->setText("Load");
        }
    }
}

// private
void UIPluginDialog::populate() {

    ui.label->setText(tr("DukeX found the following plugins:\n"
        "(looked in %1)") .arg(QDir::toNativeSeparators(m_Loader.path())));

    ui.treeWidget->clear();

    const QMap<QString, QObject*>& plugins = m_Loader.foundPlugins();
    QMap<QString, QObject*>::const_iterator it = plugins.begin();
    while (it != plugins.end()) {
        DukeXUIPlugin *p = qobject_cast<DukeXUIPlugin *> (it.value());
        if (!p) {
            ++it;
            continue;
        }

        QTreeWidgetItem *pluginItem = new QTreeWidgetItem(ui.treeWidget);

        // displaying infos
        // filename on column 0
        QString filename = QDir(m_Loader.path()).relativeFilePath(it.key());
        pluginItem->setText(0, filename);
        // load/unload button on column 1
        QPushButton* button = new QPushButton("Load", ui.treeWidget);
        button->setProperty("fileToLoad", it.key());
        button->setCheckable(true);
        connect(button, SIGNAL(toggled(bool)), this, SLOT(load(bool)));
        ui.treeWidget->setItemWidget(pluginItem, 1, button);

        // plugin description in subitems
        addItems(pluginItem, "DukeXUIPlugin", p->describe());

        // Hard coded AutoLoad
        // TODO: store AutoLoad settings in user preferences
        if(QDir(m_Loader.path()).relativeFilePath(it.key()) == "plugin_dukex_timeline.so"){
            button->setChecked(true);
        }

        ++it;
    }
}

// private
void UIPluginDialog::addItems(QTreeWidgetItem *pluginItem, const char *interfaceName, const QStringList &features) {
    QTreeWidgetItem *interfaceItem = new QTreeWidgetItem(pluginItem);
    interfaceItem->setText(0, interfaceName);
    interfaceItem->setIcon(0, m_interfaceIcon);

    foreach (QString feature, features)
        {
            if (feature.endsWith("..."))
                feature.chop(3);
            QTreeWidgetItem *featureItem = new QTreeWidgetItem(interfaceItem);
            featureItem->setText(0, feature);
            featureItem->setIcon(0, m_featureIcon);
        }
}
