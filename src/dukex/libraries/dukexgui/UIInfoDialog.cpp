#include "UIInfoDialog.h"
#include <QMouseEvent>
#include <iostream>

UIInfoDialog::UIInfoDialog(QWidget * parent, Qt::WindowFlags flags) :
    QDialog(parent, flags) {
    ui.setupUi(this);
}
