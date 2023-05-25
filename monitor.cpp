#include "monitor.h"
#include "algorithm"
#include "math.h"
#include <QRandomGenerator>
Monitor::Monitor(typeSimulation typeSimulation_):timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &Monitor::update);
    qRegisterMetaType<QVector<double>>();
    qRegisterMetaType<QVector<speedState>>();

    switch (typeSimulation_) {
        case typeSimulation::channelsSpeed:
            generateSim1Data();
        break;
        case typeSimulation::expectedSpeed:
            generateSim2Data();
        break;
        case typeSimulation::trend:
            generateSim3Data();
        break;
        default:
        break;
    }

    x_buffer.resize(50);
    buffer.resize(50);
    std::iota(std::begin(x_buffer), std::end(x_buffer), 0);

    channels_buffer.resize(CHANNELS);
    median_filter.resize(CHANNELS);
    for(auto& channel_filter:median_filter) {
        channel_filter.resize(3);
    }
    channels_flags.resize(CHANNELS);
}

void Monitor::generateSim1Data()
{
    sensorChannelsInput.resize(8);
    for(auto& sensor:sensorChannelsInput) {
        sensor.resize(BUFFER_SIZE);
        generateData(sensor, 0.5);
    }
    generateTrend(sensorChannelsInput[ch2], 15, 0, 300);
    generateTrend(sensorChannelsInput[ch2], -15, 301, 600);
    generateTrend(sensorChannelsInput[ch4], 10, 0, BUFFER_SIZE);
    sensorChannelsInput[ch2][50] = 7;
    sensorChannelsInput[ch2][100] = -7;
    generateTrend(sensorChannelsInput[ch5], 10, 200, 400);
    generateTrend(sensorChannelsInput[ch5], -10, 401, 600);
    sensorChannelsInput[ch5][200] = 10;
}

void Monitor::generateSim2Data()
{
    sensorChannelsInput.resize(8);
    for(auto& sensor:sensorChannelsInput) {
        sensor.resize(BUFFER_SIZE);
        generateData(sensor, 0.5);
        generateTrend(sensor[ch4], 10, 0, BUFFER_SIZE);
    }
}

void Monitor::generateSim3Data()
{
    sensorChannelsInput.resize(8);
    for(auto& sensor:sensorChannelsInput) {
        sensor.resize(BUFFER_SIZE);
        generateData(sensor, 0.5);
    }
    expectedSpeedData.resize(BUFFER_SIZE);
    expectedSpeedData.fill(EXPECTED_SPEED);
    generateData(expectedSpeedData, 0.5);
}

void Monitor::update()
{
    if(index == 1000) timer->stop();

    QMutexLocker locker(&mutex);
    expectedSpeedInput = expectedSpeedData[index];
    QVector<double> filtered_data = {medianFilter(sensorChannelsInput[ch1][index], ch1),
                                     medianFilter(sensorChannelsInput[ch2][index], ch2),
                                     medianFilter(sensorChannelsInput[ch3][index], ch3),
                                     medianFilter(sensorChannelsInput[ch4][index], ch4),
                                     medianFilter(sensorChannelsInput[ch5][index], ch5),
                                     medianFilter(sensorChannelsInput[ch6][index], ch6),
                                     medianFilter(sensorChannelsInput[ch7][index], ch7),
                                     medianFilter(sensorChannelsInput[ch8][index], ch8)};

    average_channels = std::accumulate(std::begin(filtered_data), std::end(filtered_data), 0.0)/CHANNELS;
    if(index == 1) {
        for(auto i = 0; i < sensorChannelsInput.size(); ++i) {
            emit sendVector(sensorChannelsInput[i], i);
        }
    }

    switch (typeSimulation_) {
        case typeSimulation::channelsSpeed:
            emit sendValue(filtered_data[ch2]);
        break;
        case typeSimulation::expectedSpeed:

        break;
        case typeSimulation::trend:
            estimateDataTrend(average_channels);
        break;
        default:
        break;
    }

    estimateChannelsSpeed(filtered_data, average_channels);
    emit sendChannelFlags(channels_flags);

    ++index;
}

void Monitor::stop()
{
    timer->stop();
}

double Monitor::leastSquares(const QVector<double>& y, const QVector<double>& x, QVector<double>& y_fit_o) {
    size_t n = x.size();
    double x_sum{0}, y_sum{0}, x2_sum{0}, xy_sum{0};
    for (auto i = 0; i < y.size(); ++i)
    {
        x_sum+=x[i];
        y_sum+=y[i];
        x2_sum+=std::pow(x[i],2);
        xy_sum+=x[i]*y[i];
    }
    double a=(n*xy_sum-x_sum*y_sum)/(n*x2_sum-x_sum*x_sum);
    double b=(x2_sum*y_sum-x_sum*xy_sum)/(x2_sum*n-x_sum*x_sum);
    for (auto i = 0; i < y_fit_o.size(); i++)
        y_fit_o[i]=a*x[i]+b;
    return a;
}

bool Monitor::estimateDataTrend(double value) {
    static size_t cnt{0};
    static double test{0};
    buffer[cnt] = value;
    if(cnt == 49) {
        QVector<double> y_fit(50);
        avg_fit = leastSquares(buffer, x_buffer, y_fit);
        qDebug() << avg_fit;
        bool trend_detection;
        if(std::abs(avg_fit) > trend_treshold) trend_detection = true;
        else trend_detection = false;
        emit sendEstimateTrend(trend_detection);
        cnt = 0;
    }
    else cnt++;
}

void Monitor::generateTrend(QVector<double>& data, double grow_coefficient, size_t begin, size_t end)
{
    assert(begin <= end && end < BUFFER_SIZE);

    QVector<double> trend(end-begin, 0);
    double delta{0};
    for(auto& i:trend) {
        i += delta;
        delta += grow_coefficient/static_cast<double>(end-begin);
        i -= 0.5;
    }
    std::transform(std::begin(data)+begin, std::begin(data)+end, std::begin(trend), std::begin(data)+begin, std::plus<double>());
}

void Monitor::generateData(QVector<double>& data, double variance)
{
    for(auto& i:data) {
         i += QRandomGenerator::global()->generateDouble();
         i -= 0.5;
    }
}

void Monitor::estimateChannelsSpeed(QVector<double>& channelsSpeed, double expectedSpeed)
{
    for(auto i = 0; i < channelsSpeed.size(); ++i) {
        if(std::abs(channelsSpeed[i]-expectedSpeed) > 10) {
            channels_flags[i] = speedState::critical;
        }
        else if(std::abs(channelsSpeed[i]-expectedSpeed) > 5) {
            channels_flags[i] = speedState::warning;
        }
        else
            channels_flags[i] = speedState::normal;
    }
}

double Monitor::medianFilter(double value, size_t channel)
{
    static QVector<size_t> counts(CHANNELS, 0);
    const size_t median_filter_size = 3;
    median_filter[channel][counts[channel]] = value;
    std::sort(std::begin(median_filter[channel]), std::end(median_filter[channel]));
    counts[channel]++;
    if(counts[channel] == median_filter_size-1) {
        counts.fill(0);
    }
    auto out = median_filter[channel][median_filter[channel].size()/2];
    return out;
}

void Monitor::start() const
{
    timer->start(100);
}

