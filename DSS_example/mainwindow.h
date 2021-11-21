#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DSS;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setMessage(const QByteArray &newMessage);

private slots:
    void on_btn_encode_clicked();
    void on_btn_verify_clicked();

private:
    Ui::MainWindow *ui;

    DSS *_dss;
};
#endif // MAINWINDOW_H
