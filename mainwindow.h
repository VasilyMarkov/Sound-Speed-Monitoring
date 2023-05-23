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
    void receiveDataToPlot(const QVector<double>&);
    void receiveAverageData(double);
    void receiveVector(const QVector<double>&, size_t);
    void receiveEstimateTrend(bool);
private:
    Ui::MainWindow *ui;
    QVector<QLabel*> channels;
    Monitor* monitor;
    QThread* monitor_thread;
    QMutex* mutex;
    QCustomPlot* channels_plot;
    QCPItemLine* line;
    void initPlot(QCustomPlot*);
};
#endif // MAINWINDOW_H
