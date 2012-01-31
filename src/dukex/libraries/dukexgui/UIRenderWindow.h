#ifndef UIRENDERWINDOW_H
#define UIRENDERWINDOW_H

#include <QtGui>

class UIRenderWindow : public QWidget {

Q_OBJECT
    enum VIEW_MODE {
        VM_PANZOOM = 0, VM_SHOWIMGINFO
    };

public:
    UIRenderWindow(QWidget* parent);
    ~UIRenderWindow();
    void showFullScreen();
    void showNormal();
    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);

public:
    void* renderWindowID() const;

signals:
    void zoomChanged(double);
    void panChanged(double, double);

private:
    QWidget* m_renderWidget;
    double zoomRatio;
    QPointF onMousePressPos;
    QPointF currentCenterPos;

    QDialog* mDialog;
    VIEW_MODE mViewMode;
};

#endif // UIRENDERWINDOW_H
