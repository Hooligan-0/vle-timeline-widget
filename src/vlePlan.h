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
    QDate   dateEnd   (void);
    QDate   dateStart (void);
    QString getClass(void);
    QString getName (void);
    void    setClass(QString c);
    void    setName (QString name);
    void    setStart(QDate   date);
    void    setEnd  (QDate   date);
private:
    QString mName;
    QString mClass;
    QDate   mDateStart;
    QDate   mDateEnd;
};

class vlePlanGroup
{
public:
    vlePlanGroup   (QString name);
    ~vlePlanGroup  ();
    QDate   dateEnd  (void);
    QDate   dateStart(void);
    QString getName(void);
    void    setName(QString name);
    int     count(void);
    vlePlanActivity *addActivity(QString name);
    vlePlanActivity *getActivity(int pos);
private:
    QDate   mDateEnd;    // Cache for the lastest "end date" of group activities
    QDate   mDateStart;  // Cache for the earliest "start date"of group activities
    QString mName;
    QList<vlePlanActivity *> mActivities;
};

class vlePlan
{
public:
    vlePlan();
    void  clear(void);
    QDate dateEnd  (void);
    QDate dateStart(void);
    void loadFile(const QString &filename);
    vlePlanGroup *getGroup(QString name, bool create = false);
    vlePlanGroup *getGroup(int pos);
    int  countGroups(void);
    int  countActivities(void);
    bool isValid(void);
private:
    bool  mValid;
    QDate mDateEnd;    // Cache for the start date
    QDate mDateStart;  // Cache for the end date
    QList<vlePlanGroup *> mGroups;
};

#endif // VLEPLAN_H
