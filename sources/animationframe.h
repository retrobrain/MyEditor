#ifndef ANIMATIONFRAME_H
#define ANIMATIONFRAME_H

#include <QMap>
#include <QPointF>

class AnimationFrame
{
public:
    AnimationFrame();
    ~AnimationFrame();
    bool setPositionOnFrame(int id, QPointF position);
    QPointF getPositionOnFrame(int id);
    void erasePointById(int id);
    QMap<int, QPointF>getPoints(){return m_mapPoints;}
    void clearFrame(){m_mapPoints.clear();}
private:
    QMap<int, QPointF>m_mapPoints;
};

#endif // ANIMATIONFRAME_H
