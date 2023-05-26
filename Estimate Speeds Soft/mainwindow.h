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
    void receiveValue(double);
    void receiveVector(const QVector<double>&, size_t);
    void receiveEstimateTrend(bool);
    void receiveChannelFlags(const QVector<my::speedState>&);
    void receiveExpectedSpeedState(my::speedState);
private slots:
    void on_sim_clicked(bool checked);
    void on_set_tresholds_ch_clicked();

    void on_set_tresholds_exp_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QLabel*> channels;
    Monitor* monitor;
    QThread* monitor_thread;
    QMutex* mutex;
    QCustomPlot* channels_plot;
    QCPItemLine* line;
    QCPItemLine* upper_warning_line;
    QCPItemLine* lower_warning_line;
    QCPItemLine* upper_critical_line;
    QCPItemLine* lower_critical_line;
    QCPItemLine* x_axis;
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
    emit void setTresholds(double, int);
};
#endif // MAINWINDOW_H
