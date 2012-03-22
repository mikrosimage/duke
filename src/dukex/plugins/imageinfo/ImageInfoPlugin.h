#ifndef IMAGEINFOPLUGIN_H
#define IMAGEINFOPLUGIN_H

#include <dukexgui/IUIPluginInterfaces.h>

class ImageInfoPlugin : public QObject, public DukeXUIPlugin {

Q_OBJECT //
Q_INTERFACES(DukeXUIPlugin) //

public:
    // DukeXUIPlugin
    QStringList describe() const;
    void initialize(IUIBuilder*, NodeManager*);
};

#endif // IMAGEINFOPLUGIN_H
