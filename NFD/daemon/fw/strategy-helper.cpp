#include "strategy-helper.hpp"
#include "core/logger.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/detail/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

namespace nfd {
namespace fw {

NFD_LOG_INIT("StrategyHelper");

std::map<std::string, std::string> StrategyHelper::getParameterMap(std::string parameters)
{
  NFD_LOG_INFO("Parsing parameters!");

  std::map < std::string, std::string > outputMap;

// Replace ASCII elements
  boost::replace_all(parameters, "%2C", ",");
  boost::replace_all(parameters, "%3D", "=");

  std::vector < std::string > paramVector;
  boost::split(paramVector, parameters, boost::is_any_of(","));

  for (auto substring : paramVector) {
    std::vector < std::string > substringVector;
    boost::split(substringVector, substring, boost::is_any_of("="));

    std::string key = substringVector.front();
    std::string value = substringVector.back();
    outputMap[substringVector.front()] = substringVector.back();
  }

  return outputMap;
}

}  // namespace nfd
}  // namespace fw
