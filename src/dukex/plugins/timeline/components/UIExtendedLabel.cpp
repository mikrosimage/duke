#include "UIExtendedLabel.h"
#include <QResizeEvent>

UIExtendedLabel::UIExtendedLabel(QWidget *parent) :
    QLabel(parent), m_elideMode(Qt::ElideMiddle) {
}

UIExtendedLabel::UIExtendedLabel(const QString &text, QWidget *parent) :
    QLabel(text, parent), m_elideMode(Qt::ElideMiddle), m_text(text) {
    setToolTip(text);
}

void UIExtendedLabel::resizeEvent(QResizeEvent *event) {
    QFontMetrics fm(fontMetrics());
    QString str = fm.elidedText(m_text, m_elideMode, event->size().width());
    QLabel::setText(str);
    QLabel::resizeEvent(event);
}

Qt::TextElideMode UIExtendedLabel::elideMode() const {
    return m_elideMode;
}

void UIExtendedLabel::setElideMode(Qt::TextElideMode mode) {
    m_elideMode = mode;
}

QSize UIExtendedLabel::minimumSizeHint() const {
    if (pixmap() != NULL)
        return QLabel::sizeHint();
    const QFontMetrics &fm = fontMetrics();
    QSize size(fm.width("..."), fm.height());
    return size;
}

QSize UIExtendedLabel::sizeHint() const {
    if (pixmap() != NULL)
        return QLabel::sizeHint();
    const QFontMetrics& fm = fontMetrics();
    QSize size(fm.width(m_text), fm.height());
    return size;
}

void UIExtendedLabel::setText(const QString &text) {
    m_text = text;
    setToolTip(text);
    QLabel::setText(text);
}

void UIExtendedLabel::mousePressEvent(QMouseEvent* ev) {
    emit clicked(this, ev);
}

void UIExtendedLabel::mouseDoubleClickEvent(QMouseEvent *) {
    emit doubleClicked();
}
