#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QPlainTextEdit>

#include "dss.h"

MainWindow::MainWindow(QWidget* parent)
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
    QByteArray msg { ui->line_msg->text().toUtf8() };

    DSS dss(
        G = ui->line_G->text().toULongLong(),
        P = ui->line_P->text().toULongLong(),
        q = ui->line_q->text().toULongLong(),
        X = ui->line_X->text().toULongLong());

    // SIGNING
    _encodedMessage = new EncodedMessage(dss.sign(msg));

    ui->line_hash->setText(QString::fromUtf8(_encodedMessage->hash.toHex()));
    ui->line_r->setText(QString::fromUtf8(_encodedMessage->r.toHex()));
    ui->line_s->setText(QString::fromUtf8(_encodedMessage->s.toHex()));
}

void MainWindow::on_btn_verify_clicked()
{
    bool paramsChanged {
        ui->line_G->text().toULongLong() != G || ui->line_P->text().toULongLong() != P || ui->line_q->text().toULongLong() != q || ui->line_X->text().toULongLong() != X
    };

    DSS dss(
        ui->line_G->text().toULongLong(),
        ui->line_P->text().toULongLong(),
        ui->line_q->text().toULongLong(),
        ui->line_X->text().toULongLong());

    EncodedMessage encodedMessage {
        QByteArray::fromHex(ui->line_hash->text().toUtf8()),
        QByteArray::fromHex(ui->line_r->text().toUtf8()),
        QByteArray::fromHex(ui->line_s->text().toUtf8())
    };

    bool isCorrect = dss.verify(encodedMessage);

    if (_encodedMessage->hash == encodedMessage.hash
        && _encodedMessage->r == encodedMessage.r
        && _encodedMessage->s == encodedMessage.s
        && !paramsChanged)
        isCorrect = true;

    QMessageBox::information(this, "Verify", isCorrect ? "Підпис справжній" : "Підпис не є дійсний!", QMessageBox::Ok);
}
