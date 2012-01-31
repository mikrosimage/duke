#include "UIRenderWindow.h"
#include <iostream>

UIRenderWindow::UIRenderWindow(QWidget* _parent) :
    QWidget(_parent), m_renderWidget(NULL), zoomRatio(1.), mViewMode(VM_PANZOOM) {
    setMinimumSize(320, 240);

    m_renderWidget = new QWidget(this);
    //        m_renderWidget->setAttribute(Qt::WA_PaintOnScreen);
    //        m_renderWidget->setAttribute(Qt::WA_OpaquePaintEvent);
    //        m_renderWidget->setAttribute(Qt::WA_PaintUnclipped);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_renderWidget);
    setLayout(layout);

    mDialog = new QDialog(this, Qt::CustomizeWindowHint);
    mDialog->setMinimumSize(100, 100);
    mDialog->setLayout(new QGridLayout(mDialog));
    mDialog->layout()->addWidget(new QPushButton("hello world"));
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
    if(event->button() == Qt::RightButton){
        mViewMode = VM_SHOWIMGINFO;
        mDialog->move(event->globalPos());
        mDialog->show();
    } else {
        mViewMode = VM_PANZOOM;
        onMousePressPos = event->posF();
    }
    event->accept();
}

void UIRenderWindow::mouseMoveEvent(QMouseEvent * event) {
    if(mViewMode == VM_SHOWIMGINFO){
        mDialog->move(event->globalPos());
        std::cerr << "move mDialog" << std::endl;
    } else {
        QPointF pan = onMousePressPos - event->posF();
        pan.setX(pan.x()/width());
        pan.setY(pan.y()/height());
        pan += currentCenterPos;
        emit panChanged(pan.x()/pow(zoomRatio, 2), pan.y()/pow(zoomRatio, 2));
    }
    event->accept();
}

void UIRenderWindow::mouseReleaseEvent(QMouseEvent * event) {
    if(mViewMode == VM_SHOWIMGINFO){
        mDialog->hide();
    } else {
        QPointF pan = currentCenterPos + (onMousePressPos - event->posF());
        pan.setX(pan.x()/width());
        pan.setY(pan.y()/height());
        currentCenterPos += pan;
    }
    event->accept();
}

void UIRenderWindow::wheelEvent(QWheelEvent * event) {
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    zoomRatio += (numSteps *6.66) /100;

    if(zoomRatio < 0.5)
        zoomRatio = 0.5;

    emit zoomChanged(pow(zoomRatio, 2));
    event->accept();
}

void* UIRenderWindow::renderWindowID() const {
    return (void*) m_renderWidget->winId();
}

