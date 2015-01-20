#include "animationframe.h"

AnimationFrame::AnimationFrame() :
    QGraphicsItem()
{
}

QRectF AnimationFrame::boundingRect() const
{
    return QRectF(0,0,0,0);
}

void AnimationFrame::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{

}
