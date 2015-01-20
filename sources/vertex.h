#ifndef VERTEX_H
#define VERTEX_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

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

    //return normalized position
    QPointF getNormalPos();
private:
};

#endif // VERTEX_H
