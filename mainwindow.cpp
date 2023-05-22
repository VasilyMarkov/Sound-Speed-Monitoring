#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), monitor_thread(new QThread())
{
    ui->setupUi(this); 
    channels_plot = ui->plot;
    initPlot(channels_plot);
    for(size_t i = 0; i < CHANNELS; ++i) {
        channels_plot->addGraph();
    }
    channels_plot->yAxis->setTickLabels(true);
    channels_plot->graph(ch1)->setPen(QPen(QColor(247, 79, 79), 1.5));
    channels_plot->graph(ch2)->setPen(QPen(QColor(247, 79, 200), 1.5));
    channels_plot->graph(ch3)->setPen(QPen(QColor(160, 79, 247), 1.5));
    channels_plot->graph(ch4)->setPen(QPen(QColor(79, 96, 247), 1.5));
    channels_plot->graph(ch5)->setPen(QPen(QColor(79, 191, 247), 1.5));
    channels_plot->graph(ch6)->setPen(QPen(QColor(82, 247, 79), 1.5));
    channels_plot->graph(ch7)->setPen(QPen(QColor(231, 247, 79), 1.5));
    channels_plot->graph(ch8)->setPen(QPen(QColor(247, 141, 79), 1.5));

    for(auto& sensor:sensors) { //Заполняем данными все каналы сенсоров
       generateData(sensor, 0.5, 0);
    }
    monitor = new Monitor(sensors);
    monitor->moveToThread(monitor_thread);
    connect(monitor_thread, &QThread::started, monitor, &Monitor::start);
    connect(monitor, &Monitor::sendChannelDataToPlot, this, &MainWindow::receiveDataToPlot, Qt::QueuedConnection);
    monitor_thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveDataToPlot(const std::array<double, CHANNELS>& data)
{
    static size_t x = 0;
    auto offset = 18;
    for(size_t i=0; i < CHANNELS; ++i) {
        channels_plot->graph(i)->addData(x, data[i]+offset-i*4);
    }
    ++x;
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
    plot->yAxis->setRange(-20,20);
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

void MainWindow::generateData(std::array<double, BUFFER_SIZE>& data, double variance, double grow_coefficient) //Генерация данных с шумом и трендом
{
    QVector<double> trend(data.size(), 0);
    double delta{0};
    for(auto& i:trend) {
        i += delta;
        delta += grow_coefficient/static_cast<double>(data.size());
    }
    std::mt19937 engine{};
    std::normal_distribution<> distr(0,variance);

    for(auto& i:data) {
         i += distr(engine);
    }
    std::transform(std::begin(data), std::end(data), std::begin(trend), std::begin(data), std::plus<double>());
}

