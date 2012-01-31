#ifndef TIMELINEPLUGIN_H
#define TIMELINEPLUGIN_H

#include <dukexgui/IUIPluginInterfaces.h>

class TimelinePlugin : public QObject, public DukeXUIPlugin {

Q_OBJECT //
Q_INTERFACES(DukeXUIPlugin) //

public:
    // DukeXUIPlugin
    QStringList describe() const;
    void initialize(IUIBuilder*, NodeManager*);
};

#endif // TIMELINEPLUGIN_H
