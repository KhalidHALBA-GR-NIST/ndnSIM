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
#include "lowest-cost-strategy.hpp"
#include "core/logger.hpp"
#include "measurement-info.hpp"

namespace nfd {
namespace fw {

NFD_LOG_INIT("LowestCostStrategy")

const Name LowestCostStrategy::STRATEGY_NAME("ndn:/localhost/nfd/strategy/lowest-cost/%FD%01/");

LowestCostStrategy::LowestCostStrategy(Forwarder& forwarder, const Name& name) :  // Default delay: 100 ms
    Strategy(forwarder, name), requirements(StrategyRequirements()),
        ownStrategyChoice(forwarder.getStrategyChoice()), priorityType(RequirementType::DELAY),
        currentWorkingFace(-1)
{
}

void LowestCostStrategy::afterReceiveInterest(const Face& inFace, const Interest& interest,
    shared_ptr<fib::Entry> fibEntry, shared_ptr<pit::Entry> pitEntry)
{

  Name currentPrefix;
  shared_ptr < MeasurementInfo > measurementInfo;
  nfd::MeasurementsAccessor& ma = this->getMeasurements();
  std::tie(currentPrefix, measurementInfo) = StrategyHelper::findPrefixMeasurements(interest, ma);

  // Prefix found
  if (measurementInfo != nullptr) {
    currentWorkingFace = measurementInfo->currentWorkingFace;
    requirements = measurementInfo->req;
  }
  else {
    // Create new prefix
    nfd::MeasurementsAccessor & ma = this->getMeasurements();
    measurementInfo = StrategyHelper::addPrefixMeasurements(interest, ma);
    NFD_LOG_WARN("New prefix " << interest.getName() << " from " << inFace.getId());
    requirements.parseParameters(ownStrategyChoice.findEffectiveParameters(interest.getName()));
    measurementInfo->req = requirements;
  }

  if (pitEntry->hasUnexpiredOutRecords()) {
    // Retransmitted interest from consumer application. Don't forward.
    return;
  }

  shared_ptr < Face > outFace = getOutputFace(fibEntry->getNextHops(), pitEntry, requirements);

  if (outFace == NULL) {
    NFD_LOG_WARN("No face available!\n");
    return;
  }
  else {
    // Probe Interests
    if (helper.probingDue()) {
      probeInterests(outFace, interest, fibEntry->getNextHops(), pitEntry);
    }

    if (outFace->getId() != currentWorkingFace) {
      NFD_LOG_TRACE("New current working face: " << currentWorkingFace);
      currentWorkingFace = outFace->getId();
      measurementInfo->currentWorkingFace = outFace->getId();
    }
    faceInfoTable[outFace->getId()].addSentInterest(interest.getName().toUri());
    this->sendInterest(pitEntry, outFace);
  }
}

shared_ptr<Face> LowestCostStrategy::getOutputFace(const fib::NextHopList& nexthops,
    shared_ptr<pit::Entry> pitEntry, StrategyRequirements &requirements)
{
  shared_ptr < Face > outFace = NULL;

  if (requirements.contains(RequirementType::DELAY)
      && requirements.contains(RequirementType::LOSS)) {
    for (auto n : nexthops) {
      bool isWorkingFace = (n.getFace()->getId() == currentWorkingFace);
      double currentDelay = faceInfoTable[n.getFace()->getId()].getCurrentValue(
          RequirementType::DELAY);
      double currentLoss = faceInfoTable[n.getFace()->getId()].getCurrentValue(
          RequirementType::LOSS);

      if (pitEntry->canForwardTo(*n.getFace())) {
        double delayLimit = requirements.getLimit(RequirementType::DELAY);
        double lossLimit = requirements.getLimit(RequirementType::LOSS);
        if (!isWorkingFace) {
          delayLimit /= 1.2;
          lossLimit /= 1.2;
        }
        if (currentDelay < delayLimit && currentLoss < lossLimit) {
          outFace = n.getFace();
          break;
        }
      }
    }
    if (outFace == NULL) {
      // Not all requirements could be met.
      outFace = getLowestTypeFace(nexthops, pitEntry, priorityType);
    }
  }
  else if (requirements.contains(RequirementType::DELAY)) {
    outFace = getLowestTypeFace(nexthops, pitEntry, RequirementType::DELAY);
  }
  else if (requirements.contains(RequirementType::LOSS)) {
    outFace = getLowestTypeFace(nexthops, pitEntry, RequirementType::LOSS);
  }
  else if (requirements.contains(RequirementType::BANDWIDTH)) {
    outFace = getLowestTypeFace(nexthops, pitEntry, RequirementType::BANDWIDTH, true);
  }
  else {
    NFD_LOG_WARN("Should not be reached!");
  }

  return outFace;
}

shared_ptr<Face> LowestCostStrategy::getLowestTypeFace(const fib::NextHopList& nexthops,
    shared_ptr<pit::Entry> pitEntry, RequirementType type, bool isUpwardAttribute)
{
  shared_ptr < Face > outFace = NULL;

  for (auto n : nexthops) {
    bool isWorkingFace = (n.getFace()->getId() == currentWorkingFace);
    double currentLimit;
    currentLimit = requirements.getLimit(type);
    if (!isWorkingFace) {
      if (StrategyRequirements::isUpwardAttribute(type)) {
        currentLimit *= 1.2;
      }
      else {
        currentLimit /= 1.2;
      }
    }
    double currentValue = faceInfoTable[n.getFace()->getId()].getCurrentValue(type);
    if (pitEntry->canForwardTo(*n.getFace())) {
      if (!isUpwardAttribute && currentValue < currentLimit) {
        outFace = n.getFace();
        break;
      }
      if (isUpwardAttribute && currentValue > currentLimit) {
        outFace = n.getFace();
        break;
      }
    }
  }

  // If no face meets the requirement: Send out on best face.
  if (outFace == NULL) {
    double lowestValue = std::numeric_limits<double>::infinity();
    double highestValue = -1;
    for (auto n : nexthops) {
      double currentValue = faceInfoTable[n.getFace()->getId()].getCurrentValue(type);
      if (!isUpwardAttribute && pitEntry->canForwardTo(*n.getFace())
          && currentValue < lowestValue) {
        lowestValue = currentValue;
        outFace = n.getFace();
      }
      if (isUpwardAttribute && pitEntry->canForwardTo(*n.getFace())
          && currentValue > highestValue) {
        NFD_LOG_TRACE(
            "Highest value: " << currentValue << ", " << highestValue << ", face: "
                << n.getFace()->getId());
        highestValue = currentValue;
        outFace = n.getFace();
      }

    }
  }
  return outFace;
}

void LowestCostStrategy::probeInterests(const shared_ptr<Face> outFace, const Interest& interest,
    const fib::NextHopList& nexthops, shared_ptr<pit::Entry> pitEntry)
{
  for (auto n : nexthops) {
    if (n.getFace() != outFace) {
      NFD_LOG_TRACE("Probing face: " << n.getFace()->getId());
      faceInfoTable[n.getFace()->getId()].addSentInterest(interest.getName().toUri());
      this->sendInterest(pitEntry, n.getFace(), true);
    }
  }
}

void LowestCostStrategy::beforeSatisfyInterest(shared_ptr<pit::Entry> pitEntry, const Face& inFace,
    const Data& data)
{

  // Update loss info!
  InterfaceEstimation& faceInfo = faceInfoTable[inFace.getId()];
  faceInfo.addSatisfiedInterest(data.getContent().value_size(), data.getName().toUri());

  pit::OutRecordCollection::const_iterator outRecord = pitEntry->getOutRecord(inFace);

  // already satisfied by another upstream
  if (pitEntry->getInRecords().empty() || outRecord == pitEntry->getOutRecords().end()) {
    // Do nothing else
  }

  // There is an in and outrecord: Update RTT value!
  else {
    time::steady_clock::Duration rtt = time::steady_clock::now() - outRecord->getLastRenewed();
    faceInfo.addRttMeasurement(time::duration_cast < time::microseconds > (rtt));
  }
}

}  // namespace fw
}  // namespace nfd

