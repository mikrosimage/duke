#include "UITrackControls.h"
#include "UIGraphicsTrack.h"
#include <QInputDialog>

UITrackControls::UITrackControls(UIGraphicsTrack* track, QWidget *parent) :
    QWidget(parent), m_track(track) {
    m_ui.setupUi(this);
    if (track->mediaType() != VideoTrack)
        m_ui.fxButton->hide();
    setTrackDisabled(!m_track->isEnabled());
    connect(m_ui.disableButton, SIGNAL( clicked(bool) ), this, SLOT( setTrackDisabled(bool) ));
    connect(m_ui.trackLabel, SIGNAL(doubleClicked()), this, SLOT(trackNameDoubleClicked()));
    connect(m_ui.fxButton, SIGNAL(clicked()), this, SLOT(fxButtonClicked()));
    updateTextLabels();
}

void UITrackControls::updateTextLabels() {
    if (m_track->mediaType() == VideoTrack) {
        if (m_title.isEmpty() == true)
            m_ui.trackLabel->setText(tr("Video #%1").arg(QString::number(m_track->trackNumber() + 1)));
        else
            m_ui.trackLabel->setText(m_title);
    } else if (m_track->mediaType() == AudioTrack) {
        if (m_title.isEmpty())
            m_ui.trackLabel->setText(tr("Audio #%1").arg(QString::number(m_track->trackNumber() + 1)));
        else
            m_ui.trackLabel->setText(m_title);
    }
}

void UITrackControls::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            m_ui.retranslateUi(this);
            updateTextLabels();
            break;
        default:
            break;
    }
}

void UITrackControls::setTrackDisabled(bool disable) {
    m_track->setTrackEnabled(!disable);
    if (!disable) {
        if (m_track->mediaType() == VideoTrack)
            m_ui.disableButton->setIcon(QIcon(":/images/trackon"));
        else if (m_track->mediaType() == AudioTrack)
            m_ui.disableButton->setIcon(QIcon(":/images/hpon"));
    } else {
        if (m_track->mediaType() == VideoTrack)
            m_ui.disableButton->setIcon(QIcon(":/images/trackoff"));
        else if (m_track->mediaType() == AudioTrack)
            m_ui.disableButton->setIcon(QIcon(":/images/hpoff"));
    }
}

void UITrackControls::trackNameDoubleClicked() {
    QString name = QInputDialog::getText(NULL, tr("Rename track"), tr("Enter the new track name"));
    if (name.isEmpty() == false) {
        m_title = name;
        updateTextLabels();
    }
}

void UITrackControls::fxButtonClicked() {
}
