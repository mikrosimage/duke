#ifndef UIEXTENDEDLABEL_H
#define UIEXTENDEDLABEL_H

#include <QLabel>

class UIExtendedLabel : public QLabel {

    Q_OBJECT

public:
    UIExtendedLabel(QWidget* parent);
    UIExtendedLabel(const QString& text, QWidget* parent);
    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode mode);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    void setText(const QString &text);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseDoubleClickEvent(QMouseEvent *ev);

private:
    Qt::TextElideMode m_elideMode;
    QString m_text;

signals:
    void clicked(QWidget* sender, QMouseEvent* ev);
    void doubleClicked();
};

#endif // UIEXTENDEDLABEL_H
