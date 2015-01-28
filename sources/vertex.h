#ifndef VERTEX_H
#define VERTEX_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include "edge.h"

const int ELIPSE_R = 10;
const int ELIPSE_D = 20;

class Vertex : public QGraphicsEllipseItem
{
public:
    Vertex(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    //detach from parent & remove from scene
    void erase();

    //set position considering the anchor point
    void setPos(const QPointF &pos);

    //used when "this" is the start point
    void setLineFrom(Edge* line);
    //used when "this" is the end point
    void setLineTo(Edge* line);

    void eraseLine(Edge* line);

    inline void setId(int id){m_iID = id;}
    inline int getId(){return m_iID;}

    //used to move attached lines
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    //return normalized position
    QPointF getNormalPos();
private:
    static int ID;
    int        m_iID;
    QVector<Edge*>m_vecLinesFromThis;
    QVector<Edge*>m_vecLinesToThis;
};

#endif // VERTEX_H
