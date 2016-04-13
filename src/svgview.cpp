/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#include <QFile>
#include <QGraphicsSvgItem>
#include <QtXml>
#include <QtXmlPatterns>
#include <QXmlStreamReader>
#include <QtDebug>
#include "svgview.h"

SvgView::SvgView(QWidget *parent)
    : QGraphicsView(parent),
    mGraphicItem(0)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    mSvgRenderer = new QSvgRenderer();

    // Prepare background check-board pattern
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);
}

void SvgView::convert(const QString &xsltFile)
{
    (void)xsltFile;
    QXmlQuery query(QXmlQuery::XSLT20);
    QString   qOut;

    query.setFocus(QUrl(mFilename));
    query.setQuery(QUrl(xsltFile));
    query.evaluateTo(&qOut);

    qInfo() << qOut;
}

QString SvgView::getTplHeader(void)
{
    QString str;
    QTextStream stream(&str);
    mTplHeader.save(stream, QDomNode::EncodingFromTextStream);
    return str;
}

QString SvgView::getTplTask(void)
{
    QString str;
    QTextStream stream(&str);
    mTplTask.save(stream, QDomNode::EncodingFromTextStream);
    return str;
}

void SvgView::loadPlan(vlePlan &plan)
{
    (void)plan;

    qInfo() << "SvgView::loadPlan";

    if ( mTplHeader.isNull() )
    {
        // ToDo : improve error handling
        qWarning() << "SvgView::loadPlan() Template error";
        return;
    }

    // Compute the height of a group
    int groupHeight;
    if (mTplHeader.hasAttribute("height"))
        groupHeight = mTplHeader.attribute("height").toDouble();
    else
        groupHeight = 100;

    // Compute size of the whole plan
    int planHeight = groupHeight * plan.countGroups();
    int planWidth  = 1000;

    // Create SVG document
    QDomDocument planSVG("xml");
    // Create root element
    QDomElement e = planSVG.createElement("svg");
    e.setAttribute("width",   QString(planWidth));
    e.setAttribute("height",  QString(planHeight));
    e.setAttribute("viewBox", QString("0 0 %1 %2").arg(planWidth).arg(planHeight));
    e.setAttribute("version", "1.1");

    for (int i=0; i < plan.countGroups(); i++)
    {
        vlePlanGroup *planGroup = plan.getGroup(i);

        // Create a new Group
        QDomElement newGrp = mTplHeader.cloneNode().toElement();
        updateField(newGrp, "{{name}}", planGroup->getName());
        updatePos(newGrp, 0, (i * planHeight));

        // ToDo : here, add activities

        e.appendChild(newGrp);
    }
    planSVG.appendChild( e );

    QByteArray data;
    QTextStream stream(&data);
    planSVG.save(stream, QDomNode::EncodingFromTextStream);

#ifdef PLAN_OUT
    QFile File("planOut.svg");
    File.open( QIODevice::WriteOnly );
    QTextStream TextStream(&File);
    planSVG.save(TextStream, 0);
    File.close();
    mFilename = "planOut.svg";
#else
    mFilename.clear();
#endif

    QXmlStreamReader xData(data);
    mSvgRenderer->load(&xData);
    refresh();
}

void SvgView::loadFile(QString fileName)
{
    qInfo() << "SVG load file " << fileName;

    mFilename = fileName;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader xmlReader( file.readAll() );
    file.close();

    mSvgRenderer->load(&xmlReader);

    refresh();
}

bool SvgView::loadTemplate(QString fileName)
{
    QFile file(fileName);

    // Read SVG file as pure XML
    file.open(QIODevice::ReadOnly);
    mTplDocument.setContent(&file);
    file.close();

    // Sanity check
    QDomElement e = mTplDocument.documentElement();
    if (e.nodeName() != "svg")
        return false;

    // Parse XML, search VLE templates
    for(QDomElement n = e.firstChildElement(); !n.isNull(); n = n.nextSiblingElement())
    {
        // Only "svg groups" are relevant
        if (n.nodeName() != "g")
            continue;

        if ( n.hasAttribute("vle:template") )
        {
            QString tplName = n.attribute("vle:template");
            if (tplName == "")
                continue;

            if (tplName == "header")
                mTplHeader = n;
            else if (tplName == "task")
                mTplTask = n;
        }
        else
            qWarning() << "SVG group with no template : " << n.attribute("id");

        //e.removeChild( n );
    }
    mTplRoot = e;

    return true;
}

void SvgView::refresh(void)
{
    QGraphicsScene *s = scene();

    s->clear();

    QGraphicsSvgItem * item = new QGraphicsSvgItem();
    item->setSharedRenderer(mSvgRenderer);

    mGraphicItem = item;
    mGraphicItem->setFlags(QGraphicsItem::ItemClipsToShape);
    mGraphicItem->setCacheMode(QGraphicsItem::NoCache);
    mGraphicItem->setZValue(0);
    mGraphicItem->sceneTransform().translate(0, 100);

    qInfo() << "SVG refresh()";

    s->addItem(mGraphicItem);
    //s->setSceneRect(0,0, 0,0);
}

void SvgView::wheelEvent(QWheelEvent* event)
{
    double scaleFactor = 1.15;

    if(event->delta() > 0)
        scale(scaleFactor, 1);
    else
        scale(1.0 / scaleFactor, 1);
}

void SvgView::updateField(QDomNode &e, QString tag, QString value)
{
    if ( e.nodeType() == QDomNode::TextNode)
    {
        if (e.nodeValue() == tag)
            e.setNodeValue(value);
    }
    else
    {
        for(QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
            updateField(n, tag, value);
    }
}

void SvgView::updatePos(QDomElement &e, int x, int y)
{
    QString v = QString("translate(%1,%2)").arg(x).arg(y);
    e.setAttribute("transform", v);
}
