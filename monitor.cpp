#include "monitor.h"
#include "algorithm"
#include "math.h"
#include <QRandomGenerator>
Monitor::Monitor():timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &Monitor::update);
    qRegisterMetaType<const QVector<double>&>();
    sensors.resize(8);
    for(auto& sensor:sensors) { //Заполняем данными все каналы сенсоров
        sensor.resize(1000);
        generateData(sensor, 0.5, 3);
    }
    t_buffer.resize(1000);
    y_fit_t.resize(1000);
    test_vector.resize(50);
    QVector<double> test;
    QVector<double> t_test(50);
    std::iota(std::begin(t_test), std::end(t_test), 0);
    QVector<double> y_test(50);
    test = sensors[ch1];
    test.resize(50);
    //qDebug() << leastSquares(test, t_test, y_test);
    test_vector = y_test;


    std::iota(std::begin(t_buffer), std::end(t_buffer), 0);
    qDebug() << leastSquares(sensors[ch1], t_buffer, y_fit_t);

    x_buffer.resize(50);
    buffer.resize(50);
    average_y_fit_buffer.resize(50);
    std::iota(std::begin(x_buffer), std::end(x_buffer), 0);

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
    QMutexLocker locker(&mutex);
    auto avg = channelSpeedAverage(sensors[ch1][index],
                                   sensors[ch2][index],
                                   sensors[ch3][index],
                                   sensors[ch4][index],
                                   sensors[ch5][index],
                                   sensors[ch6][index],
                                   sensors[ch7][index],
                                   sensors[ch8][index]);
    estimateDataTrend(avg);
//    emit sendAverage(avg);

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
        //filter(buffer);
        QVector<double> y_fit(50);
        avg_fit += leastSquares(buffer, x_buffer, y_fit);
        avg_fit /=2;
        y_fit_test.push_back(leastSquares(buffer, x_buffer, y_fit));
        qDebug() << avg_fit << "avg";
        emit sendVector(buffer, ch1);
        cnt = 0;
    }
    else cnt++;

}

void Monitor::generateData(QVector<double>& data, double variance, double grow_coefficient) //Генерация данных с шумом и трендом
{
    QVector<double> trend(data.size(), 0);
    double delta{0};
    for(auto& i:trend) {
        i += delta;
        delta += grow_coefficient/static_cast<double>(data.size());
        i -= 0.5;
    }
    for(auto& i:data) {
         i += QRandomGenerator::global()->generateDouble();
    }
    std::transform(std::begin(data), std::end(data), std::begin(trend), std::begin(data), std::plus<double>());
}

void Monitor::filter(QVector<double>& data)
{
    const size_t window_size{5};
    for(auto i = 0; i < data.size(); ++i) {
        data[i] = std::accumulate(std::begin(data)+i, std::begin(data)+i+window_size, 0.0)/window_size;
    }
}

void Monitor::start() const
{
    timer->start(20);
}

