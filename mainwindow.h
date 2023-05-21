#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "monitor.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Monitor* monitor_;
    Ui::MainWindow *ui;
    QCustomPlot* channels_plot_;
    void initPlot(QCustomPlot*);
    void generateData(QVector<double>&, double);
};
#endif // MAINWINDOW_H
