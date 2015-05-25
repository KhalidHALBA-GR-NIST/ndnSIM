#ifndef NFD_DAEMON_FW_LOSS_ESTIMATOR_HPP
#define NFD_DAEMON_FW_LOSS_ESTIMATOR_HPP

#include "common.hpp"

namespace nfd {
namespace fw {

/**
 * \brief Interface for all Loss Estimator classes
 */
class LossEstimator
{
public:

  virtual ~LossEstimator()
  {
  }

  /**
   * Adds one sent interest packet
   */
  virtual void addSentInterest(const std::string name) = 0;

  /**
   * Adds one satisfied interest packet (= received data packet)
   */
  virtual void addSatisfiedInterest(const std::string name) = 0;

  /**
   * Returns the loss percentage as value between 0 and 1.
   */
  virtual double getLossPercentage() = 0;

};

}  // namespace fw
}  // namespace nfd

#endif // NFD_DAEMON_FW_LOSS_ESTIMATOR_HPP

