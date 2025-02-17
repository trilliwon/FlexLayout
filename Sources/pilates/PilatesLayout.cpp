/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#include "PilatesLayout.h"

const std::array<float, 2> kPilatesDefaultDimensionValues = {
    {PilatesUndefined, PilatesUndefined}};

PilatesLayout::PilatesLayout()
    : position(),
      dimensions(kPilatesDefaultDimensionValues),
      margin(),
      border(),
      padding(),
      direction(PilatesDirectionInherit),
      computedFlexBasisGeneration(0),
      computedFlexBasis(PilatesUndefined),
      hadOverflow(false),
      generationCount(0),
      lastParentDirection((PilatesDirection)-1),
      nextCachedMeasurementsIndex(0),
      cachedMeasurements(),
      measuredDimensions(kPilatesDefaultDimensionValues),
      cachedLayout(PilatesCachedMeasurement()),
      didUseLegacyFlag(false),
      doesLegacyStretchFlagAffectsLayout(false) {}

bool PilatesLayout::operator==(PilatesLayout layout) const {
  bool isEqual = position == layout.position &&
      dimensions == layout.dimensions && margin == layout.margin &&
      border == layout.border && padding == layout.padding &&
      direction == layout.direction && hadOverflow == layout.hadOverflow &&
      lastParentDirection == layout.lastParentDirection &&
      nextCachedMeasurementsIndex == layout.nextCachedMeasurementsIndex &&
      cachedLayout == layout.cachedLayout;

  for (uint32_t i = 0; i < Pilates_MAX_CACHED_RESULT_COUNT && isEqual; ++i) {
    isEqual = isEqual && cachedMeasurements[i] == layout.cachedMeasurements[i];
  }

  if (!PilatesFloatIsUndefined(computedFlexBasis) ||
      !PilatesFloatIsUndefined(layout.computedFlexBasis)) {
    isEqual = isEqual && (computedFlexBasis == layout.computedFlexBasis);
  }
  if (!PilatesFloatIsUndefined(measuredDimensions[0]) ||
      !PilatesFloatIsUndefined(layout.measuredDimensions[0])) {
    isEqual =
        isEqual && (measuredDimensions[0] == layout.measuredDimensions[0]);
  }
  if (!PilatesFloatIsUndefined(measuredDimensions[1]) ||
      !PilatesFloatIsUndefined(layout.measuredDimensions[1])) {
    isEqual =
        isEqual && (measuredDimensions[1] == layout.measuredDimensions[1]);
  }

  return isEqual;
}

bool PilatesLayout::operator!=(PilatesLayout layout) const {
  return !(*this == layout);
}
