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
    // global filedialog settings
    setViewMode(QFileDialog::Detail);
    setFileMode(QFileDialog::ExistingFiles);
    setNameFilter(tr("All Files (*.*);;Playlists (*.ppl *.ppl2);;Images (*.pic *.pbm *.dpx *.tpic *.png *.pgm *.jpg *.pnm *.bmp *.exr *.sgi *.tiff *.tif *.dds *.tga *.jp2 *.rgb *.j2k *.jpeg *.ico *.hdr *.ppm)"));

    // custom checkbox
    mSequenceCheckbox = new QCheckBox(this);
    mSequenceCheckbox->setText("Load As Sequence");
    QGridLayout *layout = (QGridLayout*)this->layout();
    layout->addWidget(mSequenceCheckbox, 4, 0);
}
