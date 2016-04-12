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
    mRenderer(Native),
    m_svgItem(0),
    m_backgroundItem(0),
    m_outlineItem(0)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_svgRenderer = new QSvgRenderer();

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

void SvgView::loadPlan(const vlePlan &plan)
{
    (void)plan;
}

void SvgView::openFile(QString fileName)
{
    QGraphicsScene *s = scene();

    s->clear();

    mFilename = fileName;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray Data = file.readAll();
    file.close();

    QXmlStreamReader xmlReader(Data);
    m_svgRenderer->load(&xmlReader);

    QGraphicsSvgItem * item = new QGraphicsSvgItem();
    item->setSharedRenderer(m_svgRenderer);

    m_svgItem = item;
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);
    m_svgItem->sceneTransform().translate(0, 100);

    s->addItem(m_svgItem);
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
