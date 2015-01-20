#include "itemsfactory.h"
#include <QGraphicsScene>

ItemFactory::ItemFactory(AnimationFrame *parentFrame) :
    m_pParentFrame(parentFrame),
    m_startEdgePoint(QPointF(-1.0, -1.0))
{
}

ItemFactory::~ItemFactory()
{
}

Vertex * ItemFactory::addEllipse(const QPointF &position)
{
    Vertex *vertItem = new Vertex(0,0, ELIPSE_D, ELIPSE_D, m_pParentFrame);
    vertItem->setPos(position);
    return vertItem;
}

Edge* ItemFactory::addLine(const QPointF &position)
{
    const QLineF& line = QLineF(position, position);
    Edge *edgeItem = new Edge(line, m_pParentFrame);
    return edgeItem;
}

Edge *ItemFactory::addLine(const QLineF &line)
{
    Edge *edgeItem = new Edge(line, m_pParentFrame);
    return edgeItem;
}
