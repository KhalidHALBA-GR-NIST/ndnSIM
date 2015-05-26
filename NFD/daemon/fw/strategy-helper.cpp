/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Klaus Schneider, University of Bamberg, Germany
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Klaus Schneider <klaus.schneider@uni-bamberg.de>
 */
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
