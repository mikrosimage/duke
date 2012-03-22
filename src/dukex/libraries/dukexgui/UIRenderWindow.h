#ifndef UIRENDERWINDOW_H
#define UIRENDERWINDOW_H

#include <QtGui>

class UIRenderWindow : public QWidget {

    Q_OBJECT

public:
    UIRenderWindow(QWidget* parent);
    ~UIRenderWindow();

public:
    void showFullScreen();
    void showNormal();
    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);

public:
    void* renderWindowID() const;
    QWidget* renderWidget() const;

signals:
    void zoomChanged(double);
    void panChanged(double, double);

private:
    QWidget* mRenderWidget;
    double mZoomRatio;
    QPoint mDragPosition;
};

#endif // UIRENDERWINDOW_H
