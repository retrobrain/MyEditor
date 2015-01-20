#ifndef ITEMS_FACTORY_H
#define ITEMS_FACTORY_H

#include <QPen>
#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>

#include "vertex.h"
#include "edge.h"
#include "animationframe.h"

enum TOOL{VERTEX = 0, EDGE};
const int TOOLS_NUM = 2;

class ItemFactory
{
public:
    ItemFactory(AnimationFrame* parentFrame);
    ~ItemFactory();

    Vertex* addEllipse(const QPointF &position);
    Edge* addLine(const QPointF &position);
    Edge* addLine(const QLineF &line);
    inline void setParentFrame(AnimationFrame* parentFrame){m_pParentFrame=parentFrame;}
private:
    ItemFactory(){}

    AnimationFrame *m_pParentFrame;
    QPointF         m_startEdgePoint;
};

#endif // ITEMS_FACTORY_H
