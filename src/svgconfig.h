/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#ifndef SVGCONFIG_H
#define SVGCONFIG_H

#include <QTableWidget>
#include <QWidget>
#include "svgview.h"
#include "vlePlan.h"

class svgConfig : public QWidget
{
    Q_OBJECT
public:
    explicit svgConfig(QWidget *parent = 0);
    void clear(void);
    void setDefaultColor(QString name);
    void setPlan(vlePlan *plan);
    void setView(SvgView *view);
private:
    void setupUi(void);
private slots:
    void colorSelectionChange();
    void colorSelectionEdit(int row, int col);
private:
    vlePlan      *mPlan;
    QString       mDefaultColor;
    QTableWidget *mUiColorTable;
    SvgView      *mViewWidget;
};

#endif // SVGCONFIG_H
