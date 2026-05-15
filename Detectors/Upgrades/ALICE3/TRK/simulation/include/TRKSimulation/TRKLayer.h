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

#ifndef ALICEO2_TRK_LAYER_H
#define ALICEO2_TRK_LAYER_H

#include "TRKBase/Specs.h"
#include <TGeoManager.h>

#include <Rtypes.h>

#include <string>
#include <utility>

namespace o2
{
namespace trk
{
enum class MatBudgetParamMode {
  Thickness,
  X2X0
};

class TRKCylindricalLayer
{
 public:
  TRKCylindricalLayer() = default;
  TRKCylindricalLayer(int layerNumber, std::string layerName, float rInn, float length, float thickOrX2X0, MatBudgetParamMode mode);
  virtual ~TRKCylindricalLayer() = default;

  auto getInnerRadius() const { return mInnerRadius; }
  auto getOuterRadius() const { return mOuterRadius; }
  auto getZ() const { return mLength; }
  auto getx2X0() const { return mX2X0; }
  auto getChipThickness() const { return mChipThickness; }
  auto getNumber() const { return mLayerNumber; }
  auto getName() const { return mLayerName; }

  virtual TGeoVolume* createSensor();
  virtual TGeoVolume* createMetalStack();
  virtual void createLayer(TGeoVolume* motherVolume);

 protected:
  // User defined parameters for the layer, to be set in the constructor
  int mLayerNumber;
  std::string mLayerName;
  float mInnerRadius;
  float mOuterRadius;
  float mLength;
  float mX2X0;
  float mChipThickness;

  // Fixed parameters for the layer, to be set based on the specifications of the chip and module
  static constexpr double sSensorThickness = constants::moduleMLOT::silicon::thickness;

  static constexpr float Si_X0 = 9.5f;

  ClassDef(TRKCylindricalLayer, 0);
};

class TRKSegmentedLayer : public TRKCylindricalLayer
{
 public:
  TRKSegmentedLayer() = default;
  TRKSegmentedLayer(int layerNumber, std::string layerName, float rInn, float tiltAngle, int numberOfStaves, int numberOfModules, float thickOrX2X0, MatBudgetParamMode mode);
  ~TRKSegmentedLayer() override = default;

  TGeoVolume* createSensor() override;
  TGeoVolume* createDeadzone();
  TGeoVolume* createMetalStack() override;
  virtual TGeoVolume* createChip();
  virtual TGeoVolume* createModule();
  virtual TGeoVolume* createStave() = 0;
  void createLayer(TGeoVolume* motherVolume) override = 0;

 protected:
  float mTiltAngle;
  int mNumberOfModules;
  int mNumberOfStaves;
  bool mIsFlipped = false;

  static constexpr double sChipWidth = constants::moduleMLOT::chip::width;
  static constexpr double sChipLength = constants::moduleMLOT::chip::length;
  static constexpr double sDeadzoneWidth = constants::moduleMLOT::chip::passiveEdgeReadOut;
  static constexpr double sModuleLength = constants::moduleMLOT::length;
  static constexpr double sModuleWidth = constants::moduleMLOT::width;
  static constexpr int sHalfNumberOfChips = 4;

  // TGeo objects outside logical volumes can cause errors
  static constexpr float sLogicalVolumeThickness = 1.3;

  // Bounding radii accounting for tilt/staggering, so the layer volume contains
  // all staves without extrusions.
  virtual std::pair<float, float> getBoundingRadii(double staveWidth) const;

  ClassDefOverride(TRKSegmentedLayer, 0);
};

class TRKMLLayer : public TRKSegmentedLayer
{
 public:
  TRKMLLayer() = default;
  TRKMLLayer(int layerNumber, std::string layerName, float rInn, float staggerOffset, float tiltAngle, int numberOfStaves, int numberOfModules, float thickOrX2X0, MatBudgetParamMode mode);
  ~TRKMLLayer() override = default;

  TGeoVolume* createStave() override;
  void createLayer(TGeoVolume* motherVolume) override;

 private:
  float mStaggerOffset;

  static constexpr double sStaveWidth = constants::ML::width;
  static constexpr int sFlippedLayerNumber = 3;

  // Override to account for the staggering offset present in specific ML layers
  std::pair<float, float> getBoundingRadii(double staveWidth) const override;

  ClassDefOverride(TRKMLLayer, 0);
};

// Original (simplified) OT barrel: solid-silicon modules, cylindrically paved.
class TRKOTLayer : public TRKSegmentedLayer
{
 public:
  TRKOTLayer() = default;
  TRKOTLayer(int layerNumber, std::string layerName, float rInn, float tiltAngle, int numberOfStaves, int numberOfModules, float thickOrX2X0, MatBudgetParamMode mode);
  ~TRKOTLayer() override = default;

  TGeoVolume* createStave() override;
  TGeoVolume* createHalfStave();
  void createLayer(TGeoVolume* motherVolume) override;

 protected:
  static constexpr float sGapBetweenOuterTrackerBarrelHalves = 0.8; // cm, gap between the two halves of the OT barrel

 private:
  static constexpr double sHalfStaveWidth = constants::OT::halfstave::width;
  static constexpr double sInStaveOverlap = constants::moduleMLOT::gaps::outerEdgeLongSide + constants::moduleMLOT::chip::passiveEdgeReadOut + 0.1;
  static constexpr double sStaveWidth = constants::OT::width - sInStaveOverlap;

  std::pair<float, float> getBoundingRadii(double staveWidth) const override;

  ClassDefOverride(TRKOTLayer, 0);
};

// Simplified-realistic OT barrel: detailed module stack (FPC, cold plate, ZIF
// connector, SMD capacitors, mounting brackets, cooling pipe), two-row staves
// with active overlap, and a barrel paved in four parts (two eta half-barrels,
// each split azimuthally into two halves cut on perpendicular planes).
// All tunable dimensions live in constants::OT; everything derived is computed
// in the source.
class TRKOTLayerRealistic : public TRKSegmentedLayer
{
 public:
  TRKOTLayerRealistic() = default;
  TRKOTLayerRealistic(int layerNumber, std::string layerName, float rInn, float tiltAngle, int numberOfStaves, int numberOfModules, float thickOrX2X0, MatBudgetParamMode mode);
  ~TRKOTLayerRealistic() override = default;

  TGeoVolume* createChip() override;
  TGeoVolume* createModule() override;
  TGeoVolume* createStave() override;
  TGeoVolume* createHalfStave();
  void createLayer(TGeoVolume* motherVolume) override;

 private:
  TGeoVolume* createFPC();
  TGeoVolume* createColdPlate();
  TGeoVolume* createCoolingPipe();
  void addConnector(TGeoVolume* moduleVol, double rMid);
  void addCapacitors(TGeoVolume* moduleVol, double rMid);
  void addBrackets(TGeoVolume* moduleVol, double rMid);

  std::pair<float, float> getBoundingRadii(double staveWidth) const override;

  ClassDefOverride(TRKOTLayerRealistic, 0);
};

} // namespace trk
} // namespace o2
#endif // ALICEO2_TRK_LAYER_H
