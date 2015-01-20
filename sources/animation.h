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
#include "itemsfactory.h"



class Animation : public QGraphicsView
{
public:
     Animation(QGraphicsScene *scene = nullptr);
    ~Animation();

    //mouse events
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    //animation properties
    int  setAnimationFrame(int frameDirection);
    inline void setAutoCopy(bool copy){m_bAutoCopy = copy;}
    inline void setToolType(TOOL type){m_currentTool = type;}
    void clearFrame();
    void copyPreviousFrame();

    //check the tool type and create or get the item to move
    void createItem(const QPointF &position);

    //returns an intersected vertecs from layer
    Vertex *intersectedVertex(const QPointF &position);

    //groups all intersected edges to a map that is used on "mouseMoveEvent"
    void    makeEdgesMap(const QPointF &position);

    //erase all items intersected with position
    void eraseItem(const QPointF &position);

    //check if the line wasn't created before
    bool isConnected(const QPointF &p1, const QPointF &p2);

    int getCurrentFramesCount() const;
    int getCurrentFrameIndex() const;

    //save & load
    void saveFile();
    void loadFile();

    //reset the animation to it's default state
    void resetAnimation();

private:
    QVector<AnimationFrame*>    m_vecFrames;
    int                         m_currentFrame;

    bool                        m_bAutoCopy;
    bool                        m_bIsPlaying;

    ItemFactory                *m_pFactory;
    QPointF                     m_startEdgePosition;
    TOOL                        m_currentTool;

    Vertex                     *m_pCurrentVertex;
    Edge                       *m_pCurrentEdge;
    QMultiHash<Edge*, QPointF>  m_mapEgesToMove;
};

#endif // ANIMATION_H
