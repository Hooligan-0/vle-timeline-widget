/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#ifndef SVGVIEW_H
#define SVGVIEW_H

#include <QGraphicsView>
#include <QSvgRenderer>
#include <QWheelEvent>
#include "vlePlan.h"

class QWheelEvent;
class QPaintEvent;
class QFile;

class SvgView: public QGraphicsView
{
    Q_OBJECT

public:
    enum RendererType { Native, OpenGL, Image };
    SvgView(QWidget *parent = 0);
    void loadPlan(const vlePlan &plan);
    void openFile(QString fileName);
    void convert (const QString &xsltFile);
protected:
    void wheelEvent(QWheelEvent* event);
private:
    RendererType       mRenderer;
    QGraphicsItem     *m_svgItem;
    QGraphicsRectItem *m_backgroundItem;
    QGraphicsRectItem *m_outlineItem;

    QString       mFilename;
    QImage m_image;

    QSvgRenderer *m_svgRenderer;
};

#endif // SVGVIEW_H
