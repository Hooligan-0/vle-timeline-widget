/*
 * This file is part of VLE, a framework for multi-modeling,
 * simulation and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2016 Agilack
 */
#include <QColorDialog>
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

    // Set some default color values
    ui->svgUi->setConfig("color", "Irrigation", "#aa0000");

    connect(ui->buttonSelectCSV, SIGNAL(clicked(bool)), this, SLOT (buttonLoadCSV(bool)));
    connect(ui->buttonSelectSVG, SIGNAL(clicked(bool)), this, SLOT (buttonLoadSVG(bool)));
    connect(ui->buttonConvert,   SIGNAL(clicked(bool)), this, SLOT (buttonConvert(bool)));
    // Configuration tab
    connect(ui->cfgColorTable,   SIGNAL(itemSelectionChanged()), this, SLOT(cfgColorSelectionChange()));
    connect(ui->cfgColorAdd,     SIGNAL(clicked()), this, SLOT (buttonCfgColorAdd()));
    connect(ui->cfgColorRemove,  SIGNAL(clicked()), this, SLOT (buttonCfgColorRemove()));
    connect(ui->cfgColorPicker,  SIGNAL(clicked()), this, SLOT (buttonCfgColorPicker()));
    connect(ui->cfgColorSet,     SIGNAL(clicked()), this, SLOT (buttonCfgColorSet()));
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
        ui->svgEditHeader->clear();
        ui->svgEditHeader->appendPlainText( ui->svgUi->getTplHeader() );
        ui->svgEditHeader->moveCursor(QTextCursor::Start);
        // Dump template task to ui text-box
        ui->svgEditTask->clear();
        ui->svgEditTask->appendPlainText( ui->svgUi->getTplTask() );
        ui->svgEditTask->moveCursor(QTextCursor::Start);
        // Dump template time to ui text-box
        ui->svgEditTime->clear();
        ui->svgEditTime->appendPlainText( ui->svgUi->getTplTime() );
        ui->svgEditTime->moveCursor(QTextCursor::Start);
    }
}

void MainWindow::cfgColorSelectionChange(void)
{
    if (ui->cfgColorTable->selectedItems().count() == 0)
    {
        ui->cfgColorSet->setEnabled(false);
        ui->cfgColorActivityName->setText("");
        ui->cfgColorActivityColor->setText("");
        return;
    }

    QTableWidgetItem *name  = ui->cfgColorTable->selectedItems().at(0);
    QTableWidgetItem *color = ui->cfgColorTable->selectedItems().at(1);

    ui->cfgColorActivityName->setText (name->text() );
    ui->cfgColorActivityColor->setText(color->text());
    ui->cfgColorSet->setEnabled(true);
}

void MainWindow::buttonCfgColorAdd(void)
{
    int count = ui->cfgColorTable->rowCount();

    QTableWidgetItem *newActivity = new QTableWidgetItem("NewActivity");
    QTableWidgetItem *newColor    = new QTableWidgetItem("#000000");

    ui->cfgColorTable->insertRow(count);

    ui->cfgColorTable->setItem(count, 0, newActivity);
    ui->cfgColorTable->setItem(count, 1, newColor);

    ui->cfgColorTable->setCurrentItem(newActivity);
}

void MainWindow::buttonCfgColorRemove(void)
{
    if (ui->cfgColorTable->selectedItems().count() == 0)
        return;
    QTableWidgetItem *item = ui->cfgColorTable->selectedItems().at(0);
    QString itemName = item->text();
    ui->cfgColorTable->removeRow( item->row() );

    ui->svgUi->setConfig("color", itemName, QString());
}

void MainWindow::buttonCfgColorPicker(void)
{
    QColor currentColor;

    QString colorName = ui->cfgColorActivityColor->text();
    currentColor.setNamedColor(colorName);

    QColor color = QColorDialog::getColor(currentColor, this, tr("Activity color"));
    ui->cfgColorActivityColor->setText( color.name() );
}

void MainWindow::buttonCfgColorSet(void)
{
    QTableWidgetItem *itemName  = ui->cfgColorTable->selectedItems().at(0);
    QTableWidgetItem *itemColor = ui->cfgColorTable->selectedItems().at(1);

    QString activityName  = ui->cfgColorActivityName->text();
    QString activityColor = ui->cfgColorActivityColor->text();

    itemName->setText ( activityName  );
    itemColor->setText( activityColor );

    ui->svgUi->setConfig("color", activityName, activityColor);
}
