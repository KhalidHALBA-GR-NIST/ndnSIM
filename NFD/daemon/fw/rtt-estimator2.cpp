#include "rtt-estimator2.hpp"

namespace nfd {
namespace fw {

RttEstimator2::RttEstimator2(time::microseconds initialRtt, double gain) :
    rttInMicroSec(initialRtt.count()), gain(gain), sampleCount(0)
{
}

void RttEstimator2::addMeasurement(time::microseconds measure)
{
  double m = static_cast<double>(measure.count());
  if (sampleCount > 0) {
    double err = m - rttInMicroSec;
    double gErr = err * gain;
    rttInMicroSec += gErr;
  }
  else {
    rttInMicroSec = m;
  }
  sampleCount++;
}

double RttEstimator2::getRttInMilliseconds() const
{
  return rttInMicroSec / (double) 1000.0;
}

}  // namespace fw
}  // namespace nfd
