#include "loss-estimator-time-window.hpp"
#include "core/logger.hpp"

namespace nfd {
namespace fw {

NFD_LOG_INIT("LossEstimator");

LossEstimatorTimeWindow::LossEstimatorTimeWindow(time::steady_clock::duration interestLifetime,
    time::steady_clock::duration windowSize) :
    m_interestLifetime(interestLifetime), m_windowSize(windowSize)
{
  if (m_windowSize <= m_interestLifetime) {
    throw std::runtime_error("Window size must be greater than interest lifetime!");
  }
}

void LossEstimatorTimeWindow::addSentInterest(const std::string name)
{
  const time::steady_clock::TimePoint now = time::steady_clock::now();

  auto n = unknownMap.insert(std::make_pair(now, name));
  if (n.second == false) {
    throw std::runtime_error("Duplicate insertion!");
    NFD_LOG_DEBUG("Duplicate insertion: " << name << " Should not happen!\n");
  }

}

void LossEstimatorTimeWindow::addSatisfiedInterest(const std::string name)
{
  bool found = false;

  for (auto n : unknownMap) {
    // Add new data
    if (n.second == name) {
      NFD_LOG_TRACE("Adding found interest!: " << name);
      found = true;
      lossMap.insert(std::make_pair(n.first, PacketType::FUTURESATISFIED));
      unknownMap.erase(n.first);
    }
  }
  if (found == false) {
    throw std::runtime_error("Interest not found! Should not happen!\n");
    NFD_LOG_DEBUG("Interest " << name << " not found! Should not happen!\n");
  }
}

double LossEstimatorTimeWindow::getLossPercentage()
{
  const time::steady_clock::TimePoint now = time::steady_clock::now();
  double perc;

  // Turning FUTURESATISFIED into SATISFIED (when the interest lifetime is exceeded)
  for (auto n : lossMap) {
    if (now > n.first + m_interestLifetime && n.second == PacketType::FUTURESATISFIED) {
      time::steady_clock::time_point temp = n.first;
      lossMap.erase(n.first);
      lossMap.insert(std::make_pair(temp, PacketType::SATISFIED));
    }
  }

  // Add lost interests
  for (auto n : unknownMap) {
    if (now > n.first + m_interestLifetime) {
      lossMap.insert(std::make_pair(n.first, PacketType::LOST));
      unknownMap.erase(n.first);
    }
  }

  // Remove packets that fall out of window size
  time::steady_clock::TimePoint lastValidInterests = now - m_windowSize;
  lossMap.erase(lossMap.begin(), lossMap.upper_bound(lastValidInterests));

  int satisfied = 0;
  int lost = 0;
  // Return 0 if the map is empty
  if (lossMap.size() == 0) {
    NFD_LOG_TRACE("LossMap empty!");
    perc = 0;
  }
  else {
    for (auto n : lossMap) {
      if (n.second == PacketType::SATISFIED) {
        satisfied++;
      }
      if (n.second == PacketType::LOST) {
        lost++;
      }
    }

    // Return 0 if only FUTURESATISFIED packets are in the map
    if (lost + satisfied == 0) {
      NFD_LOG_TRACE("Only FutureSatisfied!");
      perc = 0;
    }
    else {
      perc = (double) lost / (double) (lost + satisfied);
    }
  }

  NFD_LOG_TRACE("Loss Percentage: " << perc);
  return perc;
}

}  // namespace fw
}  // namespace nfd
