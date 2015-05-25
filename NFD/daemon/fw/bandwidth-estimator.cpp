#include "bandwidth-estimator.hpp"

namespace nfd {
namespace fw {

BandwidthEstimator::BandwidthEstimator(time::steady_clock::duration window) :
    windowSize(window)
{
}

void BandwidthEstimator::addPacket(size_t sizeInBytes)
{
  time::steady_clock::TimePoint now = time::steady_clock::now();
  bwMap[now] = sizeInBytes;
}

double BandwidthEstimator::getKBytesPerSecond()
{
  time::steady_clock::time_point now = time::steady_clock::now();

  // Remove too early data packets
  time::steady_clock::TimePoint lastValidInterests = now - windowSize;
  bwMap.erase(bwMap.begin(), bwMap.upper_bound(lastValidInterests));

  // Return 0 if no packets are inside the sliding window
  if (bwMap.size() == 0) {
    return 0;
  }
  // Else return the data rate
  else {
    size_t totalSize = 0;
    for (auto n : bwMap) {
      totalSize += n.second;
    }
    double windowSeconds = windowSize.count() / 1000000000.0;
    double kiloBytesPerSec = (double) totalSize / ((double) windowSeconds * 1024);

    return kiloBytesPerSec;
  }
}

}  // namespace fw
}  // namespace nfd
