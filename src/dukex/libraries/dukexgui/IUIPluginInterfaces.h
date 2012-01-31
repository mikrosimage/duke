#ifndef IUIPLUGININTERFACES_H
#define IUIPLUGININTERFACES_H

#include "IUIBuilder.h"
#include <dukexcore/dkxNodeManager.h>
#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QStringList;
QT_END_NAMESPACE

class DukeXUIPlugin {
public:
    virtual ~DukeXUIPlugin() {
    }

public:
    virtual QStringList describe() const = 0;
    virtual void initialize(IUIBuilder*, NodeManager*) = 0;
};

QT_BEGIN_NAMESPACE Q_DECLARE_INTERFACE(DukeXUIPlugin, "fr.mikrosimage.dukeX.UIPlugin/1.0")
QT_END_NAMESPACE

#endif // IUIPLUGININTERFACES_H
