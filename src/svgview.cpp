/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#include <QFile>
#include <QGraphicsSvgItem>
#include <QScrollBar>
#include <QToolTip>
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

    mPlan = NULL;
    mPixelPerDay = 1;
    mZoomFactor  = 1.15;
    mZoomLevel   = 1;
    mMaxWidth    = 1500;
    mConfig.clear();
}

void SvgView::convert(const QString &xsltFile)
{
    (void)xsltFile;
    QXmlQuery query(QXmlQuery::XSLT20);
    QString   qOut;

    // ToDo : This function is a skeleton that can be used for XSLT processing

    query.setFocus(QUrl(mFilename));
    query.setQuery(QUrl(xsltFile));
    query.evaluateTo(&qOut);

    qWarning() << qOut;
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

QString SvgView::getTplTime(void)
{
    QString str;
    QTextStream stream(&str);
    mTplTime.save(stream, QDomNode::EncodingFromTextStream);
    return str;
}

void SvgView::loadPlan(vlePlan *plan)
{
    qWarning() << "SvgView::loadPlan";

    if ( mTplHeader.isNull() )
    {
        // ToDo : improve error handling
        qWarning() << "SvgView::loadPlan() Template error";
        return;
    }

    // Compute the height of a group
    if (mTplHeader.hasAttribute("height"))
        mGroupHeight = mTplHeader.attribute("height").toDouble();
    else
        mGroupHeight = 100;

    // Compute size of the whole plan
    int planHeight = mGroupHeight * (1 + plan->countGroups());
    int planWidth  = (mMaxWidth * mZoomLevel);

    // Create SVG document
    QDomDocument planSVG("xml");
    // Create root element
    QDomElement e = planSVG.createElement("svg");
    e.setAttribute("width",   QString(planWidth));
    e.setAttribute("height",  QString(planHeight));
    e.setAttribute("viewBox", QString("0 0 %1 %2").arg(planWidth).arg(planHeight));
    e.setAttribute("version", "1.1");

    QDate dateStart = plan->dateStart();
    QDate dateEnd   = plan->dateEnd();
    int nbDays = dateStart.daysTo(dateEnd);

    // In the plan duration is more than 1500 days
    if (nbDays > mMaxWidth)
    {
        // Update "pixel-per-day" to avoid very large picture
        qreal widgetSize = mMaxWidth;
        mPixelPerDay = (widgetSize / nbDays);
    }

    if (plan != mPlan)
    {
    qWarning() << "Plan period is from" << dateStart.toString("dd/MM/yyyy")
            << "to" << dateEnd.toString("dd/MM/yyyy")
            << "(" << nbDays<< "days)"
            << "[" << mPixelPerDay << "pixel per day]";
    }

    // First insert the time rule
    QDomElement timeGrp = mTplHeader.cloneNode().toElement();
    updateField(timeGrp, "{{name}}", "");
    updatePos  (timeGrp, 0, 0);
    updateAttr (timeGrp, "header_background", "width", QString::number(planWidth));
    float yLen = (mPixelPerDay * 365 * mZoomLevel);
    // Show Weeks
    if (yLen > 2000)
    {
        QDate r;
        if (dateStart.daysInMonth() == 1)
            r.setDate(dateStart.year(), dateStart.month(), dateStart.day());
        else
            r.setDate(dateStart.year(), dateStart.month() + 1, 1);
        while (r < dateEnd)
        {
            QDomElement newTimeStep = mTplTime.cloneNode().toElement();
            if (yLen < 5000)
                updateField(newTimeStep, "{{name}}", r.toString("dd/MM") );
            else
                updateField(newTimeStep, "{{name}}", r.toString("dd/MM/yy") );
            updateAttr (newTimeStep, "step_block", "width", QString::number(4));

            int offset = dateStart.daysTo(r);
            int aPos = (offset * mPixelPerDay * mZoomLevel);
            updatePos(newTimeStep, aPos, 0);
            timeGrp.appendChild(newTimeStep);
            r = r.addDays(7);
        }
    }
    // Show month
    else if (yLen > 500)
    {
        QDate r;
        if (dateStart.daysInMonth() == 1)
            r.setDate(dateStart.year(), dateStart.month(), dateStart.day());
        else
            r.setDate(dateStart.year(), dateStart.month() + 1, 1);
        while (r < dateEnd)
        {
            QDomElement newTimeStep = mTplTime.cloneNode().toElement();
            if (yLen < 1000)
                updateField(newTimeStep, "{{name}}", r.toString("MMM") );
            else
                updateField(newTimeStep, "{{name}}", r.toString("MMM yy") );
            updateAttr (newTimeStep, "step_block", "width", QString::number(4));

            int offset = dateStart.daysTo(r);
            int aPos = (offset * mPixelPerDay * mZoomLevel);
            updatePos(newTimeStep, aPos, 0);
            timeGrp.appendChild(newTimeStep);
            r = r.addMonths(1);
        }
    }
    // Show Year
    else
    {
        QDate r;
        if (dateStart.dayOfYear() == 1)
            r.setDate(dateStart.year(), dateStart.month(), dateStart.day());
        else
            r.setDate(dateStart.year() + 1, 1, 1);
        while (r < dateEnd)
        {
            QDomElement newTimeStep = mTplTime.cloneNode().toElement();
            updateField(newTimeStep, "{{name}}", QString::number(r.year()) );
            updateAttr (newTimeStep, "step_block", "width", QString::number(4));

            int offset = dateStart.daysTo(r);
            int aPos = (offset * mPixelPerDay * mZoomLevel);
            updatePos(newTimeStep, aPos, 0);
            timeGrp.appendChild(newTimeStep);
            r = r.addYears(1);
        }
    }
    e.appendChild(timeGrp);

    // Insert all the known groups
    for (int i=0; i < plan->countGroups(); i++)
    {
        vlePlanGroup *planGroup = plan->getGroup(i);
        vlePlanActivity *prevActivity = 0;
        int prevLen = 0;
        int prevOffset = 0;

        // Create a new Group
        QDomElement newGrp = mTplHeader.cloneNode().toElement();
        updateField(newGrp, "{{name}}", planGroup->getName());
        updatePos  (newGrp, 0, ((i + 1) * mGroupHeight));
        updateAttr (newGrp, "header_background", "width", QString::number(planWidth));

        for (int j = 0; j < planGroup->count(); j++)
        {
            vlePlanActivity *planActivity = planGroup->getActivity(j);

            QDate actStart = planActivity->dateStart();
            QDate actEnd   = planActivity->dateEnd();

            qreal actLength = (mPixelPerDay * actStart.daysTo(actEnd) * mZoomLevel);
            if (actLength < 1)
                actLength = 1;

            QDomElement newAct = mTplTask.cloneNode().toElement();
            updateField(newAct, "{{name}}", planActivity->getName());
            updateAttr (newAct, "activity_block", "width", QString::number(actLength));

            QString cfgColor("#00edda");
            QString activityClass = planActivity->getClass();
            if ( ! activityClass.isEmpty() )
            {
                QString cfg = getConfig("color", activityClass);
                if ( ! cfg.isEmpty() )
                    cfgColor = cfg;
            }
            QString fillStyle = QString(";fill:%1").arg(cfgColor);
            updateAttr (newAct, "activity_block", "style", fillStyle, false);

            int date = dateStart.daysTo(planActivity->dateStart());
            int aPos = (date * mPixelPerDay * mZoomLevel);

            if (prevActivity)
            {
                if (prevLen > aPos)
                {
                    if (prevOffset < 40)
                        prevOffset += 15;
                    updateAttr(newAct, "activity_name", "y", QString::number(prevOffset));
                }
                else
                    prevOffset = 15;
            }

            updatePos(newAct, aPos, 0);
            newGrp.appendChild(newAct);

            prevActivity = planActivity;
            prevLen = aPos + (planActivity->getName().size() * 8);
        }

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

    mPlan = plan;

    QXmlStreamReader xData(data);
    mSvgRenderer->load(&xData);
    refresh();
}

void SvgView::loadFile(QString fileName)
{
    qWarning() << "SVG load file " << fileName;

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

    mTplHeader.clear();
    mTplTask.clear();
    mTplTime.clear();

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
            else if (tplName == "time")
                mTplTime = n;
        }
        else
            qWarning() << "SVG group with no template : " << n.attribute("id");
    }
    mTplRoot = e;

    return true;
}

void SvgView::refresh(void)
{
    QGraphicsScene *s = scene();
    QPixmap Image(mSvgRenderer->defaultSize());
    QPainter Painter;

    Image.fill(Qt::transparent);

    qWarning() << "SVG refresh() zoom factor" << mZoomLevel;
    Painter.begin(&Image);
    mSvgRenderer->render(&Painter);
    Painter.end();
    s->clear();
    s->addPixmap(Image);
}

QString SvgView::getConfig(QString c, QString key)
{
    SvgViewConfig *entry = NULL;

    for (int i = 0; i < mConfig.count(); i++)
    {
        entry = mConfig.at(i);
        if (entry->getName() == c)
            break;
    }
    if (entry == NULL)
        return QString();
    return entry->getKey(key);
}

void SvgView::setConfig(QString c, QString key, QString value)
{
    SvgViewConfig *entry = NULL;
    for (int i = 0; i < mConfig.count(); i++)
    {
        SvgViewConfig *e;
        e = mConfig.at(i);
        if (e->getName() == c)
        {
            entry = e;
            break;
        }
    }

    if (entry == NULL)
    {
        entry = new SvgViewConfig();
        entry->setName(c);
        mConfig.push_back(entry);
    }
    if ( ! value.isEmpty())
        entry->setKey(key, value);
    else
        entry->removeKey(key);
}

void SvgView::setZommFactor(qreal factor)
{
    mZoomFactor = factor;
}

void SvgView::mouseMoveEvent(QMouseEvent *event)
{
    // Search the group at the current mouse Y
    QPoint pos = event->pos();
    int mouseGroup = pos.y() / mGroupHeight;
    // If mouse is outside the plan, nothing to do
    if ( (mouseGroup == 0) ||
         (mouseGroup > mPlan->countGroups()) )
    {
        if (QToolTip::isVisible())
            QToolTip::hideText();
        return;
    }

    vlePlanGroup *planGroup = mPlan->getGroup(mouseGroup - 1);
    QDate dateStart = mPlan->dateStart();

    // Get mouse X position
    int currentX = horizontalScrollBar()->value() + pos.x();
    int mouseTimePos = (currentX - 120);

    // Search if the mouse is over one acivity of the current group
    for (int j = 0; j < planGroup->count(); j++)
    {
        vlePlanActivity *planActivity = planGroup->getActivity(j);
        // Convert activity date to "days from the begining of the plan"
        int dataOffsetStart = dateStart.daysTo(planActivity->dateStart());
        int dateOffsetEnd   = dateStart.daysTo(planActivity->dateEnd());
        // Convert this number of days to pixels/coordinates
        int startPos = (dataOffsetStart * mPixelPerDay * mZoomLevel);
        int endPos   = (dateOffsetEnd   * mPixelPerDay * mZoomLevel);
        // Compare activity start/end to the current mouse position
        if ( (mouseTimePos >= startPos) &&
             (mouseTimePos <= endPos) )
        {
            if ( ( planActivity->attributeCount() ) &&
                 ( ! QToolTip::isVisible()) )
            {
                QString newMsg(planActivity->getName());

                for (int k = 0; k < planActivity->attributeCount(); k++)
                    newMsg += "\n" + planActivity->getAttribute(k);
                QRect tipPos(pos.x() - 10, pos.y() - 10, 20, 20);
                QToolTip::showText(event->globalPos(), newMsg, this, tipPos);
            }
        }
    }
}

void SvgView::wheelEvent(QWheelEvent* event)
{
    if(event->delta() > 0)
        mZoomLevel = (mZoomLevel * mZoomFactor);
    else
    {
        if (mZoomLevel > 0.4)
            mZoomLevel = (mZoomLevel / mZoomFactor);
    }

    loadPlan(mPlan);
}

void SvgView::updateAttr(QDomNode &node, QString selector, QString tag, QString value, bool replace)
{
    if ( ! node.isElement())
        return;

    QDomElement e = node.toElement();

    if (e.hasAttribute("vle:selector") && (e.attribute("vle:selector") == selector))
    {
        if (replace)
            e.setAttribute(tag, value);
        else
        {
            QString newVal = e.attribute(tag);
            newVal.append(value);
            e.setAttribute(tag, newVal);
        }
    }

    // Continue search across child nodes
    for(QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
        updateAttr(n, selector, tag, value, replace);
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
