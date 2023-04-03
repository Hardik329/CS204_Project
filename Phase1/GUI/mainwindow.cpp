#include "mainwindow.h"
#include "ui_mainwindow.h"


#define _CRT_SECURE_NO_WARNINGS

extern int X[32];
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_run_clicked()
{
    run_riscvsim();
    QTableWidgetItem* item;
    item = new QTableWidgetItem(QString::number(69));
    ui->table1->setItem(3,0,item);
     for(int i = 0;i<16;i++){
         item = new QTableWidgetItem(QString::number(X[i]));
         ui->table1->setItem(i,0,item);
     }
     for(int i = 16;i<31;i++){
         item = new QTableWidgetItem(QString::number(X[i]));
         ui->table2->setItem(i-16,0,item);
     }



}


void MainWindow::on_btn_reset_clicked()
{
    reset_proc();
//   MainWindow.write_ui();
    QTableWidgetItem* item;
     for(int i = 0;i<16;i++){
         item = new QTableWidgetItem(QString::number(X[i]));
         ui->table1->setItem(i,0,item);
     }
     for(int i = 16;i<31;i++){
         item = new QTableWidgetItem(QString::number(X[i]));
         ui->table2->setItem(i-16,0,item);
     }

}

