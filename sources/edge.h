#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>

class Edge : public QGraphicsLineItem
{
public:
    Edge(const QLineF &lineObj, QGraphicsItem *parent = 0);

    //detach from parent & remove from scene
    void erase();
private:
};

#endif // EDGE_H
