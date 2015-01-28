#include "animation.h"

namespace
{
const int iTimePerFrame = 10; // miliseconds
static bool bIsFrameChanged = true;
};

Animation::Animation(QGraphicsScene *scene)
    : QGraphicsView(),
      m_iInterFrames(0),
      m_currentFrame(NULL),
      m_pFactory(nullptr),
      m_startEdgePosition(QPointF(0.0,0.0)),
      m_currentTool(TOOL::VERTEX),
      m_pCurrentVertex(nullptr),
      m_pCurrentEdge(nullptr)
{
    setScene(scene);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    m_pLayer = new Layer;
    m_pFactory = new ItemFactory(m_pLayer);
    scene->addItem(m_pLayer);

    AnimationFrame * pFrame = new AnimationFrame;
    m_vecFrames.push_back(pFrame);
    m_currentFrame++;

    m_pTimer = new QTimer;
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerOverflow()));
}

Animation::~Animation()
{
    delete m_pFactory;
}

void Animation::mousePressEvent(QMouseEvent *event)
{
    if(!bIsFrameChanged)
        return;

    switch(event->button())
    {
    case Qt::LeftButton:
        createItem(mapToScene(event->pos())); break;
    case Qt::RightButton:
        eraseItem(mapToScene(event->pos())); break;
    default: break;
    }
}

void Animation::createItem(const QPointF &position)
{
    m_pCurrentVertex = nullptr;
    m_pCurrentEdge   = nullptr;

    m_pCurrentVertex = intersectedVertex(position);

    if(m_currentTool == TOOL::VERTEX)
    {
        if(m_pCurrentVertex == nullptr)
            m_pCurrentVertex = m_pFactory->addEllipse(position);
    }
    else if(m_currentTool == TOOL::EDGE)
    {
        if(m_pCurrentVertex)
        {
            m_startEdgePosition = m_pCurrentVertex->getNormalPos();
            m_pCurrentEdge = m_pFactory->addLine(m_startEdgePosition);
        }
    }
}

void Animation::eraseItem(const QPointF &position)
{
    Edge   *edge = nullptr;
    QList<Edge*>edgeEraseList;
    for(auto iter : m_pLayer->childItems())
        if(edge = qgraphicsitem_cast<Edge*>(iter))
        {
            qreal qrDistance1 = QLineF(position, edge->line().p1()).length();
            qreal qrDistance2 = QLineF(position, edge->line().p2()).length();
            if(qrDistance1 < ELIPSE_R || qrDistance2 < ELIPSE_R || edge->isUnderMouse())
                edgeEraseList.push_back(edge);
        }

    Vertex *vertex;
    if(!edgeEraseList.isEmpty())
    {
        for(auto edgeIter : edgeEraseList)
        {
            for(auto vertexIter : m_pLayer->childItems())
                if(vertex = qgraphicsitem_cast<Vertex*>(vertexIter))
                    if(edgeIter->isConnected(vertex->getId()))
                        vertex->eraseLine(edgeIter);
            edgeIter->erase();
        }
    }

    vertex = intersectedVertex(position);
    if(vertex != nullptr)
    {
        for(auto iter : m_vecFrames)
            iter->erasePointById(vertex->getId());

        vertex->erase();
    }
}

Vertex *Animation::intersectedVertex(const QPointF &position)
{
    Vertex *vertex;

    for(auto vertIter : m_pLayer->childItems())
        if(vertex = qgraphicsitem_cast<Vertex*>(vertIter))
            if(vertex->isUnderMouse())
                return vertex;

    return nullptr;
}

void Animation::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF& position = mapToScene(event->pos());
    if(m_currentTool == TOOL::VERTEX && m_pCurrentVertex)
        m_pCurrentVertex->setPos(position);
    else if(m_currentTool == TOOL::EDGE && m_pCurrentEdge)
        m_pCurrentEdge->setLine(QLineF(m_startEdgePosition, position));
}

void Animation::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        if(m_currentTool == TOOL::VERTEX && m_pCurrentVertex)
        {
            int id           = m_pCurrentVertex->getId();
            QPointF position = m_pCurrentVertex->getNormalPos();
            if(m_vecFrames.at(m_currentFrame-1)->setPositionOnFrame(id, position))
                for(auto iter : m_vecFrames)
                    iter->setPositionOnFrame(id, position);
        }
        else if(m_currentTool == TOOL::EDGE && m_pCurrentEdge)
        {
            Vertex *destVertex = intersectedVertex(mapToScene(event->pos()));
            bool bConnected;
            if(destVertex)
            {
                int id1 = m_pCurrentVertex->getId();
                int id2 =destVertex->getId();
                QPointF destPoint = destVertex->getNormalPos();
                bConnected = isConnected(id1, id2);
                if(!bConnected)
                {
                    m_pCurrentEdge->setLine(QLineF(m_startEdgePosition, destPoint));
                    m_pCurrentVertex->setLineFrom(m_pCurrentEdge);
                    destVertex->setLineTo(m_pCurrentEdge);
                    m_pCurrentEdge->setConnection(id1, id2);
                    m_mapConnections.insert(id1, id2);
                    return;
                }
            }
            m_pCurrentEdge->erase();
        }

    m_pCurrentVertex = nullptr;
    m_pCurrentEdge   = nullptr;
}

void Animation::setAnimationSpeed(int time)
{
    m_iInterFrames = time / iTimePerFrame;
}

bool Animation::isConnected(const int &id1, const int &id2)
{
    QMultiHash<int, int>::iterator iter = m_mapConnections.begin();
    for(iter; iter != m_mapConnections.end(); ++iter)
    {
        int keyId = iter.key();
        int valId = iter.value();
        if(keyId == id1 && valId == id2 || keyId == id2 && valId == id1)
            return true;
    }

    return false;
}

void Animation::calculateVelocity()
{
    m_listVelocities.clear();
    Vertex *pVertex;
    AnimationFrame *pFrame = m_vecFrames.at(m_currentFrame-1);

    for(auto iter : m_pLayer->childItems())
        if(pVertex = qgraphicsitem_cast<Vertex*>(iter))
        {
            int iCurVertexId = pVertex->getId();
            QPointF startPos = pVertex->getNormalPos();
            QPointF destPos  = pFrame->getPositionOnFrame(iCurVertexId);
            QPointF velocity = QPointF(destPos.x() - startPos.x(), destPos.y() - startPos.y()) / m_iInterFrames;
            m_listVelocities.push_back(velocity);
        }
}

void Animation::loadFrames()
{
    m_currentFrame++;
    QMap<int, QPointF>mapVertices = m_vecFrames.at(m_currentFrame-1)->getPoints();
    QMap<int, QPointF>::iterator posIter = mapVertices.begin();
    for(posIter; posIter != mapVertices.end(); ++posIter)
        m_pFactory->addEllipse(posIter.value());

    posIter = mapVertices.begin();
    Vertex *start = nullptr;
    Vertex *dest = nullptr;
    Vertex *buff;
    QMultiHash<int, int>::iterator conIt = m_mapConnections.begin();
    for(conIt; conIt != m_mapConnections.end(); ++conIt)
    {
        for(auto vertIter : m_pLayer->childItems())
        {
            if(buff = qgraphicsitem_cast<Vertex*>(vertIter))
            {
                if(conIt.key() == buff->getId())
                    start = buff;
                else if(conIt.value() == buff->getId())
                    dest = buff;
                if(start && dest)
                {
                    Edge *edge = m_pFactory->addLine(QLineF(start->getNormalPos(), dest->getNormalPos()));
                    start->setLineFrom(edge);
                    dest->setLineTo(edge);
                    edge->setConnection(conIt.key(), conIt.value());
                    start = dest = nullptr;
                }
            }
        }
    }
}

void Animation::changePointsPosition()
{
    if(bIsFrameChanged || m_listVelocities.isEmpty())
    {
        m_pTimer->stop();
        return;
    }

    bIsFrameChanged = true;

    Vertex *pVertex;
    AnimationFrame *pFrame = m_vecFrames.at(m_currentFrame-1);
    qreal minDistance = 1.0;

    QList<QPointF>::iterator veloIter = m_listVelocities.begin();
    for(auto iter : m_pLayer->childItems())
        if(pVertex = qgraphicsitem_cast<Vertex*>(iter))
        {
            int iCurVertexId = pVertex->getId();
            QPointF startPos = pVertex->getNormalPos();
            QPointF destPos  = pFrame->getPositionOnFrame(iCurVertexId);
            QPointF curPos   = startPos + *veloIter;
            qreal distance = QLineF(startPos, destPos).length();
            if(distance > minDistance)
            {
                distance = QLineF(startPos, curPos).length();
                if(distance < minDistance)
                    curPos = destPos;

                pVertex->setPos(curPos);
                bIsFrameChanged = false;
            }
            veloIter++;
        }
}

int Animation::setAnimationFrame(int frameDirection)
{
    if(m_currentFrame+frameDirection == 0)
        return m_currentFrame;

    m_currentFrame+=frameDirection;

    if(m_currentFrame-1 == m_vecFrames.size())
    {
        Vertex *pVertex;
        AnimationFrame *pFrame = new AnimationFrame;

        for(auto iter : m_pLayer->childItems())
            if(pVertex = qgraphicsitem_cast<Vertex*>(iter))
                pFrame->setPositionOnFrame(pVertex->getId(), pVertex->getNormalPos());

        m_vecFrames.push_back(pFrame);
        return m_currentFrame;
    }


    calculateVelocity();
    bIsFrameChanged = false;
    m_pTimer->start(iTimePerFrame);

    return m_currentFrame;
}

int Animation::getCurrentFramesCount() const
{
    return m_vecFrames.size();
}

int Animation::getCurrentFrameIndex() const
{
    return m_currentFrame;
}


void Animation::timerOverflow()
{
    changePointsPosition();
}

void Animation::saveFile()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Animation"), "", tr("(*.2dA)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }

    QDomDocument domDoc;

    QDomElement animation = domDoc.createElement("animation");
    domDoc.appendChild(animation);

    for(auto frameIter : m_vecFrames)
    {
        QDomElement frame = domDoc.createElement("frame");
        animation.appendChild(frame);

        QDomElement verticesOnFrame = domDoc.createElement("frame_vertices");
        frame.appendChild(verticesOnFrame);

        QMap<int, QPointF>mapVertices = frameIter->getPoints();
        QMap<int, QPointF>::iterator childIter = mapVertices.begin();
        for(childIter; childIter != mapVertices.end(); ++childIter)
        {
            QDomElement pointToFile = domDoc.createElement("frame_vertex");
            pointToFile.setAttribute("id", childIter.key());
            pointToFile.setAttribute("x", childIter.value().x());
            pointToFile.setAttribute("y", childIter.value().y());
            verticesOnFrame.appendChild(pointToFile);
        }
    }

    QDomElement connections = domDoc.createElement("vertex_connections");
    animation.appendChild(connections);

    QMultiHash<int, int>::iterator iter = m_mapConnections.begin();
    for(iter; iter != m_mapConnections.end(); ++iter)
    {
        QDomElement connection = domDoc.createElement("connection");
        connection.setAttribute("id1", iter.key());
        connection.setAttribute("id2", iter.value());
        connections.appendChild(connection);
    }
    QTextStream TextStream(&file);
    TextStream << domDoc.toString();
    file.close();
}

void Animation::loadFile()
{
    resetAnimation();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Animation"), "",tr("(*.2dA)"));
    if (fileName.isEmpty())
        return;


    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }


    QDomDocument domDoc;
    if (!domDoc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomElement animation = domDoc.documentElement();

    QDomNode dataType = animation.firstChild();

    while(!dataType.isNull())
    {
        if(dataType.toElement().tagName() == "frame")
        {
            QDomNode frameVertices = dataType.firstChild();
            if(!frameVertices.isNull())
            {
                AnimationFrame * pFrame = new AnimationFrame;
                m_vecFrames.push_back(pFrame);
                QDomNode vertices = frameVertices.firstChild();
                while(!vertices.isNull())
                {
                    QDomElement vertex = vertices.toElement();
                    int id = vertex.attribute("id").toInt();
                    QPointF pos(vertex.attribute("x").toDouble(), vertex.attribute("y").toDouble());
                    pFrame->setPositionOnFrame(id, pos);
                    vertices = vertices.nextSibling();
                }
            }
        }
        dataType = dataType.nextSibling();
        if(dataType.toElement().tagName() == "vertex_connections")
        {
            QDomNode connections = dataType.firstChild();
            while(!connections.isNull())
            {
                QDomElement connect = connections.toElement();
                int id1 = connect.attribute("id1").toInt();
                int id2 = connect.attribute("id2").toInt();
                m_mapConnections.insert(id1,id2);
                connections = connections.nextSibling();
            }
            dataType = dataType.nextSibling();
        }
    }
    loadFrames();
}

void Animation::resetAnimation()
{
    for(auto framesIter : m_vecFrames)
        framesIter->clearFrame();

    for(auto childIter : m_pLayer->childItems())
    {
        if(qgraphicsitem_cast<Vertex*>(childIter))
            qgraphicsitem_cast<Vertex*>(childIter)->erase();
        else if(qgraphicsitem_cast<Edge*>(childIter))
            qgraphicsitem_cast<Edge*>(childIter)->erase();
    }

    m_vecFrames.clear();
    m_mapConnections.clear();
    m_listVelocities.clear();
    scene()->clear();
    m_currentFrame = NULL;
    m_startEdgePosition = QPointF(0.0,0.0);
    m_currentTool = TOOL::VERTEX;
    m_pCurrentVertex = nullptr;
    m_pCurrentEdge = nullptr;

    m_pLayer = new Layer;
    m_pFactory = new ItemFactory(m_pLayer);
    scene()->addItem(m_pLayer);
}
