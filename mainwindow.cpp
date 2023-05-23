#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), monitor_thread(new QThread())
{
    ui->setupUi(this); 
    setFixedHeight(1350);
    setFixedWidth(2100);
    channels_plot = ui->plot;
    initPlot(channels_plot);
    for(size_t i = 0; i < CHANNELS; ++i) {
        channels_plot->addGraph();
    }
    channels_plot->addGraph();
    channels_plot->yAxis->setTickLabels(true);
    //channels_plot->graph(ch1)->setPen(QPen(QColor(Qt::gray), 1.5));
    channels_plot->graph(ch1)->setPen(QPen(QColor(247, 79, 79), 1.5));
    channels_plot->graph(ch2)->setPen(QPen(QColor(247, 79, 200), 1.5));
    channels_plot->graph(ch3)->setPen(QPen(QColor(160, 79, 247), 1.5));
    channels_plot->graph(ch4)->setPen(QPen(QColor(79, 96, 247), 1.5));
    channels_plot->graph(ch5)->setPen(QPen(QColor(79, 191, 247), 1.5));
    channels_plot->graph(ch6)->setPen(QPen(QColor(82, 247, 79), 1.5));
    channels_plot->graph(ch7)->setPen(QPen(QColor(231, 247, 79), 1.5));
    channels_plot->graph(ch8)->setPen(QPen(QColor(247, 141, 79), 1.5));


    monitor = new Monitor();
    monitor->moveToThread(monitor_thread);
    connect(monitor_thread, &QThread::started, monitor, &Monitor::start);
    //connect(monitor, &Monitor::sendChannelDataToPlot, this, &MainWindow::receiveDataToPlot, Qt::QueuedConnection);
    connect(monitor, &Monitor::sendAverage, this, &MainWindow::receiveAverageData, Qt::QueuedConnection);
    connect(monitor, &Monitor::sendVector, this, &MainWindow::receiveVector, Qt::QueuedConnection);
    monitor_thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveDataToPlot(const QVector<double>& data)
{


//    static size_t index = 0;
//    static size_t global_x = 0;
//    QVector<double> buffer;
//    buffer[index] = data;
//    const auto offset = 14;
//    if(index == 9) {
//        for(auto& element:buffer) {
//            for(size_t i=0; i < CHANNELS; ++i) {
//                channels_plot->graph(i)->addData(global_x, element[i]+offset-i*4);
//            }
//            global_x++;
//        }
//        index = 0;
//    }
//    ++index;
//    channels_plot->replot();
//    qDebug() << channels_plot->replotTime(true);
}

void MainWindow::receiveAverageData(double value)
{
//    static size_t x = 0;
//    channels_plot->graph(ch1)->addData(x++, value);
//    channels_plot->replot();
}

void MainWindow::receiveVector(const QVector<double>& data, size_t channel)
{
    static size_t x = 0;
    for(size_t i=0; i < data.size(); ++i) {
        channels_plot->graph(channel)->addData(x, data[i]);
        x++;
    }
    channels_plot->replot();
}

void MainWindow::initPlot(QCustomPlot* plot) {
    plot->xAxis->setBasePen(QPen(Qt::white, 1));
    plot->yAxis->setBasePen(QPen(Qt::white, 1));
    plot->xAxis->setTickPen(QPen(Qt::white, 1));
    plot->yAxis->setTickPen(QPen(Qt::white, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridVisible(true);
    plot->yAxis->grid()->setSubGridVisible(true);
    plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    plot->yAxis->setRange(-5,5);
    plot->xAxis->setRange(0,BUFFER_SIZE);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    plot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    plot->axisRect()->setBackground(axisRectGradient);
}


