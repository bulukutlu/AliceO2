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

#define _USE_MATH_DEFINES

#include <cmath>
#include <memory>

// root includes
#include "TFile.h"
#include "TMathBase.h"

// o2 includes
#include "DataFormatsTPC/TrackTPC.h"
#include "DataFormatsTPC/dEdxInfo.h"
#include "TPCQC/GPUError.h"
#include "TPCQC/Helpers.h"

//ClassImp(o2::tpc::qc::GPUError);

using namespace o2::tpc::qc;

//______________________________________________________________________________
void GPUError::initializeHistograms()
{

}
//______________________________________________________________________________
void GPUError::resetHistograms()
{
  for (const auto& pair : mMapHist) {
    for (auto& hist : pair.second) {
      hist->Reset();
    }
  }
}
//______________________________________________________________________________
bool GPUError::processErrors(gsl::span<const std::array<unsigned int, 4>>)
{
  return true;
}

//______________________________________________________________________________
void GPUError::dumpToFile(const std::string filename)
{
  auto f = std::unique_ptr<TFile>(TFile::Open(filename.c_str(), "recreate"));
  for (const auto& [name, histos] : mMapHist) {
    TObjArray arr;
    arr.SetName(name.data());
    for (auto& hist : histos) {
      arr.Add(hist.get());
    }
    arr.Write(arr.GetName(), TObject::kSingleKey);
  }
  f->Close();
}
