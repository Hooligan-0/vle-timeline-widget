/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>
#include "vlePlan.h"

vlePlan::vlePlan()
{
    mValid = false;
    mGroups.clear();
}

void vlePlan::clear(void)
{
    // Delete all known groups
    while ( ! mGroups.isEmpty())
        delete mGroups.takeFirst();
    // Mark current plan as invalid
    mValid = false;
}

QDate vlePlan::dateEnd(void)
{
    if ( mDateEnd.isValid() )
        return mDateEnd;

    QDate lastest;

    for (int i = 0; i < mGroups.count(); i++)
    {
        vlePlanGroup *g = mGroups.at(i);

        // If the reference is not already set
        if ( ! lastest.isValid())
        {
            // Use the start date of this group as reference
            lastest = g->dateEnd();
            continue;
        }

        // If the end of this group is after the reference date
        if (g->dateEnd() > lastest)
            // Update to the lastest
            lastest = g->dateEnd();
    }

    // Save the start date (cache)
    mDateEnd = lastest;

    return mDateEnd;
}

QDate vlePlan::dateStart(void)
{
    if ( mDateStart.isValid() )
        return mDateStart;

    QDate oldest;

    for (int i = 0; i < mGroups.count(); i++)
    {
        vlePlanGroup *g = mGroups.at(i);

        // If the reference is not already set
        if ( ! oldest.isValid())
        {
            // Use the start date of this group as reference
            oldest = g->dateStart();
            continue;
        }

        // If the start of this group is before the reference date
        if (g->dateStart() < oldest)
            // Update to the oldest
            oldest = g->dateStart();
    }

    // Save the start date (cache)
    mDateStart = oldest;

    return mDateStart;
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

    // If (at least) one group has been loaded ...
    if (countGroups() > 0)
        // ... then, Plan is now valid
        mValid = true;
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

        // Reset cache to NULL date
        mDateEnd   = QDate();
        mDateStart = QDate();
    }
    return ret;
}

vlePlanGroup *vlePlan::getGroup(int pos)
{
    if (pos > mGroups.count())
        return NULL;

    return mGroups.at(pos);
}

bool vlePlan::isValid(void)
{
    return mValid;
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

QDate vlePlanActivity::dateEnd(void)
{
    return mDateEnd;
}

QDate vlePlanActivity::dateStart(void)
{
    return mDateStart;
}

QString vlePlanActivity::getClass(void)
{
    return mClass;
}

QString vlePlanActivity::getName(void)
{
    return mName;
}

void vlePlanActivity::setClass(QString c)
{
    mClass = c;
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

QDate vlePlanGroup::dateEnd(void)
{
    if ( mDateEnd.isValid() )
        return mDateEnd;

    QDate lastest;

    for (int i = 0; i < mActivities.count(); i++)
    {
        vlePlanActivity *a = mActivities.at(i);

        // If the reference is not already set
        if ( ! lastest.isValid())
        {
            // Use the start date of this activity as reference
            lastest = a->dateEnd();
            continue;
        }

        // If the end of this activity is after the reference date
        if (a->dateEnd() > lastest)
            // Update to the lastest
            lastest = a->dateEnd();
    }

    // Save the start date (cache)
    mDateEnd = lastest;

    return mDateEnd;
}

QDate vlePlanGroup::dateStart(void)
{
    if ( mDateStart.isValid() )
        return mDateStart;

    QDate oldest;

    for (int i = 0; i < mActivities.count(); i++)
    {
        vlePlanActivity *a = mActivities.at(i);

        // If the reference is not already set
        if ( ! oldest.isValid())
        {
            // Use the start date of this activity as reference
            oldest = a->dateStart();
            continue;
        }

        // If the start of this activity is before the reference date
        if (a->dateStart() < oldest)
            // Update to the oldest
            oldest = a->dateStart();
    }

    // Save the start date (cache)
    mDateStart = oldest;

    return mDateStart;
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

    // Reset cache to NULL date
    mDateEnd   = QDate();
    mDateStart = QDate();

    return newAct;
}

vlePlanActivity *vlePlanGroup::getActivity(int pos)
{
    if (pos > mActivities.count())
        return NULL;

    return mActivities.at(pos);
}
