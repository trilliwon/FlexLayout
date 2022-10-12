/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#include "PilatesStyle.h"

const PilatesValue kPilatesValueUndefined = {PilatesUndefined, PilatesUnitUndefined};

const PilatesValue kPilatesValueAuto = {PilatesUndefined, PilatesUnitAuto};

const std::array<PilatesValue, PilatesEdgeCount> kPilatesDefaultEdgeValuesUnit = {
    {kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined,
     kPilatesValueUndefined}};

const std::array<PilatesValue, 2> kPilatesDefaultDimensionValuesAutoUnit = {
    {kPilatesValueAuto, kPilatesValueAuto}};

const std::array<PilatesValue, 2> kPilatesDefaultDimensionValuesUnit = {
    {kPilatesValueUndefined, kPilatesValueUndefined}};

PilatesStyle::PilatesStyle()
    : direction(PilatesDirectionInherit),
      flexDirection(PilatesFlexDirectionColumn),
      justifyContent(PilatesJustifyFlexStart),
      alignContent(PilatesAlignFlexStart),
      alignItems(PilatesAlignStretch),
      alignSelf(PilatesAlignAuto),
      positionType(PilatesPositionTypeRelative),
      flexWrap(PilatesWrapNoWrap),
      overflow(PilatesOverflowVisible),
      display(PilatesDisplayFlex),
      flex(PilatesUndefined),
      flexGrow(PilatesUndefined),
      flexShrink(PilatesUndefined),
      flexBasis(kPilatesValueAuto),
      margin(kPilatesDefaultEdgeValuesUnit),
      position(kPilatesDefaultEdgeValuesUnit),
      padding(kPilatesDefaultEdgeValuesUnit),
      border(kPilatesDefaultEdgeValuesUnit),
      dimensions(kPilatesDefaultDimensionValuesAutoUnit),
      minDimensions(kPilatesDefaultDimensionValuesUnit),
      maxDimensions(kPilatesDefaultDimensionValuesUnit),
      aspectRatio(PilatesUndefined) {}

// Pilates specific properties, not compatible with flexbox specification
bool PilatesStyle::operator==(const PilatesStyle& style) {
  bool areNonFloatValuesEqual = direction == style.direction &&
      flexDirection == style.flexDirection &&
      justifyContent == style.justifyContent &&
      alignContent == style.alignContent && alignItems == style.alignItems &&
      alignSelf == style.alignSelf && positionType == style.positionType &&
      flexWrap == style.flexWrap && overflow == style.overflow &&
      display == style.display && PilatesValueEqual(flexBasis, style.flexBasis) &&
      PilatesValueArrayEqual(margin, style.margin) &&
      PilatesValueArrayEqual(position, style.position) &&
      PilatesValueArrayEqual(padding, style.padding) &&
      PilatesValueArrayEqual(border, style.border) &&
      PilatesValueArrayEqual(dimensions, style.dimensions) &&
      PilatesValueArrayEqual(minDimensions, style.minDimensions) &&
      PilatesValueArrayEqual(maxDimensions, style.maxDimensions);

  if (!(std::isnan(flex) && std::isnan(style.flex))) {
    areNonFloatValuesEqual = areNonFloatValuesEqual && flex == style.flex;
  }

  if (!(std::isnan(flexGrow) && std::isnan(style.flexGrow))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && flexGrow == style.flexGrow;
  }

  if (!(std::isnan(flexShrink) && std::isnan(style.flexShrink))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && flexShrink == style.flexShrink;
  }

  if (!(std::isnan(aspectRatio) && std::isnan(style.aspectRatio))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && aspectRatio == style.aspectRatio;
  }

  return areNonFloatValuesEqual;
}

bool PilatesStyle::operator!=(PilatesStyle style) {
  return !(*this == style);
}

PilatesStyle::~PilatesStyle() {}
