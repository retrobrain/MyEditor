#ifndef ANIMATION_H
#define ANIMATION_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml/QtXml>
#include <QtXml/QDomDocument>

#include "animationframe.h"
#include "layer.h"
#include "itemsfactory.h"



class Animation : public QGraphicsView
{
    Q_OBJECT

public:
     Animation(QGraphicsScene *scene = nullptr);
    ~Animation();

    //mouse events
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    //animation properties
    void setAnimationSpeed(int time);
    int  setAnimationFrame(int frameDirection);
    inline void setToolType(TOOL type){m_currentTool = type;}



    int getCurrentFramesCount() const;
    int getCurrentFrameIndex() const;

    //save to file
    void saveFile();

    //load to frames and connections map. still need to add to the scene
    void loadFile();

    //reset the animation to it's default state
    void resetAnimation();
private slots:
    void timerOverflow();
private:
    //check the tool type and create or get the item to move
    void createItem(const QPointF &position);

    //returns an intersected vertecs from layer
    Vertex *intersectedVertex(const QPointF &position);

    //called by timer to change position
    void changePointsPosition();

    //erase all items intersected with position
    void eraseItem(const QPointF &position);

    //check if the line wasn't created before
    bool isConnected(const int &id1, const int &id2);

    //calculates velocity applied on position when the frame is changed
    void calculateVelocity();

    //add to the scene loaded objects
    void loadFrames();

    int                         m_iInterFrames;

    Layer                      *m_pLayer;
    int                         m_currentFrame;

    ItemFactory                *m_pFactory;
    QPointF                     m_startEdgePosition;
    TOOL                        m_currentTool;

    Vertex                     *m_pCurrentVertex;
    Edge                       *m_pCurrentEdge;
    QVector<AnimationFrame*>    m_vecFrames;
    QTimer                     *m_pTimer;
    QMultiHash<int, int>        m_mapConnections;
    QList<QPointF>             m_listVelocities;
};

#endif // ANIMATION_H
