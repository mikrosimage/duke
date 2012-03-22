#include "UIRenderWindow.h"
#include <iostream>

UIRenderWindow::UIRenderWindow(QWidget* _parent) :
    QWidget(_parent), mRenderWidget(new QWidget(this)), mZoomRatio(1.) {

    setMinimumSize(320, 240);
//            mRenderWidget->setAttribute(Qt::WA_PaintOnScreen);
//            mRenderWidget->setAttribute(Qt::WA_OpaquePaintEvent);
//            mRenderWidget->setAttribute(Qt::WA_PaintUnclipped);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mRenderWidget);
    setLayout(layout);
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

void UIRenderWindow::keyPressEvent(QKeyEvent * event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            showNormal();
            break;
        default:
            break;
    }
}

void UIRenderWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void UIRenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QPointF pan = event->globalPos() - mDragPosition;
        pan.setX(-pan.x() / width());
        pan.setY(-pan.y() / height());
        emit panChanged(pan.x()/pow(mZoomRatio, 2), pan.y()/pow(mZoomRatio, 2));
        event->accept();
    }
}

void UIRenderWindow::mouseReleaseEvent(QMouseEvent * event) {
}

void UIRenderWindow::wheelEvent(QWheelEvent * event) {
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    mZoomRatio += (numSteps * 6.66) / 100;
    if (mZoomRatio < 0.5)
        mZoomRatio = 0.5;
    emit zoomChanged(pow(mZoomRatio, 2));
    event->accept();
}

void* UIRenderWindow::renderWindowID() const {
    return (void*) mRenderWidget->winId();
}

QWidget* UIRenderWindow::renderWidget() const {
    return mRenderWidget;
}

