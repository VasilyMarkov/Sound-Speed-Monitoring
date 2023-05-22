#include "monitor.h"
#include "algorithm"

Monitor::Monitor(const sensor_channels& sensors):timer(new QTimer(this)), sensors_(sensors)
{
    connect(timer, &QTimer::timeout, this, &Monitor::update);
    qRegisterMetaType<sensor_channels>();
    qRegisterMetaType<sample_of_channels>();
}

double Monitor::channelSpeedAverage(double ch1,
                                    double ch2,
                                    double ch3,
                                    double ch4,
                                    double ch5,
                                    double ch6,
                                    double ch7,
                                    double ch8)
{
    std::array<double, 8> channels = {ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8};
    return std::accumulate(std::begin(channels), std::end(channels), 0.0)/8;
}

void Monitor::update()
{
    std::array<double, CHANNELS> buffer;
    for(size_t i = 0; i < CHANNELS; ++i) {
        buffer[i] = sensors_[i][index];
    }
    if(index < 1000)
        emit sendChannelDataToPlot(buffer);
    ++index;
}

void Monitor::start() const
{
    timer->start(100);
}

