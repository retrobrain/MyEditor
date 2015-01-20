#include "vertex.h"

Vertex::Vertex(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsEllipseItem(x,y,w,h,parent)
{
    QBrush bBrush = QBrush(Qt::gray);
    bBrush.setColor(QColor(150, 150, 150, 100));
    setBrush(bBrush);
    QPen pPen = QPen(Qt::red);
    pPen.setWidth(2);
    setPen(pPen);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsScenePositionChanges);
    setZValue(1);
}

void Vertex::erase()
{
    QGraphicsScene *scene = parentItem()->scene();
    setParentItem(NULL);
    scene->removeItem(this);

}

void Vertex::setPos(const QPointF &pos)
{
    QPointF anchoredPos;
    anchoredPos.setX(pos.x() - ELIPSE_R);
    anchoredPos.setY(pos.y() - ELIPSE_R);
    QGraphicsEllipseItem::setPos(anchoredPos);
}

QPointF Vertex::getNormalPos()
{
    QPointF anchoredPos;
    anchoredPos.setX(pos().x() + ELIPSE_R);
    anchoredPos.setY(pos().y() + ELIPSE_R);
    return anchoredPos;
}
