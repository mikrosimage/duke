#ifndef UIWIDGET_H
#define UIWIDGET_H

#include <dukexcore/dkxIObserver.h>
#include <QWidget>

class UIWidget : public QWidget, public IObserver {

Q_OBJECT

public:
    UIWidget() { // QWidget* parent > set by UIBuilder
    }
    virtual ~UIWidget() {
    }

};

#endif // UIWIDGET_H
