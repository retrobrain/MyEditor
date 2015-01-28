#include "vertex.h"

int Vertex::ID = 0;

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

    m_iID = ID++;
}

void Vertex::erase()
{
    m_vecLinesToThis.clear();
    m_vecLinesFromThis.clear();
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

void Vertex::setLineTo(Edge *line)
{
    m_vecLinesToThis.push_back(line);
}

void Vertex::eraseLine(Edge *line)
{
    if(!m_vecLinesFromThis.removeOne(line))
        m_vecLinesToThis.removeOne(line);
}

void Vertex::setLineFrom(Edge *line)
{
    m_vecLinesFromThis.push_back(line);
}

QVariant Vertex::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    for(auto iter : m_vecLinesFromThis)
        iter->setLine(QLineF(getNormalPos(), iter->line().p2()));
    for(auto iter : m_vecLinesToThis)
        iter->setLine(QLineF(iter->line().p1(), getNormalPos()));

    return QGraphicsEllipseItem::itemChange(change, value);
}

QPointF Vertex::getNormalPos()
{
    QPointF anchoredPos;
    anchoredPos.setX(pos().x() + ELIPSE_R);
    anchoredPos.setY(pos().y() + ELIPSE_R);
    return anchoredPos;
}
