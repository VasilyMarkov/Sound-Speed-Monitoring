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
    QMutex mutex;
    QVector<QVector<double>> sensors;
    std::array<double, BUFFER_SIZE> average_speed_of_sound_;
    size_t index{0};
    QVector<double> test_vector;
    QVector<double> x_buffer;
    QVector<double> t_buffer;
    QVector<double> y_fit_t;
    QVector<double> buffer;
    QVector<double> average_y_fit_buffer;
    QVector<double> y_fit_test;
    double avg_fit{0};
    const double trend_treshold{0.001};
    void generateData(QVector<double>&, double, double);
    bool estimateDataTrend(double);
    double leastSquares(const QVector<double>&,const QVector<double>&,QVector<double>&);
    void filter(QVector<double>&);
private slots:
    void update();
public slots:
    void start() const;
public:
    Monitor();
    double channelSpeedAverage(double, double, double, double, double, double, double, double);
signals:
    emit void sendChannelDataToPlot(double);
    emit void sendAverage(double);
    emit void sendVector(const QVector<double>&, size_t);
};

Q_DECLARE_METATYPE(QVector<QVector<double>>)

#endif // MONITOR_H
