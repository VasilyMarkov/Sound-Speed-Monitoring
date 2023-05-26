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
    void receiveChannelFlags(const QVector<my::speedState>&);
    void receiveExpectedSpeedState(my::speedState);
private slots:
    void on_sim_clicked(bool checked);
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

    size_t sample_index{0};
    void initPlot(QCustomPlot*);
    const QString normal_state{"{border: 1px solid #8f8f8f; \
                                border-radius: 2px; \
                                background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00DD00, stop: 1 #77FF77)}"};

    const QString warning_state{"{border: 1px solid #8f8f8f; \
                                 border-radius: 2px; \
                                 background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F8D514, stop: 1 #FAE365)}"};

    const QString crictical_state{"{border: 1px solid #8f8f8f; \
                                   border-radius: 2px; \
                                   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F92424, stop: 1 #FA7878)}"};
signals:
    emit void monitorStart(int);
    emit void monitorStop();
};
#endif // MAINWINDOW_H
