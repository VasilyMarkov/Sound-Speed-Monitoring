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
    void receiveValue(double);
    void receiveVector(const QVector<double>&, size_t);
    void receiveEstimateTrend(bool);
    void receiveChannelFlags(const QVector<speedState>&);
private:
    Ui::MainWindow *ui;
    QVector<QLabel*> channels;
    Monitor* monitor;
    QThread* monitor_thread;
    QMutex* mutex;
    QCustomPlot* channels_plot;
    QCPItemLine* line;
    QCPItemLine* line1;
    QCPItemLine* line2;
    QCPItemLine* line3;
    QCPItemLine* line4;
    void initPlot(QCustomPlot*);
};
#endif // MAINWINDOW_H
