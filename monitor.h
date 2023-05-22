#ifndef MONITOR_H
#define MONITOR_H
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

constexpr size_t BUFFER_SIZE = 1000;
constexpr size_t CHANNELS = 8;

using sensor_channels = std::array<std::array<double, BUFFER_SIZE>, CHANNELS>;
using sample_of_channels = std::array<double, CHANNELS>;

enum channels {
    ch1,
    ch2,
    ch3,
    ch4,
    ch5,
    ch6,
    ch7,
    ch8
};

class Monitor: public QObject
{
    Q_OBJECT
private:
    QTimer* timer;
    sensor_channels sensors_;
    size_t index{0};
private slots:
    void update();
public slots:
    void start() const;
public:
    Monitor(const sensor_channels&);
    double channelSpeedAverage(double, double, double, double, double, double, double, double);
signals:
    emit void sendChannelDataToPlot(const std::array<double, CHANNELS>&);
};

Q_DECLARE_METATYPE(sensor_channels)
Q_DECLARE_METATYPE(sample_of_channels)

#endif // MONITOR_H
