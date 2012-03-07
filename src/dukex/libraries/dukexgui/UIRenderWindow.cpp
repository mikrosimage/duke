#include "UIRenderWindow.h"
#include "UIInfoDialog.h"
#include <iostream>

UIRenderWindow::UIRenderWindow(QWidget* _parent) :
    QWidget(_parent), m_renderWidget(NULL), zoomRatio(1.), mMode(MM_PANZOOM) {
    setMinimumSize(320, 240);

    m_renderWidget = new QWidget(this);
    //        m_renderWidget->setAttribute(Qt::WA_PaintOnScreen);
    //        m_renderWidget->setAttribute(Qt::WA_OpaquePaintEvent);
    //        m_renderWidget->setAttribute(Qt::WA_PaintUnclipped);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_renderWidget);
    setLayout(layout);

    mDialog = new UIInfoDialog(this, Qt::CustomizeWindowHint);
    mDialog->setWindowOpacity(0.6);
}

UIRenderWindow::~UIRenderWindow() {
}

void UIRenderWindow::showFullScreen() {
    setWindowFlags(Qt::Window);
    QWidget::showFullScreen();
}

void UIRenderWindow::showNormal() {
    setWindowFlags(Qt::Widget);
    QWidget::showNormal();
}

//void UIRenderWindow::showInfo() {
//    if (mDialog->isVisible())
//        mDialog->hide();
//    else
//        mDialog->show();
//}

void UIRenderWindow::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            showNormal();
            break;
        default:
            break;
    }
}

void UIRenderWindow::mousePressEvent(QMouseEvent * event) {
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::RightButton) {
//        mDialog->move(event->pos());
        mMode = MM_INFODIALOG;
    } else {
        onMousePressPos = event->posF();
        mMode = MM_PANZOOM;
    }
    event->accept();
}

void UIRenderWindow::mouseMoveEvent(QMouseEvent * event) {
    QWidget::mouseMoveEvent(event);
    if (mMode==MM_INFODIALOG) {
//        mDialog->move(event->pos());
    } else {
        QPointF pan = onMousePressPos - event->posF();
        pan.setX(pan.x() / width());
        pan.setY(pan.y() / height());
        pan += currentCenterPos;
        emit panChanged(pan.x() / pow(zoomRatio, 2), pan.y() / pow(zoomRatio, 2));
    }
    event->accept();
}

void UIRenderWindow::mouseReleaseEvent(QMouseEvent * event) {
    QWidget::mouseReleaseEvent(event);
    if (mMode==MM_INFODIALOG) {
//        mDialog->move(event->pos());
    } else {
        QPointF pan = currentCenterPos + (onMousePressPos - event->posF());
        pan.setX(pan.x() / width());
        pan.setY(pan.y() / height());
        currentCenterPos += pan;
    }
    event->accept();
}

void UIRenderWindow::wheelEvent(QWheelEvent * event) {
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    zoomRatio += (numSteps * 6.66) / 100;

    if (zoomRatio < 0.5)
        zoomRatio = 0.5;

    emit zoomChanged(pow(zoomRatio, 2));
    event->accept();
}

void* UIRenderWindow::renderWindowID() const {
    return (void*) m_renderWidget->winId();
}

