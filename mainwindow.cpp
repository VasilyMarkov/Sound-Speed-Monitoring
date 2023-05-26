#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), monitor_thread(new QThread())
{
    ui->setupUi(this); 
    setFixedHeight(700);
    setFixedWidth(1000);
    using namespace my;
    monitor = new Monitor();
    monitor->moveToThread(monitor_thread);
    connect(this, &MainWindow::monitorStart, monitor, &Monitor::start);
    connect(this, &MainWindow::monitorStop, monitor, &Monitor::stop);
    connect(this, &MainWindow::setTresholds, monitor, &Monitor::setTresholds);
    connect(monitor, &Monitor::sendValue, this, &MainWindow::receiveValue, Qt::QueuedConnection);
    connect(monitor, &Monitor::sendVector, this, &MainWindow::receiveVector, Qt::QueuedConnection);
    connect(monitor, &Monitor::sendEstimateTrend, this, &MainWindow::receiveEstimateTrend, Qt::QueuedConnection);
    connect(monitor, &Monitor::sendChannelFlags, this, &MainWindow::receiveChannelFlags, Qt::QueuedConnection);
    connect(monitor, &Monitor::sendExpectedSpeedState, this, &MainWindow::receiveExpectedSpeedState, Qt::QueuedConnection);
    monitor_thread->start();

    channels = {ui->ch1, ui->ch2, ui->ch3, ui->ch4, ui->ch5, ui->ch6, ui->ch7, ui->ch8};
    ui->sim->setCheckable(true);
    ui->tests->addItems({"Проверка каналов", "Проверка ожидаемой скорости", "Проверка тренда"});
    ui->warning_ch->setText(QString::number(5));
    ui->critical_ch->setText(QString::number(10));
    ui->warning_exp->setText(QString::number(5));
    ui->critical_exp->setText(QString::number(10));

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

    channels_plot->graph(real_time)->setPen(QPen(QColor(116, 123, 212), 2));
    channels_plot->graph(real_time)->setName("Real Time");

    line = new QCPItemLine(channels_plot);
    line->setPen(QPen(QColor(250, 250, 62), 0.5));

    upper_warning_line = new QCPItemLine(channels_plot);
    upper_warning_line->setPen(QPen(QColor(240, 199, 50), 1.5, Qt::DotLine));
    lower_warning_line = new QCPItemLine(channels_plot);
    lower_warning_line->setPen(QPen(QColor(240, 199, 50), 1.5, Qt::DotLine));
    upper_critical_line = new QCPItemLine(channels_plot);
    upper_critical_line->setPen(QPen(QColor(219, 57, 57), 1.5, Qt::DotLine));
    lower_critical_line = new QCPItemLine(channels_plot);
    lower_critical_line->setPen(QPen(QColor(219, 57, 57), 1.5, Qt::DotLine));

    upper_warning_line->start->setCoords(0, -125);
    upper_warning_line->end->setCoords(1000, -125);
    lower_warning_line->start->setCoords(0, -135);
    lower_warning_line->end->setCoords(1000, -135);

    upper_critical_line->start->setCoords(0, -120);
    upper_critical_line->end->setCoords(1000, -120);
    lower_critical_line->start->setCoords(0, -140);
    lower_critical_line->end->setCoords(1000, -140);

    x_axis = new QCPItemLine(channels_plot);
    x_axis->setPen(QPen(QColor(Qt::white), 1));
    x_axis->start->setCoords(0, -130);
    x_axis->end->setCoords(1000, -130);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sim_clicked(bool checked)
{
    if(checked) {
        emit monitorStart(ui->tests->currentIndex());
        for(size_t i = 0; i < CHANNELS+1; ++i) {
            channels_plot->graph(i)->data()->clear();
        }
        sample_index = 0;
        ui->sim->setText("Остановить симуляцию");
        ui->sim->setStyleSheet(QString("QPushButton")+normal_state);

    }
    else{
        emit monitorStop();
        ui->sim->setText("Начать симуляцию");
        ui->sim->setStyleSheet("");
    }
}

void MainWindow::receiveValue(double value)
{
    channels_plot->graph(my::real_time)->addData(sample_index, value - 130);
    line->start->setCoords(sample_index, -160);
    line->end->setCoords(sample_index, 160);
    sample_index++;
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
    if(trend) ui->trend->setStyleSheet(QString("QLabel")+crictical_state);
    else ui->trend->setStyleSheet(QString("QLabel")+normal_state);
}

void MainWindow::receiveChannelFlags(const QVector<my::speedState>& channels_speed_state)
{
    assert(channels_speed_state.size() == channels.size());
    for(auto i=0; i < channels.size(); ++i) {
        switch (channels_speed_state[i]) {
            case my::speedState::normal:
                channels[i]->setStyleSheet(QString("QLabel")+normal_state);
            break;
            case my::speedState::warning:
                channels[i]->setStyleSheet(QString("QLabel")+warning_state);
            break;
            case my::speedState::critical:
                channels[i]->setStyleSheet(QString("QLabel")+crictical_state);
            break;
        }
    }
}

void MainWindow::receiveExpectedSpeedState(my::speedState state)
{
    switch (state) {
        case my::speedState::normal:
            ui->expected_speed->setStyleSheet(QString("QLabel")+normal_state);
        break;
        case my::speedState::warning:
            ui->expected_speed->setStyleSheet(QString("QLabel")+warning_state);
        break;
        case my::speedState::critical:
            ui->expected_speed->setStyleSheet(QString("QLabel")+crictical_state);
        break;
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

void MainWindow::on_set_tresholds_ch_clicked()
{
    auto warn_treshold =  std::abs(ui->warning_ch->text().toDouble());
    auto crit_treshold =  std::abs(ui->critical_ch->text().toDouble());
    if(!ui->warning_ch->text().isEmpty()) {
        upper_warning_line->start->setCoords(0, -130+warn_treshold);
        upper_warning_line->end->setCoords(1000, -130+warn_treshold);
        lower_warning_line->start->setCoords(0, -130-warn_treshold);
        lower_warning_line->end->setCoords(1000, -130-warn_treshold);
        emit setTresholds(warn_treshold, my::typeTreshold::warning_channel);
    }
    if(!ui->critical_ch->text().isEmpty()) {
        upper_critical_line->start->setCoords(0, -130+crit_treshold);
        upper_critical_line->end->setCoords(1000, -130+crit_treshold);
        lower_critical_line->start->setCoords(0, -130-crit_treshold);
        lower_critical_line->end->setCoords(1000, -130-crit_treshold);
        emit setTresholds(crit_treshold, my::typeTreshold::critical_channel);
    }
    channels_plot->replot();
}

void MainWindow::on_set_tresholds_exp_clicked()
{
    auto warn_treshold =  std::abs(ui->warning_exp->text().toDouble());
    auto crit_treshold =  std::abs(ui->critical_exp->text().toDouble());
    if(!ui->warning_exp->text().isEmpty()) {
        upper_warning_line->start->setCoords(0, -130+warn_treshold);
        upper_warning_line->end->setCoords(1000, -130+warn_treshold);
        lower_warning_line->start->setCoords(0, -130-warn_treshold);
        lower_warning_line->end->setCoords(1000, -130-warn_treshold);
        emit setTresholds(warn_treshold, my::typeTreshold::warning_expected);
    }

    if(!ui->critical_exp->text().isEmpty()) {
        upper_critical_line->start->setCoords(0, -130+crit_treshold);
        upper_critical_line->end->setCoords(1000, -130+crit_treshold);
        lower_critical_line->start->setCoords(0, -130-crit_treshold);
        lower_critical_line->end->setCoords(1000, -130-crit_treshold);
        emit setTresholds(crit_treshold, my::typeTreshold::critical_expected);
    }
    channels_plot->replot();
}

