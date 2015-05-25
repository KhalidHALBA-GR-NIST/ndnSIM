#ifndef NFD_DAEMON_FW_STRATEGY_HELPER_HPP
#define NFD_DAEMON_FW_STRATEGY_HELPER_HPP

#include <map>
#include <string>

namespace nfd {
namespace fw {

class StrategyHelper
{

public:
  static std::map<std::string, std::string> getParameterMap(std::string parameters);
};

}  // namespace fw
}  // namespace nfd

#endif
