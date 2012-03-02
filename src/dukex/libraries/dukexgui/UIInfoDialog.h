#ifndef UIINFODIALOG_H
#define UIINFODIALOG_H

#include "ui_infodialog.h"
#include <QDialog>

class UIInfoDialog : public QDialog {

    Q_OBJECT

public:
    UIInfoDialog(QWidget * parent, Qt::WindowFlags flags);

private:
    Ui::infoDialog ui;
};

#endif // UIINFODIALOG_H
