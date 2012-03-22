#ifndef UIVIDEOPAGE_H
#define UIVIDEOPAGE_H

#include "ui_videopage.h"
#include <QFileInfo>

class UIVideoPage : public QWidget {

    Q_OBJECT //
    Q_DISABLE_COPY(UIVideoPage) //

public:
    UIVideoPage(QWidget* parent = 0) :
        QWidget(parent) {
        ui.setupUi(this);
    }
    virtual ~UIVideoPage() {
    }

public:
    void setFilename(const std::string & s) {
        QFileInfo fi(QString::fromStdString(s));
        ui.fileLabel->setText(fi.fileName());
    }

    void setPath(const std::string & s) {
        QFileInfo fi(QString::fromStdString(s));
        ui.pathTextEdit->setText(fi.path());
    }

    void setWidth(const size_t n) {
        ui.widthLabel->setText(QString::number(n));
    }

    void setHeight(const size_t n) {
        ui.heightLabel->setText(QString::number(n));
    }

    void setDepth(const size_t n) {
        ui.depthLabel->setText(QString::number(n));
    }

    void setFormat(const std::string & s) {
        ui.formatLabel->setText(QString::fromStdString(s));
    }

    void setImageSize(const size_t n) {
        ui.imageSizeLabel->setText(QString::number((n/1024.)/1024.));
    }

    void setFileSize(const size_t n) {
        ui.fileSizeLabel->setText(QString::number((n/1024.)/1024.));
    }

private:
    Ui::VideoPage ui;
};

#endif // UIVIDEOPAGE_H
