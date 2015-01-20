#include "animation.h"

Animation::Animation(QGraphicsScene *scene)
    : QGraphicsView(),
      m_currentFrame(NULL),
      m_bAutoCopy(false),
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

    AnimationFrame *frame = new AnimationFrame;
    scene->addItem(frame);
    m_vecFrames.push_back(frame);
    m_currentFrame++;

    m_pFactory = new ItemFactory(frame);    
}

Animation::~Animation()
{
    delete m_pFactory;
}

void Animation::mousePressEvent(QMouseEvent *event)
{
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
        else
            makeEdgesMap(position);
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
    Vertex *vertex = intersectedVertex(position);


    Edge   *edge;
    QList<Edge*>edgeEraseList;
    for(auto iter : m_vecFrames.at(m_currentFrame-1)->childItems())
    {
        if(edge = qgraphicsitem_cast<Edge*>(iter))
        {
            qreal qrDistance1 = QLineF(position, edge->line().p1()).length();
            qreal qrDistance2 = QLineF(position, edge->line().p2()).length();
            if(qrDistance1 < ELIPSE_R || qrDistance2 < ELIPSE_R || edge->isUnderMouse())
                edgeEraseList.push_back(edge);
        }
    }

    if(vertex)
        vertex->erase();
    if(!edgeEraseList.isEmpty())
        for(auto iter : edgeEraseList)
            iter->erase();
}

Vertex *Animation::intersectedVertex(const QPointF &position)
{
    Vertex *vertex;

    for(auto vertIter : m_vecFrames.at(m_currentFrame-1)->childItems())
        if(vertex = qgraphicsitem_cast<Vertex*>(vertIter))
            if(vertex->isUnderMouse())
                return vertex;

    return nullptr;
}

void Animation::makeEdgesMap(const QPointF &position)
{
    m_mapEgesToMove.clear();
    Edge *edge;
    QPointF destPoint;

    for(auto iter : m_vecFrames.at(m_currentFrame-1)->childItems())
    {
        if(edge = qgraphicsitem_cast<Edge*>(iter))
        {
            qreal qrDistance1 = QLineF(position, edge->line().p1()).length();
            qreal qrDistance2 = QLineF(position, edge->line().p2()).length();
            if(qrDistance1 < ELIPSE_R)
            {
                destPoint = edge->line().p2();
                m_mapEgesToMove.insert(edge, destPoint);
            }
            else if(qrDistance2 < ELIPSE_R)
            {
                destPoint = edge->line().p1();
                m_mapEgesToMove.insert(edge, destPoint);
            }
        }
    }
}

void Animation::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF& position = mapToScene(event->pos());
    if(m_currentTool == TOOL::VERTEX && m_pCurrentVertex)
    {
        m_pCurrentVertex->setPos(position);

        if(!m_mapEgesToMove.isEmpty())
        {
            QMultiHash<Edge*, QPointF>::iterator iter = m_mapEgesToMove.begin();
            for(iter; iter != m_mapEgesToMove.end(); ++iter)
                iter.key()->setLine(QLineF(iter.value(), position));
        }
    }
    else if(m_currentTool == TOOL::EDGE && m_pCurrentEdge)
        m_pCurrentEdge->setLine(QLineF(m_startEdgePosition, position));
}

void Animation::mouseReleaseEvent(QMouseEvent *event)
{
    m_mapEgesToMove.clear();

    if(m_currentTool == TOOL::EDGE && m_pCurrentEdge)
    {
        Vertex *destVertex = intersectedVertex(mapToScene(event->pos()));
        bool bConnected;
        if(destVertex)
        {
            QPointF destPoint = destVertex->getNormalPos();
            bConnected = isConnected(m_startEdgePosition, destPoint);
            if(!bConnected)
            {
                m_pCurrentEdge->setLine(QLineF(m_startEdgePosition, destPoint));
                return;
            }
        }
        m_pCurrentEdge->erase();
    }
}

bool Animation::isConnected(const QPointF &p1, const QPointF &p2)
{
    Edge *eLine;
    for(auto iter : m_vecFrames.at(m_currentFrame-1)->childItems())
    {
        if(eLine = qgraphicsitem_cast<Edge*>(iter))
        {
            if(eLine == m_pCurrentEdge)
                continue;

            qreal qrDistance1 = QLineF(p1, eLine->line().p1()).length();
            qreal qrDistance2 = QLineF(p1, eLine->line().p2()).length();
            if(qrDistance1 < ELIPSE_R || qrDistance2 < ELIPSE_R)
            {
                qrDistance1 = QLineF(p2, eLine->line().p1()).length();
                qrDistance2 = QLineF(p2, eLine->line().p2()).length();
                if(qrDistance1 < ELIPSE_R || qrDistance2 < ELIPSE_R)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int Animation::setAnimationFrame(int frameDirection)
{
    if(m_currentFrame+frameDirection == 0)
        return m_currentFrame;

    m_vecFrames.at(m_currentFrame-1)->hide();

    m_currentFrame+=frameDirection;

    bool copyPrevious = false;
    if(m_currentFrame-1 == m_vecFrames.size())
    {
        AnimationFrame *newFrame = new AnimationFrame;
        scene()->addItem(newFrame);
        m_vecFrames.push_back(newFrame);
        copyPrevious = m_bAutoCopy;
    }

    m_vecFrames.at(m_currentFrame-1)->show();
    m_pFactory->setParentFrame(m_vecFrames.at(m_currentFrame-1));

    if(copyPrevious)
        copyPreviousFrame();

    return m_currentFrame;
}

void Animation::clearFrame()
{
    if(m_vecFrames.at(m_currentFrame-1)->childItems().isEmpty())
    {
        return;
    }

    for(auto childIter : m_vecFrames.at(m_currentFrame-1)->childItems())
    {
        if(qgraphicsitem_cast<Vertex*>(childIter))
            qgraphicsitem_cast<Vertex*>(childIter)->erase();
        else if(qgraphicsitem_cast<Edge*>(childIter))
            qgraphicsitem_cast<Edge*>(childIter)->erase();
    }
}

void Animation::copyPreviousFrame()
{
    if(m_currentFrame < 2)
    {
        return;
    }

    clearFrame();

    Vertex *vertex;
    Edge   *edge;

    for(auto iter : m_vecFrames.at(m_currentFrame-2)->childItems())
        if(vertex = qgraphicsitem_cast<Vertex*>(iter))
            m_pFactory->addEllipse(vertex->getNormalPos());
        else if(edge = qgraphicsitem_cast<Edge*>(iter))
            m_pFactory->addLine(edge->line());
}

int Animation::getCurrentFramesCount() const
{
    return m_vecFrames.size();
}

int Animation::getCurrentFrameIndex() const
{
    return m_currentFrame;
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

        QDomElement verticesOnFrame = domDoc.createElement("vertices");
        frame.appendChild(verticesOnFrame);
        QDomElement edgesOnFrame = domDoc.createElement("edges");
        frame.appendChild(edgesOnFrame);

        for(auto childIter : frameIter->childItems())
        {
            Vertex *vertex;
            if(vertex = qgraphicsitem_cast<Vertex*>(childIter))
            {
                QDomElement vertexToFile = domDoc.createElement("vertex");
                vertexToFile.setAttribute("x", vertex->getNormalPos().x());
                vertexToFile.setAttribute("y", vertex->getNormalPos().y());
                verticesOnFrame.appendChild(vertexToFile);
            }
            Edge *edge;
            if(edge = qgraphicsitem_cast<Edge*>(childIter))
            {
                QDomElement edgeToFile = domDoc.createElement("edge");
                edgeToFile.setAttribute("vertex1x", edge->line().p1().x());
                edgeToFile.setAttribute("vertex1y", edge->line().p1().y());
                edgeToFile.setAttribute("vertex2x", edge->line().p2().x());
                edgeToFile.setAttribute("vertex2y", edge->line().p2().y());
                edgesOnFrame.appendChild(edgeToFile);
            }
        }
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

    QDomElement docElem = domDoc.documentElement();

    QDomNode framesOnFile = docElem.firstChild();

    while(!framesOnFile.isNull())
    {
        AnimationFrame *frame = new AnimationFrame;
        scene()->addItem(frame);
        m_vecFrames.push_back(frame);
        m_vecFrames.back()->hide();
        m_pFactory->setParentFrame(frame);

        QDomNode elementType = framesOnFile.firstChild();
        if(!elementType.isNull())
        {
            if(elementType.toElement().tagName() == "vertices")
            {
                QDomNode vertices = elementType.firstChild();
                while(!vertices.isNull())
                {
                    QDomElement vertex = vertices.toElement();
                    QPointF pos(vertex.attribute("x").toDouble(), vertex.attribute("y").toDouble());
                    m_pFactory->addEllipse(pos);
                    vertices = vertices.nextSibling();
                }
            }
            elementType = elementType.nextSibling();
            if(elementType.toElement().tagName() == "edges")
            {
                QDomNode edges = elementType.firstChild();
                while(!edges.isNull())
                {
                    QDomElement edge = edges.toElement();
                    auto c = edge.tagName();
                    QPointF pos1(edge.attribute("vertex1x").toDouble(), edge.attribute("vertex1y").toDouble());
                    QPointF pos2(edge.attribute("vertex2x").toDouble(), edge.attribute("vertex2y").toDouble());
                    m_pFactory->addLine(QLineF(pos1, pos2));
                    edges = edges.nextSibling();
                }
            }
            elementType = elementType.nextSibling();
        }
        framesOnFile = framesOnFile.nextSibling();
    }

    m_vecFrames.at(m_currentFrame)->show();
    m_currentFrame++;
}

void Animation::resetAnimation()
{
    for(auto framesIter : m_vecFrames)
        for(auto childIter : framesIter->childItems())
        {
            if(qgraphicsitem_cast<Vertex*>(childIter))
                qgraphicsitem_cast<Vertex*>(childIter)->erase();
            else if(qgraphicsitem_cast<Edge*>(childIter))
                qgraphicsitem_cast<Edge*>(childIter)->erase();
        }

    m_vecFrames.clear();
    scene()->clear();
    m_currentFrame = NULL;
    m_bAutoCopy = false;
    m_startEdgePosition = QPointF(0.0,0.0);
    m_currentTool = TOOL::VERTEX;
    m_pCurrentVertex = nullptr;
    m_pCurrentEdge = nullptr;
}
