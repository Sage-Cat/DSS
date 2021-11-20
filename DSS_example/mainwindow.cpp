#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dss.h"

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

void MainWindow::on_btn_encode_clicked()
{

}

void MainWindow::on_btn_verify_clicked()
{

}

