#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace
{
    const int iTimePerFrame = 500; // miliseconds
    static bool bIsPlayAnim = false;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_autoCopy(false)
{
    ui->setupUi(this);
    QSize minScreenSize = QDesktopWidget().availableGeometry().size() * 0.5;
    setMinimumSize(minScreenSize);
    showMaximized();
    setWindowTitle(tr("2D Animation Editor"));

    QGraphicsScene *pScene = new QGraphicsScene(rect());

    //========DrawLayer==========4
    m_pAnimation = new Animation(pScene);
    setCentralWidget(m_pAnimation);

    //========ToolBar===========
    qreal rButtonSide = QDesktopWidget().availableGeometry().size().height() / 15;
    QSize sButtonSize = QSize(rButtonSide, rButtonSide);
    createButtons(sButtonSize);

    //========MainMenu==========
    m_pMenu = ui->menuBar->addMenu(tr("File"));
    m_pSaveAction = new QAction(tr("Save As"), this);
    connect(m_pSaveAction, SIGNAL(triggered()), this, SLOT(save()));
    m_pMenu->addAction(m_pSaveAction);
    m_pLoadAction = new QAction(tr("Load"), this);
    connect(m_pLoadAction, SIGNAL(triggered()), this, SLOT(load()));
    m_pMenu->addAction(m_pLoadAction);

    //========StatusBar==========
    m_frameLabel = new QLabel(QString::number(1));
    QFont("Arial", 10, QFont::Bold);
    m_frameLabel->setFont(QFont("Arial", sButtonSize.height()*0.3));
    ui->statusBar->addWidget(m_frameLabel);
    pointToolSelected();

    //==Timer==
    m_pTimer = new QTimer(parent);
    QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerOverflow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int iDirection = 0;
    switch (event->key())
    {
    case Qt::Key_A: --iDirection; break;
    case Qt::Key_D: ++iDirection; break;
    }

    int iCurFrame = m_pAnimation->setAnimationFrame(iDirection);
    m_frameLabel->setText(QString::number(iCurFrame));
}

void MainWindow::createButtons(QSize buttonSize)
{
    m_pPointButton = new QPushButton(tr("Point"));
    m_pPointButton->setFixedHeight(buttonSize.height());
    m_pPointButton->setCheckable(true);
    connect(m_pPointButton, SIGNAL(clicked()), this, SLOT(pointToolSelected()));
    m_pLineButton = new QPushButton(tr("Line"));
    m_pLineButton->setFixedHeight(buttonSize.height());
    m_pLineButton->setCheckable(true);
    connect(m_pLineButton, SIGNAL(clicked()), this, SLOT(lineToolSelected()));
    m_pPlayButton = new QPushButton(tr("Play"));
    m_pPlayButton->setFixedHeight(buttonSize.height());
    m_pPlayButton->setCheckable(false);
    connect(m_pPlayButton, SIGNAL(clicked()), this, SLOT(playAnimation()));
    m_pStopButton = new QPushButton(tr("Stop"));
    m_pStopButton->setFixedHeight(buttonSize.height());
    m_pStopButton->setCheckable(false);
    connect(m_pStopButton, SIGNAL(clicked()), this, SLOT(stopAnimation()));
    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);
    m_pClearFrame = new QPushButton(tr("Clear frame"));
    m_pClearFrame->setFixedHeight(buttonSize.height());
    connect(m_pClearFrame, SIGNAL(clicked()), this, SLOT(clearFrame()));
    m_pCopyFrame = new QPushButton(tr("Copy previous"));
    m_pCopyFrame->setFixedHeight(buttonSize.height());
    connect(m_pCopyFrame, SIGNAL(clicked()), this, SLOT(copyFrame()));
    m_pAutoCopyFrame = new QPushButton(tr("AutoCopy"));
    m_pAutoCopyFrame->setFixedHeight(buttonSize.height());
    m_pAutoCopyFrame->setCheckable(true);
    connect(m_pAutoCopyFrame, SIGNAL(clicked()), this, SLOT(setAutoCopy()));
    ui->mainToolBar->addWidget(m_pPointButton);
    ui->mainToolBar->addWidget(m_pLineButton);
    ui->mainToolBar->addWidget(m_pPlayButton);
    ui->mainToolBar->addWidget(m_pStopButton);
    ui->mainToolBar->addWidget(spacerWidget);
    ui->mainToolBar->addWidget(m_pClearFrame);
    ui->mainToolBar->addWidget(m_pCopyFrame);
    ui->mainToolBar->addWidget(m_pAutoCopyFrame);
}

void MainWindow::pointToolSelected()
{
    m_pAnimation->setToolType(TOOL::VERTEX);
    m_pPointButton->setChecked(true);
    m_pLineButton->setChecked(false);
}

void MainWindow::lineToolSelected()
{
    m_pAnimation->setToolType(TOOL::EDGE);
    m_pLineButton->setChecked(true);
    m_pPointButton->setChecked(false);
}

void MainWindow::playAnimation()
{
    if (bIsPlayAnim)
        return;

    int newFrameId = m_pAnimation->setAnimationFrame(1 - m_pAnimation->getCurrentFrameIndex());
    m_frameLabel->setText(QString::number(newFrameId));

    m_pTimer->start(iTimePerFrame);
    bIsPlayAnim = true;

    m_pPointButton->setEnabled(!bIsPlayAnim);
    m_pLineButton->setEnabled(!bIsPlayAnim);
    m_pPlayButton->setEnabled(!bIsPlayAnim);

    m_pClearFrame->setEnabled(!bIsPlayAnim);
    m_pCopyFrame->setEnabled(!bIsPlayAnim);
    m_pAutoCopyFrame->setEnabled(!bIsPlayAnim);
}

void MainWindow::stopAnimation()
{
    if (!bIsPlayAnim)
        return;

    bIsPlayAnim = false;
    m_pTimer->stop();

    int newFrameId = m_pAnimation->setAnimationFrame(1 - m_pAnimation->getCurrentFrameIndex());
    m_frameLabel->setText(QString::number(newFrameId));

    m_pPointButton->setEnabled(!bIsPlayAnim);
    m_pLineButton->setEnabled(!bIsPlayAnim);
    m_pPlayButton->setEnabled(!bIsPlayAnim);

    m_pClearFrame->setEnabled(!bIsPlayAnim);
    m_pCopyFrame->setEnabled(!bIsPlayAnim);
    m_pAutoCopyFrame->setEnabled(!bIsPlayAnim);
}

void MainWindow::timerOverflow()
{
    if (m_pAnimation->getCurrentFramesCount() == m_pAnimation->getCurrentFrameIndex())
    {
        stopAnimation();
        return;
    }

    int newFrameId = m_pAnimation->setAnimationFrame(1);
    m_frameLabel->setText(QString::number(newFrameId));
}

void MainWindow::setAutoCopy()
{
    m_autoCopy = !m_autoCopy;
    m_pAnimation->setAutoCopy(m_autoCopy);
}

void MainWindow::copyFrame()
{
    m_pAnimation->copyPreviousFrame();
}

void MainWindow::clearFrame()
{
    m_pAnimation->clearFrame();
}

void MainWindow::save()
{
    m_pAnimation->saveFile();
}

void MainWindow::load()
{
    m_pAnimation->loadFile();
    m_frameLabel->setText(QString::number(1));
}
