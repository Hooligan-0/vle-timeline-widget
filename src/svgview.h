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

class SvgViewConfig
{
public:
    SvgViewConfig() { }
    QString getName(void)      { return mName; }
    QString getKey (QString k) { return mConfig.value(k); }
    void    setName(QString v) { mName = v; }
    void    setKey (QString k, QString v) { mConfig.insert(k, v); }
    void removeKey (QString k) { mConfig.remove(k); }
private:
    QString mName;
    QMap<QString, QString> mConfig;
};

class SvgView: public QGraphicsView
{
    Q_OBJECT

public:
    SvgView(QWidget *parent = 0);
    void convert (const QString &xsltFile);
    QString getTplHeader(void);
    QString getTplTask  (void);
    void loadPlan(vlePlan *plan);
    void loadFile(QString fileName);
    bool loadTemplate(QString fileName);
    void refresh (void);
    QString getConfig(QString c, QString key);
    void    setConfig(QString c, QString key, QString value);
    void setZommFactor(qreal factor);
private:
    void updateAttr (QDomNode    &e, QString selector, QString attr, QString value, bool replace = true);
    void updateField(QDomNode    &e, QString tag,  QString value);
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
    vlePlan       *mPlan;
    int            mMaxWidth;
    qreal          mPixelPerDay;
    qreal          mZoomFactor;
    qreal          mZoomLevel;
    QList<SvgViewConfig *> mConfig;

    // Debug and temporary
    QString       mFilename;
};

#endif // SVGVIEW_H
