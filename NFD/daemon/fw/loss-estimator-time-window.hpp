#ifndef NFD_DAEMON_FW_LOSS_ESTIMATOR_TIME_WINDOW_HPP
#define NFD_DAEMON_FW_LOSS_ESTIMATOR_TIME_WINDOW_HPP

#include "common.hpp"
#include "loss-estimator.hpp"

namespace nfd {
namespace fw {

/**
 * \brief Implements the loss estimation with a sliding window over the last x time units.
 *
 * The loss percentage is calculated with all packets of status LOST or SATISFIED during the
 * sliding window.
 */
class LossEstimatorTimeWindow : public LossEstimator
{
public:

  /**
   * Constructs the Loss Estimator with the given sliding window time and interest lifetime.
   * Throws a runtime exception if the interest lifetime is larger than the loss window!
   */
  LossEstimatorTimeWindow(time::steady_clock::duration interestLifetime,
      time::steady_clock::duration lossWindow);

  /*
   * Adds an interest to the unknownMap.
   * Throws a runtime exception if the name already exists!
   */
  void addSentInterest(const std::string name)
  DECL_OVERRIDE;

  /*
   * Adds a satisfied interest packet.
   * Throws a runtime exception if no matching data packet exists!
   */
  void addSatisfiedInterest(const std::string name)
  DECL_OVERRIDE;

  /*
   * Returns the loss percentage.
   * Returns 0 if the lossMap is empty or contains only FUTURESATISFIED packets.
   */
  double getLossPercentage()
  DECL_OVERRIDE;

private:

  /**
   * Packet types:
   * - SATISFIED: A data packet has returned for that interest.
   * - LOST: No data packet has returned and the interest lifetime is exceeded
   * - FUTURESATISFIED: No data packet has returned, but may return in the future(the interest lifetime is not exceeded yet)
   */
  enum class PacketType
  {
    SATISFIED, LOST, FUTURESATISFIED
  };

private:

  /**
   * The interest lifetime which acts as timeout before packets are either marked as LOST or SATISFIED
   */
  const time::steady_clock::duration m_interestLifetime;

  /**
   * The windowsize over which the final loss value is calculated
   */
  const time::steady_clock::duration m_windowSize;

  /**
   * The map for interests inside the interest lifetime.
   * Their status is undecided depending on wheter a data packet will return.
   */
  std::map<const time::steady_clock::TimePoint, const std::string> unknownMap;

  /**
   * The map for the final loss calculation.
   */
  std::map<const time::steady_clock::TimePoint, const PacketType> lossMap;

};

}  // namespace fw
}  // namespace nfd

#endif // NFD_DAEMON_FW_LOSS_ESTIMATOR_HPP

