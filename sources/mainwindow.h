#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsItem>

#include "animation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void keyPressEvent(QKeyEvent *event);
    void createButtons(QSize buttonSize);
private slots:
    //button events
    void pointToolSelected();
    void lineToolSelected();
    void playAnimation();
    void stopAnimation();
    void timerOverflow();
    void setAutoCopy();
    void copyFrame();
    void clearFrame();
    void save();
    void load();
private:
    Ui::MainWindow *ui;

    QGraphicsScene *m_pScene;
    QTimer         *m_pTimer;

    QMenu          *m_pMenu;
    QAction        *m_pSaveAction;
    QAction        *m_pLoadAction;

    QToolBar       *m_pToolBar;
    QStatusBar     *m_pStatusBar;
    QLabel         *m_frameLabel;

    QPushButton    *m_pPointButton;
    QPushButton    *m_pLineButton;

    QPushButton    *m_pPlayButton;
    QPushButton    *m_pStopButton;

    QPushButton    *m_pClearFrame;
    QPushButton    *m_pCopyFrame;
    QPushButton    *m_pAutoCopyFrame;

    Animation      *m_pAnimation;

    bool            m_autoCopy;
};

#endif // MAINWINDOW_H
