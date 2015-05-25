#ifndef NFD_DAEMON_FW_OWNSTRATEGY_HPP
#define NFD_DAEMON_FW_OWNSTRATEGY_HPP

#include "strategy.hpp"

namespace nfd {
namespace fw {

/** \class BroadcastNewNonceStrategy
 *  \brief Forwards interests to all nexthops using a new nonce.
 *
 *  The parameter "nonce" denotes if a new nonce should be used (nonce=true) or not (nonce=false).
 *  The default is to use a new nonce.
 */
class BroadcastNewNonceStrategy : public Strategy
{
public:
  BroadcastNewNonceStrategy(Forwarder& forwarder, const Name& name = STRATEGY_NAME);

  virtual
  ~BroadcastNewNonceStrategy();

  virtual void
  afterReceiveInterest(const Face& inFace, const Interest& interest,
      shared_ptr<fib::Entry> fibEntry, shared_ptr<pit::Entry> pitEntry)
  DECL_OVERRIDE;

public:
  static const Name STRATEGY_NAME;

private:
  StrategyChoice& ownStrategyChoice;
};

}  // namespace fw
}  // namespace nfd

#endif
