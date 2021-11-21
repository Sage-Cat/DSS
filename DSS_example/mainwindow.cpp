#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dss.h"

#include <QMessageBox>
#include <QPlainTextEdit>

const uint64_t
    G { 5 },
    P { 7 },
    q { 11 },
    X { 1023 };

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _dss(new DSS(G, P, q, X))
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_encode_clicked()
{
    QByteArray m = ui->textEdit->toPlainText().toUtf8();

    // SIGNING
    QString str = QString::fromUtf8(_dss->sign(m));

    ui->textEdit->setText(str);
}

void MainWindow::on_btn_verify_clicked()
{
    QByteArray m = ui->textEdit->toPlainText().toUtf8();

    bool isCorrect = _dss->verify(m);

    QMessageBox::information(this, "Verify", isCorrect ? "Підпис справжній" : "Підпис не є дійсний!", QMessageBox::Ok);
}
