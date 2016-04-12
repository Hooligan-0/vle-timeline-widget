/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#include <QFile>
#include <QString>
#include <QDebug>
#include "vlePlan.h"

vlePlan::vlePlan()
{
    mGroups.clear();
}

void vlePlan::clear(void)
{
    // Delete all known groups
    while ( ! mGroups.isEmpty())
        delete mGroups.takeFirst();
}

int vlePlan::countGroups(void)
{
    return mGroups.count();
}
int vlePlan::countActivities(void)
{
    int count = 0;
    for (int i = 0; i < mGroups.size(); ++i)
    {
        vlePlanGroup *a = mGroups.at(i);
        count += a->count();
    }
    return count;
}

void vlePlan::loadFile(const QString &filename)
{
    QFile file(filename);
    int lineCount;

    file.open(QIODevice::ReadOnly);

    lineCount = 0;

    while ( ! file.atEnd() )
    {
        // Get one line of text from CSV
        QByteArray line = file.readLine();
        // Split the string to get CSV columns into a list
        QStringList fields = QString(line).split(QChar(';'));

        if (lineCount == 0)
        {
            // If the header line is malformed, abort file load
            if (fields.count() < 4)
                break;
            // Clear current plan (if any previously loaded)
            clear();
            lineCount++;
            continue;
        }
        // Sanity check
        if (fields.count() < 4)
            continue;

        // Copy values into named strings (poor perf but usefull during dev)
        QString activityName( fields.at(0) );
        QString groupName   ( fields.at(1) );
        QString startDate   ( fields.at(2) );
        QString endDate     ( fields.at(3) );

        // Save the activity into the vlePlan
        vlePlanGroup    *g = getGroup(groupName, true);
        vlePlanActivity *a = g->addActivity(activityName);
        a->setStart(QDate::fromString(startDate, Qt::ISODate));
        a->setEnd  (QDate::fromString(endDate,   Qt::ISODate));

        lineCount++;
    }
    file.close();
}

vlePlanGroup *vlePlan::getGroup(QString name, bool create)
{
    vlePlanGroup *ret = NULL;

    for (int i = 0; i < mGroups.size(); ++i)
    {
        vlePlanGroup *a = mGroups.at(i);
        if (a->getName() == name)
        {
            ret = a;
            break;
        }
    }
    if ( (ret == NULL) && create)
    {
        ret = new vlePlanGroup(name);
        mGroups.push_back(ret);
    }
    return ret;
}

// ******************** Activities ******************** //

vlePlanActivity::vlePlanActivity(QString name)
{
    mName = name;
}
vlePlanActivity::~vlePlanActivity()
{
    // Nothing to do
}

QString vlePlanActivity::getName(void)
{
    return mName;
}
void vlePlanActivity::setName(QString name)
{
    mName = name;
}

void vlePlanActivity::setStart(QDate date)
{
    mDateStart = date;
}

void vlePlanActivity::setEnd(QDate date)
{
    mDateEnd = date;
}

// ******************** Groups ******************** //

vlePlanGroup::vlePlanGroup(QString name)
{
    mName = name;
    mActivities.clear();
}

vlePlanGroup::~vlePlanGroup()
{
    // Delete all known activities
    while ( ! mActivities.isEmpty())
        delete mActivities.takeFirst();
}

QString vlePlanGroup::getName(void)
{
    return mName;
}
void vlePlanGroup::setName(QString name)
{
    mName = name;
}

int vlePlanGroup::count(void)
{
    return mActivities.count();
}

vlePlanActivity *vlePlanGroup::addActivity(QString name)
{
    vlePlanActivity *newAct;

    newAct = new vlePlanActivity(name);

    // Insert it to the list of known activities
    mActivities.push_back(newAct);

    return newAct;
}
