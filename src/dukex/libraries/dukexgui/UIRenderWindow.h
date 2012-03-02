#ifndef UIRENDERWINDOW_H
#define UIRENDERWINDOW_H

#include <QtGui>

class UIInfoDialog;

class UIRenderWindow : public QWidget {

    Q_OBJECT

    enum MOUSEMODE {
        MM_PANZOOM = 0, MM_INFODIALOG
    };

public:
    UIRenderWindow(QWidget* parent);
    ~UIRenderWindow();

public:
    void showFullScreen();
    void showNormal();
    void showInfo();
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

    MOUSEMODE mMode;
    UIInfoDialog* mDialog;
};

#endif // UIRENDERWINDOW_H
