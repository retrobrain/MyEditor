#include "edge.h"


Edge::Edge(const QLineF &lineObj, QGraphicsItem *parent) :
    QGraphicsLineItem(lineObj, parent)
{
    QPen pen = QPen(Qt::gray);
    pen.setCapStyle(Qt::PenCapStyle::RoundCap);
    pen.setWidth(5);
    setPen(pen);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsScenePositionChanges);
}

void Edge::erase()
{
    QGraphicsScene *scene = parentItem()->scene();
    setParentItem(NULL);
    scene->removeItem(this);
}
