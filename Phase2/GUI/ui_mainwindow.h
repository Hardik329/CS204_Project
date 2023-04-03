/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTableWidget *table1;
    QTableWidget *table2;
    QPushButton *btn_run;
    QPushButton *btn_reset;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1169, 744);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        table1 = new QTableWidget(centralwidget);
        if (table1->columnCount() < 1)
            table1->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table1->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (table1->rowCount() < 16)
            table1->setRowCount(16);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        __qtablewidgetitem2->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        __qtablewidgetitem3->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        __qtablewidgetitem4->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(3, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        __qtablewidgetitem5->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(4, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        __qtablewidgetitem6->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(5, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        __qtablewidgetitem7->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(6, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        __qtablewidgetitem8->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(7, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        __qtablewidgetitem9->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(8, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        __qtablewidgetitem10->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(9, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        __qtablewidgetitem11->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(10, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        __qtablewidgetitem12->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(11, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        __qtablewidgetitem13->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(12, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        __qtablewidgetitem14->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(13, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        __qtablewidgetitem15->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(14, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        __qtablewidgetitem16->setTextAlignment(Qt::AlignCenter);
        table1->setVerticalHeaderItem(15, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        table1->setItem(3, 0, __qtablewidgetitem17);
        table1->setObjectName("table1");
        table1->setGeometry(QRect(570, 10, 171, 621));
        table2 = new QTableWidget(centralwidget);
        if (table2->columnCount() < 1)
            table2->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        table2->setHorizontalHeaderItem(0, __qtablewidgetitem18);
        if (table2->rowCount() < 16)
            table2->setRowCount(16);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        __qtablewidgetitem19->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(0, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        __qtablewidgetitem20->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(1, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        __qtablewidgetitem21->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(2, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        __qtablewidgetitem22->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(3, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        __qtablewidgetitem23->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(4, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        __qtablewidgetitem24->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(5, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        __qtablewidgetitem25->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(6, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        __qtablewidgetitem26->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(7, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        __qtablewidgetitem27->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(8, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        __qtablewidgetitem28->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(9, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        __qtablewidgetitem29->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(10, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        __qtablewidgetitem30->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(11, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        __qtablewidgetitem31->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(12, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        __qtablewidgetitem32->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(13, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        __qtablewidgetitem33->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(14, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        __qtablewidgetitem34->setTextAlignment(Qt::AlignCenter);
        table2->setVerticalHeaderItem(15, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        table2->setItem(1, 0, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        table2->setItem(2, 0, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        table2->setItem(3, 0, __qtablewidgetitem37);
        table2->setObjectName("table2");
        table2->setGeometry(QRect(780, 10, 171, 621));
        btn_run = new QPushButton(centralwidget);
        btn_run->setObjectName("btn_run");
        btn_run->setGeometry(QRect(40, 140, 151, 41));
        btn_reset = new QPushButton(centralwidget);
        btn_reset->setObjectName("btn_reset");
        btn_reset->setGeometry(QRect(40, 210, 151, 41));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1169, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        QTableWidgetItem *___qtablewidgetitem = table1->verticalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "x0", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = table1->verticalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "x1", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = table1->verticalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "x2", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = table1->verticalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "x3", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = table1->verticalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "x4", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = table1->verticalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "x5", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = table1->verticalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "x6", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = table1->verticalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "x7", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = table1->verticalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "x8", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = table1->verticalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "x9", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = table1->verticalHeaderItem(10);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "x10", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = table1->verticalHeaderItem(11);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "x11", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = table1->verticalHeaderItem(12);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "x12", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = table1->verticalHeaderItem(13);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "x13", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = table1->verticalHeaderItem(14);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "x14", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = table1->verticalHeaderItem(15);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "x15", nullptr));

        const bool __sortingEnabled = table1->isSortingEnabled();
        table1->setSortingEnabled(false);
        table1->setSortingEnabled(__sortingEnabled);

        QTableWidgetItem *___qtablewidgetitem16 = table2->verticalHeaderItem(0);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "x16", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = table2->verticalHeaderItem(1);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "x17", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = table2->verticalHeaderItem(2);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "x18", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = table2->verticalHeaderItem(3);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "x19", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = table2->verticalHeaderItem(4);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "x20", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = table2->verticalHeaderItem(5);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "x21", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = table2->verticalHeaderItem(6);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "x22", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = table2->verticalHeaderItem(7);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "x23", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = table2->verticalHeaderItem(8);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "x24", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = table2->verticalHeaderItem(9);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "x25", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = table2->verticalHeaderItem(10);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "x26", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = table2->verticalHeaderItem(11);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "x27", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = table2->verticalHeaderItem(12);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "x28", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = table2->verticalHeaderItem(13);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "x29", nullptr));
        QTableWidgetItem *___qtablewidgetitem30 = table2->verticalHeaderItem(14);
        ___qtablewidgetitem30->setText(QCoreApplication::translate("MainWindow", "x30", nullptr));
        QTableWidgetItem *___qtablewidgetitem31 = table2->verticalHeaderItem(15);
        ___qtablewidgetitem31->setText(QCoreApplication::translate("MainWindow", "x31", nullptr));

        const bool __sortingEnabled1 = table2->isSortingEnabled();
        table2->setSortingEnabled(false);
        table2->setSortingEnabled(__sortingEnabled1);

        btn_run->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        btn_reset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
