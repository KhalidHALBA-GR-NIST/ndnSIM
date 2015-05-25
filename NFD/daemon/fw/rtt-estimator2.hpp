#ifndef NFD_DAEMON_FW_RTT_ESTIMATOR2_HPP
#define NFD_DAEMON_FW_RTT_ESTIMATOR2_HPP

#include "common.hpp"

namespace nfd {
namespace fw {

/**
 * \brief A simple RTT Estimator without rto calculation
 *
 * Uses an exponential moving average.
 *
 * reference: ns3::RttMeanDeviation
 *
 */
class RttEstimator2
{
public:

  /**
   * Sets the initial rtt to 10 ms and the gain to 0.1.
   */
  RttEstimator2(time::microseconds initialRtt = time::milliseconds(10), double gain = 0.1);

  /**
   * Adds one new rtt measurement.
   */
  void
  addMeasurement(time::microseconds measure);

  double
  getRttInMilliseconds() const;

private:

  double rttInMicroSec;
  double gain;
  uint32_t sampleCount;
};

}  // namespace fw
}  // namespace nfd

#endif // NFD_DAEMON_FW_RTT_ESTIMATOR_NEW_HPP
