#ifndef ANIMATION_FRAME_H
#define ANIMATION_FRAME_H

#include <QGraphicsItem>
#include <QGraphicsScene>

class Layer : public QGraphicsItem
{
public:
    Layer();

    // QGraphicsItem interface
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
};

#endif // ANIMATION_FRAME_H
