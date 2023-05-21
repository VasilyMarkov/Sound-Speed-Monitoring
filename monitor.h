#ifndef MONITOR_H
#define MONITOR_H
#include <QObject>

class Monitor: public QObject
{
    Q_OBJECT
public:
    Monitor();
    double channelSpeedAverage(double, double, double, double, double, double, double, double);
};

#endif // MONITOR_H
