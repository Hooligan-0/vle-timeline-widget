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
    setWindowTitle("VLE plan Widget Unit-test");

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

    if (mPlan.isValid())
        ui->svgUi->loadPlan(&mPlan);
    else
    {
        QMessageBox msg;
        msg.setText("Plan not loaded, please specify a CSV");
        msg.exec();
    }
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

void MainWindow::buttonLoadSVG(bool c)
{
    QString fileName;
    (void)c;

    // Show an "Open File" dialog
    fileName = QFileDialog::getOpenFileName(this, tr("Open SVG File"), "", tr("SVG Files (*.svg)"));
    // Update filename text-box
    ui->tplFilename->setText(fileName);

    // If the selected file exists ...
    if (QFile(fileName).exists())
    {
        // ... load it
        ui->svgUi->loadTemplate(fileName);

        // Dump template header to ui text-box
        ui->svgEditHeader->appendPlainText( ui->svgUi->getTplHeader() );
        ui->svgEditHeader->moveCursor(QTextCursor::Start);
        // Dump template task to ui text-box
        ui->svgEditTask->appendPlainText( ui->svgUi->getTplTask() );
        ui->svgEditTask->moveCursor(QTextCursor::Start);
    }
}
