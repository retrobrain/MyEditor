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

    //store the id's of the points connected to this edge
    void setConnection(int p1, int p2){m_aPoints[0] = p1; m_aPoints[1] = p2;}

    //check if the line is connected
    bool isConnected(int id){return (m_aPoints[0] == id && m_aPoints[1] == id) ? true : false;}

    int getIdP1(){return m_aPoints[0];}
    int getIdP2(){return m_aPoints[1];}
private:
    int m_aPoints[2];
};

#endif // EDGE_H
