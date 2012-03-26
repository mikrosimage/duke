#ifndef IMAGEINFOPLUGIN_H
#define IMAGEINFOPLUGIN_H

#include <dukexgui/IUIPluginInterfaces.h>

class UIImageInfo;

class ImageInfoPlugin : public QObject, public DukeXUIPlugin {

Q_OBJECT //
Q_INTERFACES(DukeXUIPlugin) //

public:
    ImageInfoPlugin();

public:
    // DukeXUIPlugin
    QStringList describe() const;
    void initialize(IUIBuilder*, NodeManager*);

private slots:
    void actionTriggered(bool _b);

private:
    QDockWidget * mDockWidget;
    QAction * mAction;
};

#endif // IMAGEINFOPLUGIN_H
