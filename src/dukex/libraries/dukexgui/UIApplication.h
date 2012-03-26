#ifndef UIAPPLICATION_H
#define UIAPPLICATION_H

#include "IUIBuilder.h"
#include "ui_mainwindow.h"
#include "UserPreferences.h"
#include <dukexcore/dkxNodeManager.h>
#include <dukexcore/dkxSession.h>
#include <QtGui>

class UIRenderWindow;
class UIFileDialog;
class UIPluginDialog;

class UIApplication : public QMainWindow, public IUIBuilder {

Q_OBJECT

public:
    UIApplication(Session::ptr s);
    ~UIApplication() {
    }

public:
    // IUIBuilder Interface
    void addObserver(QObject* _plugin, IObserver* _observer);
    QAction* createAction(QObject* _plugin, const QString & _parentMenuName);
    QMenu* createMenu(QObject* _plugin, const QString & _parentMenuName);
    QDockWidget* createWindow(QObject* _plugin, Qt::DockWidgetArea _area, bool floating);
    void closeUI(QObject* _plug);

private:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeCentralWidget(const QSize& resolution);
    void updateRecentFilesMenu();

private slots:
    // file
    void openFiles(const QStringList &, const bool &, const bool &);
    void openFiles();
    void openRecent();
    void browseDirectory();
    // control
    void playStop();
    void previousFrame();
    void nextFrame();
    void firstFrame();
    void lastFrame();
    void previousShot();
    void nextShot();
    // display
    void info();
    // window
    void fullscreen();
    void toggleFitMode();
    void fitToNormalSize();
    void fitImageToWindowWidth();
    void fitImageToWindowHeight();
    void zoom(double);
    void pan(double, double);
    // ?
    void about();
    void aboutPlugins();

    void topLevelChanged(bool); // TEST

private:
    Ui::mainWindow ui;
//    QDeclarativeEngine m_Engine;
    NodeManager m_Manager;
    Session::ptr m_Session;
    UserPreferences m_Preferences;
    UIRenderWindow* m_RenderWindow;
    UIFileDialog* m_FileDialog;
    UIPluginDialog* m_PluginDialog;
    int m_timerID;
};

#endif // UIAPPLICATION_H
