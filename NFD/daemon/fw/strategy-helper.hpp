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
#ifndef NFD_DAEMON_FW_STRATEGY_HELPER_HPP
#define NFD_DAEMON_FW_STRATEGY_HELPER_HPP

#include <map>
#include <string>

namespace nfd {
namespace fw {

class StrategyHelper
{

public:

  /**
   * Parses a parameter string to a map of parameter attribute names to attribute values.
   *
   * \param parameterString is of the format "param1=value1,param2=value2,...,paramn=valuen"
   */
  static std::map<std::string, std::string> getParameterMap(std::string parameterString);
};

}  // namespace fw
}  // namespace nfd

#endif
