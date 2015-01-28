#include "layer.h"

Layer::Layer() :
    QGraphicsItem()
{
}

QRectF Layer::boundingRect() const
{
    return QRectF(0,0,0,0);
}

void Layer::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{

}
