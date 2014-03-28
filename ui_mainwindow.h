/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionXLines;
    QAction *actionYLines;
    QAction *actionZLines;
    QAction *actionRPlane;
    QAction *actionRHeight;
    QAction *actionPolygon;
    QAction *actionRescale;
    QAction *actionReset;
    QAction *actionXVPoint;
    QAction *actionYVPoint;
    QAction *actionZVPoint;
    QAction *actionHomography;
    QAction *actionAlpha;
    QAction *actionTexture;
    QAction *actionScene2Image;
    QAction *actionPtPool;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuCalculation;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 300);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionXLines = new QAction(MainWindow);
        actionXLines->setObjectName(QStringLiteral("actionXLines"));
        actionYLines = new QAction(MainWindow);
        actionYLines->setObjectName(QStringLiteral("actionYLines"));
        actionZLines = new QAction(MainWindow);
        actionZLines->setObjectName(QStringLiteral("actionZLines"));
        actionRPlane = new QAction(MainWindow);
        actionRPlane->setObjectName(QStringLiteral("actionRPlane"));
        actionRHeight = new QAction(MainWindow);
        actionRHeight->setObjectName(QStringLiteral("actionRHeight"));
        actionPolygon = new QAction(MainWindow);
        actionPolygon->setObjectName(QStringLiteral("actionPolygon"));
        actionRescale = new QAction(MainWindow);
        actionRescale->setObjectName(QStringLiteral("actionRescale"));
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        actionXVPoint = new QAction(MainWindow);
        actionXVPoint->setObjectName(QStringLiteral("actionXVPoint"));
        actionYVPoint = new QAction(MainWindow);
        actionYVPoint->setObjectName(QStringLiteral("actionYVPoint"));
        actionZVPoint = new QAction(MainWindow);
        actionZVPoint->setObjectName(QStringLiteral("actionZVPoint"));
        actionHomography = new QAction(MainWindow);
        actionHomography->setObjectName(QStringLiteral("actionHomography"));
        actionAlpha = new QAction(MainWindow);
        actionAlpha->setObjectName(QStringLiteral("actionAlpha"));
        actionTexture = new QAction(MainWindow);
        actionTexture->setObjectName(QStringLiteral("actionTexture"));
        actionScene2Image = new QAction(MainWindow);
        actionScene2Image->setObjectName(QStringLiteral("actionScene2Image"));
        actionPtPool = new QAction(MainWindow);
        actionPtPool->setObjectName(QStringLiteral("actionPtPool"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 19));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuCalculation = new QMenu(menuBar);
        menuCalculation->setObjectName(QStringLiteral("menuCalculation"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuCalculation->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionXLines);
        menuEdit->addAction(actionYLines);
        menuEdit->addAction(actionZLines);
        menuEdit->addAction(actionRPlane);
        menuEdit->addAction(actionRHeight);
        menuEdit->addAction(actionPtPool);
        menuEdit->addAction(actionPolygon);
        menuEdit->addSeparator();
        menuEdit->addAction(actionRescale);
        menuEdit->addAction(actionReset);
        menuCalculation->addAction(actionXVPoint);
        menuCalculation->addAction(actionYVPoint);
        menuCalculation->addAction(actionZVPoint);
        menuCalculation->addAction(actionHomography);
        menuCalculation->addAction(actionAlpha);
        menuCalculation->addAction(actionTexture);
        menuCalculation->addSeparator();
        menuCalculation->addSeparator();
        menuCalculation->addAction(actionScene2Image);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0));
        actionXLines->setText(QApplication::translate("MainWindow", "XLines", 0));
        actionYLines->setText(QApplication::translate("MainWindow", "YLines", 0));
        actionZLines->setText(QApplication::translate("MainWindow", "ZLines", 0));
        actionRPlane->setText(QApplication::translate("MainWindow", "RPlane", 0));
        actionRHeight->setText(QApplication::translate("MainWindow", "RHeight", 0));
        actionPolygon->setText(QApplication::translate("MainWindow", "Polygon", 0));
        actionRescale->setText(QApplication::translate("MainWindow", "Rescale", 0));
        actionReset->setText(QApplication::translate("MainWindow", "Reset", 0));
        actionXVPoint->setText(QApplication::translate("MainWindow", "XVPoint", 0));
        actionYVPoint->setText(QApplication::translate("MainWindow", "YVPoint", 0));
        actionZVPoint->setText(QApplication::translate("MainWindow", "ZVPoint", 0));
        actionHomography->setText(QApplication::translate("MainWindow", "Homography", 0));
        actionAlpha->setText(QApplication::translate("MainWindow", "Alpha", 0));
        actionTexture->setText(QApplication::translate("MainWindow", "Texture", 0));
        actionScene2Image->setText(QApplication::translate("MainWindow", "Scene2Image", 0));
        actionPtPool->setText(QApplication::translate("MainWindow", "PtPool", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0));
        menuCalculation->setTitle(QApplication::translate("MainWindow", "Calculation", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
