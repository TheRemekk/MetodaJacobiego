#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern "C" {
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_calcBtn_clicked();
    void Jacobi();
    void Jacobi_fl_point();
    void Jacobi_interval();
    void resetCalculations();

signals:
    void calculationsFinished();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
