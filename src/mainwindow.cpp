/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonSelectCSV, SIGNAL(clicked(bool)), this, SLOT (buttonLoadCSV(bool)));
    connect(ui->buttonSelectSVG, SIGNAL(clicked(bool)), this, SLOT (buttonLoadSVG(bool)));
    connect(ui->buttonConvert,   SIGNAL(clicked(bool)), this, SLOT (buttonConvert(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
    mPlan.clear();
}

void MainWindow::buttonConvert(bool c)
{
    (void)c;

    QMessageBox msgBox;
    msgBox.setText("Not implemented yet !");
    msgBox.exec();
}

void MainWindow::buttonLoadCSV(bool c)
{
    QString fileName;
    (void)c;

    // Show an "Open File" dialog
    fileName = QFileDialog::getOpenFileName(this, tr("Open VLE Plan file"), "", tr("CSV Files (*.csv)"));
    // Update filename text-box
    ui->csvFilename->setText(fileName);

    // If the selected file exists ...
    if (QFile(fileName).exists())
    {
        // ... load it
        mPlan.loadFile(fileName);

        // Update ui to show Plan statistics
        ui->labelGroupCount->setText   (QString::number(mPlan.countGroups()));
        ui->labelActivityCount->setText(QString::number(mPlan.countActivities()));
    }
}

void MainWindow::loadSVGTemplate(QString &filename)
{
    QFile file(filename);

    // Read SVG file as pure XML
    file.open(QIODevice::ReadOnly);
    QDomDocument xmlContent;
    xmlContent.setContent(&file);
    file.close();

    // Sanity check
    QDomElement e = xmlContent.documentElement();
    if (e.nodeName() != "svg")
        return;

    // Parse XML, search VLE templates
    for(QDomElement n = e.firstChildElement(); !n.isNull(); n = n.nextSiblingElement())
    {
        // Only "svg groups" are relevant
        if (n.nodeName() != "g")
            continue;

        if ( n.hasAttribute("vle:template") )
        {
            QString str;
            QTextStream stream(&str);

            QString tplName = n.attribute("vle:template");
            if (tplName == "")
                continue;

            // Convert XML element to text
            n.save(stream, QDomNode::CDATASectionNode);

            if (tplName == "header")
            {
                // Dump this text to ui text-box
                ui->svgEditHeader->appendPlainText(str);
                ui->svgEditHeader->moveCursor(QTextCursor::Start);
            }
            else if (tplName == "task")
            {
                // Dump this text to ui text-box
                ui->svgEditTask->appendPlainText(str);
                ui->svgEditTask->moveCursor(QTextCursor::Start);
            }
        }
        else
            qWarning() << "SVG group with no template : " << n.attribute("id");
    }

    // DEBUG
    ui->svgUi->openFile(filename);
}

void MainWindow::buttonLoadSVG(bool c)
{
    QString fileName;
    (void)c;

    // Show an "Open File" dialog
    fileName = QFileDialog::getOpenFileName(this,
             tr("Open SVG File"), "", tr("SVG Files (*.svg)"));
    // Update filename text-box
    ui->tplFilename->setText(fileName);

    // If the selected file exists ...
    if (QFile(fileName).exists())
        // ... load it
        loadSVGTemplate(fileName);
}
