#include "animationframe.h"

AnimationFrame::AnimationFrame()
{

}

AnimationFrame::~AnimationFrame()
{

}

bool AnimationFrame::setPositionOnFrame(int id, QPointF position)
{
    bool bWasInserted = !m_mapPoints.contains(id);
    m_mapPoints.insert(id, position);
    return bWasInserted;
}

QPointF AnimationFrame::getPositionOnFrame(int id)
{
    return m_mapPoints.find(id).value();
}

void AnimationFrame::erasePointById(int id)
{
    m_mapPoints.remove(id);
}

