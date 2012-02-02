#include "UIFileDialog.h"
#include <QGridLayout>
#include <iostream>

UIFileDialog::UIFileDialog(QWidget * parent, Qt::WindowFlags flags) :
    QFileDialog(parent, flags) {
    setupCustomUI();
}

UIFileDialog::UIFileDialog(QWidget * parent, const QString & caption, const QString & directory, const QString & filter) :
    QFileDialog(parent, caption, directory, filter) {
    setupCustomUI();
}

bool UIFileDialog::asSequence() const {
    return mSequenceCheckbox->isChecked();
}

// private
void UIFileDialog::setupCustomUI() {
    setFileMode(QFileDialog::ExistingFiles);
    setNameFilter(tr("All Files (*.*);;Playlists (*.ppl *.ppl2);;Images (*.pic *.pbm *.dpx *.tpic *.png *.pgm *.jpg *.pnm *.bmp *.exr *.sgi *.tiff *.tif *.dds *.tga *.jp2 *.rgb *.j2k *.jpeg *.ico *.hdr *.ppm)"));
    mSequenceCheckbox = new QCheckBox(this);
    mSequenceCheckbox->setText("Load As Sequence");
    mDirectoryCheckbox = new QCheckBox(this);
    mDirectoryCheckbox->setText("Select Directory");
    connect(mDirectoryCheckbox, SIGNAL(toggled(bool)), this, SLOT(directorybox_toggled(bool)));
    QGridLayout *layout = (QGridLayout*)this->layout();
    layout->addWidget(mSequenceCheckbox, 4, 0);
    layout->addWidget(mDirectoryCheckbox, 4, 1);
}

// slot
void UIFileDialog::directorybox_toggled(bool b){
    if(b){
        setFileMode(QFileDialog::Directory);
    } else {
        setFileMode(QFileDialog::ExistingFiles);
        setNameFilter(tr("All Files (*.*);;Playlists (*.ppl *.ppl2);;Images (*.pic *.pbm *.dpx *.tpic *.png *.pgm *.jpg *.pnm *.bmp *.exr *.sgi *.tiff *.tif *.dds *.tga *.jp2 *.rgb *.j2k *.jpeg *.ico *.hdr *.ppm)"));
    }
}
