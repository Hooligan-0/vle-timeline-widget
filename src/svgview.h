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
#include <QtXml>
#include <QWheelEvent>
#include "vlePlan.h"

class QWheelEvent;
class QPaintEvent;
class QFile;

class SvgView: public QGraphicsView
{
    Q_OBJECT

public:
    SvgView(QWidget *parent = 0);
    void convert (const QString &xsltFile);
    QString getTplHeader(void);
    QString getTplTask  (void);
    void loadPlan(vlePlan &plan);
    void loadFile(QString fileName);
    bool loadTemplate(QString fileName);
    void refresh(void);
private:
    void updateField(QDomNode    &e, QString tag, QString value);
    void updatePos  (QDomElement &e, int x, int y);
protected:
    void wheelEvent(QWheelEvent* event);
private:
    // Widget variables
    QGraphicsItem *mGraphicItem;
    QSvgRenderer  *mSvgRenderer;
    // SVG template variables
    QDomDocument   mTplDocument;
    QDomElement    mTplRoot;
    QDomElement    mTplHeader;
    QDomElement    mTplTask;
    //
    qreal          mPixelPerDay;

    // Debug and temporary
    QString       mFilename;
};

#endif // SVGVIEW_H
