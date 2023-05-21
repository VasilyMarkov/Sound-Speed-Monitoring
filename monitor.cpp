#include "monitor.h"
#include "algorithm"
Monitor::Monitor()
{

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
