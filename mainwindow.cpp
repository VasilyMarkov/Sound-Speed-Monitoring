#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), monitor_thread(new QThread())
{
    ui->setupUi(this); 
    setFixedHeight(1400);
    setFixedWidth(2000);
    channels = {ui->ch1, ui->ch2, ui->ch3, ui->ch4, ui->ch5, ui->ch6, ui->ch7, ui->ch8};
    ui->sim->setCheckable(true);
    ui->tests->addItems({"Проверка каналов", "Проверка ожидаемой скорости", "Проверка тренда"});
    channels_plot = ui->plot;
    initPlot(channels_plot);
    for(size_t i = 0; i < CHANNELS+1; ++i) {
        channels_plot->addGraph();
    }
    channels_plot->yAxis->setTickLabels(true);

    channels_plot->graph(ch1)->setPen(QPen(QColor(212, 62, 250), 2));
    channels_plot->graph(ch1)->setName("Channel 1");
    channels_plot->graph(ch2)->setPen(QPen(QColor(106, 62, 250), 2));
    channels_plot->graph(ch2)->setName("Channel 2");
    channels_plot->graph(ch3)->setPen(QPen(QColor(62, 119, 250), 2));
    channels_plot->graph(ch3)->setName("Channel 3");
    channels_plot->graph(ch4)->setPen(QPen(QColor(62, 250, 250), 2));
    channels_plot->graph(ch4)->setName("Channel 4");
    channels_plot->graph(ch5)->setPen(QPen(QColor(62, 250, 172), 2));
    channels_plot->graph(ch5)->setName("Channel 5");
    channels_plot->graph(ch6)->setPen(QPen(QColor(82, 247, 79), 2));
    channels_plot->graph(ch6)->setName("Channel 6");
    channels_plot->graph(ch7)->setPen(QPen(QColor(62, 250, 100), 2));
    channels_plot->graph(ch7)->setName("Channel 7");
    channels_plot->graph(ch8)->setPen(QPen(QColor(156, 250, 62), 2));
    channels_plot->graph(ch8)->setName("Channel 8");

    channels_plot->graph(expected)->setPen(QPen(QColor(210, 195, 224), 2));
    channels_plot->graph(expected)->setName("Real Time");

    line = new QCPItemLine(channels_plot);
    line->setPen(QPen(QColor(250, 250, 62), 1));

    line1 = new QCPItemLine(channels_plot);
    line1->setPen(QPen(QColor(240, 199, 50), 2, Qt::DotLine));
    line2 = new QCPItemLine(channels_plot);
    line2->setPen(QPen(QColor(240, 199, 50), 2, Qt::DotLine));
    line3 = new QCPItemLine(channels_plot);
    line3->setPen(QPen(QColor(219, 57, 57), 2, Qt::DotLine));
    line4 = new QCPItemLine(channels_plot);
    line4->setPen(QPen(QColor(219, 57, 57), 2, Qt::DotLine));

    line1->start->setCoords(0, -125);
    line1->end->setCoords(1000, -125);
    line2->start->setCoords(0, -135);
    line2->end->setCoords(1000, -135);

    line3->start->setCoords(0, -120);
    line3->end->setCoords(1000, -120);
    line4->start->setCoords(0, -140);
    line4->end->setCoords(1000, -140);



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

void MainWindow::receiveValue(double value)
{
    static size_t x = 0;
    channels_plot->graph(expected)->addData(x, value - 130);
    line->start->setCoords(x, -160);
    line->end->setCoords(x, 160);
    x++;
    channels_plot->replot();
}

void MainWindow::receiveVector(const QVector<double>& data, size_t channel)
{
    static size_t x = 0;
    const int offset = 105;
    for(auto i=0; i < data.size(); ++i) {
        channels_plot->graph(channel)->addData(x, data[i]-channel*30+offset);
        x++;
    }
    x = 0;
    channels_plot->replot();
}

void MainWindow::receiveEstimateTrend(bool trend)
{
    if(trend) ui->trend->setStyleSheet("QLabel { \
                                       border: 1px solid #8f8f8f; \
                                       border-radius: 2px; \
                                       background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F92424, stop: 1 #FA7878) }");
    else ui->trend->setStyleSheet("QLabel { \
                                  border: 1px solid #8f8f8f; \
                                  border-radius: 2px; \
                                  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #77FF77, stop: 1 #00DD00) }");
}

void MainWindow::receiveChannelFlags(const QVector<speedState>& channels_speed_state)
{
    assert(channels_speed_state.size() == channels.size());
    for(auto i=0; i < channels.size(); ++i) {
        switch (channels_speed_state[i]) {
            case speedState::normal:
                channels[i]->setStyleSheet("QLabel { \
                                           border: 1px solid #8f8f8f; \
                                           border-radius: 2px; \
                                           background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00DD00, stop: 1 #77FF77) }");
            break;
            case speedState::warning:
                channels[i]->setStyleSheet("QLabel { \
                                           border: 1px solid #8f8f8f; \
                                           border-radius: 2px; \
                                           background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F8D514, stop: 1 #FAE365) }");
            break;
            case speedState::critical:
                channels[i]->setStyleSheet("QLabel { \
                                           border: 1px solid #8f8f8f; \
                                           border-radius: 2px; \
                                           background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F92424, stop: 1 #FA7878) }");
            break;
        }
    }
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
    plot->yAxis->setRange(-150,150);
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

    plot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    plot->legend->setFont(legendFont);
}



void MainWindow::on_sim_clicked(bool checked)
{
    if(checked) {
        if(!monitor) {
            monitor = new Monitor();
            monitor->moveToThread(monitor_thread);
            connect(monitor_thread, &QThread::started, monitor, &Monitor::start);
            connect(monitor, &Monitor::sendValue, this, &MainWindow::receiveValue, Qt::QueuedConnection);
            connect(monitor, &Monitor::sendVector, this, &MainWindow::receiveVector, Qt::QueuedConnection);
            connect(monitor, &Monitor::sendEstimateTrend, this, &MainWindow::receiveEstimateTrend, Qt::QueuedConnection);
            connect(monitor, &Monitor::sendChannelFlags, this, &MainWindow::receiveChannelFlags, Qt::QueuedConnection);
            monitor_thread->start();
        }

        ui->sim->setStyleSheet("QPushButton { \
                               border: 1px solid #8f8f8f; \
                               border-radius: 2px; \
                               background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #77FF77, stop: 1 #00DD00) }");
    }
    else{
        ui->sim->setStyleSheet("");
    }
}

