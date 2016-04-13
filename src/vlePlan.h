/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#ifndef VLEPLAN_H
#define VLEPLAN_H

#include <QDate>
#include <QList>

class vlePlanActivity
{
public:
    vlePlanActivity (QString name);
    ~vlePlanActivity();
    QString getName (void);
    void    setName (QString name);
    void    setStart(QDate  date);
    void    setEnd  (QDate  date);
private:
    QString mName;
    QDate   mDateStart;
    QDate   mDateEnd;
};

class vlePlanGroup
{
public:
    vlePlanGroup   (QString name);
    ~vlePlanGroup  ();
    QString getName(void);
    void    setName(QString name);
    int     count(void);
    vlePlanActivity *addActivity(QString name);
private:
    QString mName;
    QList<vlePlanActivity *> mActivities;
};

class vlePlan
{
public:
    vlePlan();
    void clear(void);
    void loadFile(const QString &filename);
    vlePlanGroup *getGroup(QString name, bool create = false);
    vlePlanGroup *getGroup(int pos);
    int  countGroups(void);
    int  countActivities(void);
    bool isValid(void);
private:
    bool mValid;
    QList<vlePlanGroup *> mGroups;
};

#endif // VLEPLAN_H
