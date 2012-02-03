#ifndef UIFILEDIALOG_H
#define UIFILEDIALOG_H

#include <QFileDialog>
#include <QCheckBox>

class UIFileDialog : public QFileDialog {

    Q_OBJECT

public:
    UIFileDialog(QWidget * parent, Qt::WindowFlags flags);
    UIFileDialog(QWidget * parent = 0, const QString & caption = QString(), const QString & directory = QString(), const QString & filter = QString());

public:
    bool asSequence() const;

private:
    void setupCustomUI();

private:
    QCheckBox * mSequenceCheckbox;
};

#endif // UIFILEDIALOG_H
