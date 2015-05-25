#ifndef NFD_DAEMON_FW_BANDWIDTH_ESTIMATOR_HPP
#define NFD_DAEMON_FW_BANDWIDTH_ESTIMATOR_HPP

#include "common.hpp"
#include <boost/chrono/duration.hpp>
#include <ndn-cxx/util/time.hpp>
#include <map>

namespace nfd {
namespace fw {

/**
 * \brief Implements a bandwidth estimator using a simple moving average.
 *
 * The average bandwidth is calculated over a sliding window of x time units.
 *
 */
class BandwidthEstimator
{
public:

  /**
   * Constructs the bandwidth estimator and sets the sliding window size
   */
  BandwidthEstimator(time::steady_clock::duration window);

  /**
   * Adds one interest or data packet to the bandwidth calculation.
   */
  void
  addPacket(size_t sizeInBytes);

  /**
   * Returns the bandwidth over the sliding window in kilobytes per second (1 KB = 1024 bytes).
   * Returns 0 if there were no data packets inside the time window.
   */
  double
  getKBytesPerSecond();

private:

  // The sliding window size
  const time::steady_clock::duration windowSize;

  // A map of time stamps and corresponding size of packets
  std::map<time::steady_clock::TimePoint, size_t> bwMap;

};

}  // namespace fw
}  // namespace nfd

#endif // NFD_DAEMON_FW_BANDWIDTH_ESTIMATOR_HPP
