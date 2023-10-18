// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @file   GPUError.h
/// @author Berkin Ulukutlu, berkin.ulukutlu@cern.ch
///

#ifndef AliceO2_TPC_QC_GPUERROR_H
#define AliceO2_TPC_QC_GPUERROR_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <string_view>
#include <gsl/span>

// root includes
#include "TH1.h"

// o2 includes
#include "DataFormatsTPC/Defs.h"

namespace o2
{
namespace gpu
{
class GPUErrorCodes;
struct GPUO2InterfaceConfiguration;
} // namespace gpu
namespace tpc
{
namespace qc
{

/// @brief  TPC QC task for errors from GPU reconstruction
///
/// This class is used to retrieve and visualize GPU errors
/// according to corresponding error code and location.
///
/// origin: TPC
/// @author Berkin Ulukutlu, berkin.ulukutlu@cern.ch
class GPUError
{
 public:
  /// \brief Constructor.
  GPUError() = default;

  /// bool extracts intormation from track and fills it to histograms
  /// @return true if information can be extracted and filled to histograms
  bool processErrors(gsl::span<const std::array<unsigned int, 4>>);

  /// Initialize all histograms
  void initializeHistograms();

  /// Reset all histograms
  void resetHistograms();

  /// Dump results to a file
  void dumpToFile(std::string filename);

  std::unordered_map<std::string_view, std::vector<std::unique_ptr<TH1>>>& getMapOfHisto() { return mMapHist; }
  const std::unordered_map<std::string_view, std::vector<std::unique_ptr<TH1>>>& getMapOfHisto() const { return mMapHist; }

 private:
  std::unordered_map<std::string_view, std::vector<std::unique_ptr<TH1>>> mMapHist;
  ClassDefNV(GPUError, 1)
};
} // namespace qc
} // namespace tpc
} // namespace o2

#endif