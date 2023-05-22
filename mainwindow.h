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
public slots:
    void receiveDataToPlot(const std::array<double, CHANNELS>&);
private:
    Ui::MainWindow *ui;
    Monitor* monitor;
    QThread* monitor_thread;
    QCustomPlot* channels_plot;
    sensor_channels sensors;
    void initPlot(QCustomPlot*);
    void generateData(std::array<double, BUFFER_SIZE>&, double, double);
};
#endif // MAINWINDOW_H
