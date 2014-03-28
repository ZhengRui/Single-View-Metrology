#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    svmView = new SVMGraphicsView(this);
    setCentralWidget(svmView);

    setWindowTitle(tr("Single View Modeling"));
    resize(800, 600);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionOpen_triggered()
{
    svmView->openAct();
//    ui->actionXLines->setEnabled(true);
//    ui->actionYLines->setEnabled(true);
//    ui->actionZLines->setEnabled(true);
}


void MainWindow::on_actionRescale_triggered()
{
    svmView->rescaleAct();
}

void MainWindow::on_actionXLines_triggered()
{
    svmView->pickXLines();
}

void MainWindow::on_actionYLines_triggered()
{
    svmView->pickYLines();
}

void MainWindow::on_actionZLines_triggered()
{
    svmView->pickZLines();
}

void MainWindow::on_actionRPlane_triggered()
{
//    if(svmView->isLinesReady())
//    {
//        ui->actionXLines->setEnabled(false);
//        ui->actionYLines->setEnabled(false);
//        ui->actionZLines->setEnabled(false);
        svmView->pickRefPlane();
//    }
}

void MainWindow::on_actionRHeight_triggered()
{
    svmView->pickRefHeightPoint();
}


void MainWindow::on_actionXVPoint_triggered()
{
    svmView->calXVP();
}

void MainWindow::on_actionYVPoint_triggered()
{
    svmView->calYVP();
}

void MainWindow::on_actionZVPoint_triggered()
{
    svmView->calZVP();
}

void MainWindow::on_actionHomography_triggered()
{
    svmView->calHomography_Public();
}


void MainWindow::on_actionAlpha_triggered()
{
    svmView->calGamma();
}

void MainWindow::on_actionScene2Image_triggered()
{
    svmView->scnMapToImg();
}

void MainWindow::on_actionPtPool_triggered()
{
    svmView->make3DPtPool();
}

void MainWindow::on_actionPolygon_triggered()
{
    svmView->pickPolygon();
}

void MainWindow::on_actionTexture_triggered()
{
    svmView->calPatch();
}

void MainWindow::on_actionSave_triggered()
{
    svmView->saveModel();
}
