/********************************************************************************
** Form generated from reading UI file 'patchconfirmdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PATCHCONFIRMDIALOG_H
#define UI_PATCHCONFIRMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE

class Ui_patchConfirmDialog
{
public:

    void setupUi(QMainWindow *patchConfirmDialog)
    {
        if (patchConfirmDialog->objectName().isEmpty())
            patchConfirmDialog->setObjectName(QStringLiteral("patchConfirmDialog"));
        patchConfirmDialog->resize(400, 300);

        retranslateUi(patchConfirmDialog);

        QMetaObject::connectSlotsByName(patchConfirmDialog);
    } // setupUi

    void retranslateUi(QMainWindow *patchConfirmDialog)
    {
        patchConfirmDialog->setWindowTitle(QApplication::translate("patchConfirmDialog", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class patchConfirmDialog: public Ui_patchConfirmDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PATCHCONFIRMDIALOG_H
