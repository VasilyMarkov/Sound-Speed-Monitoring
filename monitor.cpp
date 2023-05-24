#include "monitor.h"
#include "algorithm"
#include "math.h"
#include <QRandomGenerator>
Monitor::Monitor():timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &Monitor::update);
    qRegisterMetaType<const QVector<double>&>();
    sensorChannelsInput.resize(8);
    for(auto& sensor:sensorChannelsInput) { //Заполняем данными все каналы сенсоров
        sensor.resize(BUFFER_SIZE);
        //sensor.fill(600);
        generateData(sensor, 0.5);
    }
    sensorChannelsInput[ch1][50] = 10;
    sensorChannelsInput[ch1][100] = -10;
    expectedSpeedData.resize(BUFFER_SIZE);
    expectedSpeedData.fill(EXPECTED_SPEED);
    generateData(expectedSpeedData, 0.5);
    //generateTrend(sensors[ch1], 10, 0, BUFFER_SIZE-1);

    x_buffer.resize(50);
    buffer.resize(50);
    std::iota(std::begin(x_buffer), std::end(x_buffer), 0);

    channels_buffer.resize(8);
    median_filter.resize(7);
    median_filter.fill(10);
}

void Monitor::update()
{
    QMutexLocker locker(&mutex);
    QVector<double> input_data = {sensorChannelsInput[ch1][index],
                                  sensorChannelsInput[ch2][index],
                                  sensorChannelsInput[ch3][index],
                                  sensorChannelsInput[ch4][index],
                                  sensorChannelsInput[ch5][index],
                                  sensorChannelsInput[ch6][index],
                                  sensorChannelsInput[ch7][index],
                                  sensorChannelsInput[ch8][index]};
    expectedSpeedInput = expectedSpeedData[index];
    average_channels = std::accumulate(std::begin(input_data), std::end(input_data), 0.0)/CHANNELS;
    emit sendValue(speedEst(input_data[ch1]));
    //estimateChannelsSpeed(sensors_buffer);


    //estimateDataTrend(average_channels);
    //emit sendValue(average_channels);
    if(index == 1) {
        for(auto i = 0; i < sensorChannelsInput.size(); ++i) {
            emit sendVector(sensorChannelsInput[i], i);
        }
    }
    if(index == 999) {
        qDebug() << std::accumulate(std::begin(y_fit_test), std::end(y_fit_test), 0.0)/y_fit_test.size();
        timer->stop();
    }

    ++index;
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
    buffer[cnt] = value;
    if(cnt == 49) {
        filter(buffer, 9);
        QVector<double> y_fit(50);
        avg_fit = leastSquares(buffer, x_buffer, y_fit);
        y_fit_test.push_back(leastSquares(buffer, x_buffer, y_fit));
        qDebug() << avg_fit << "avg";
        bool trend_detection;
        if(std::abs(avg_fit) > trend_treshold) trend_detection = true;
        else trend_detection = false;
        emit sendEstimateTrend(trend_detection);
        emit sendVector(buffer, ch1);
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
    }
}

void Monitor::filter(QVector<double>& data, size_t window)
{
    assert(window > 0 && !data.empty());

    for(size_t i = 0; i < data.size()-window; ++i) {
        data[i] = std::accumulate(std::begin(data)+i, std::begin(data)+i+window, 0.0)/window;
    }
    for(size_t i = data.size()-1; i > data.size()-window; --i) {
        data[i] = std::accumulate(std::begin(data)+i-window, std::begin(data)+i, 0.0)/window;
    }
}

void Monitor::estimateChannelsSpeed(QVector<double>& channels)
{
    assert(channels.size() != CHANNELS);

    static size_t cnt = 0;
    const size_t median_filter_size = 7;
    channels_buffer.push_back(channels);
    QVector<double> buffer(median_filter_size);
    if(cnt == median_filter_size-1) {
        for(auto i = 0; i < median_filter_size; ++i) {

        }
    }
    cnt++;
}

double Monitor::speedEst(double value)
{
    static size_t cnt = 0;
    const size_t median_filter_size = 7;
    median_filter[cnt] = value;
    std::sort(std::begin(median_filter), std::end(median_filter));
    cnt++;
    if(cnt == median_filter_size-1) {
        cnt = 0;
    }
    auto out = median_filter[median_filter.size()/2];
    return out;
}

void Monitor::start() const
{
    timer->start(20);
}

