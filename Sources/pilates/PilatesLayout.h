/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once
#include "Pilates-internal.h"

struct PilatesLayout {
  std::array<float, 4> position;
  std::array<float, 2> dimensions;
  std::array<float, 6> margin;
  std::array<float, 6> border;
  std::array<float, 6> padding;
  PilatesDirection direction;

  uint32_t computedFlexBasisGeneration;
  float computedFlexBasis;
  bool hadOverflow;

  // Instead of recomputing the entire layout every single time, we
  // cache some information to break early when nothing changed
  uint32_t generationCount;
  PilatesDirection lastParentDirection;

  uint32_t nextCachedMeasurementsIndex;
  std::array<PilatesCachedMeasurement, Pilates_MAX_CACHED_RESULT_COUNT>
      cachedMeasurements;
  std::array<float, 2> measuredDimensions;

  PilatesCachedMeasurement cachedLayout;
  bool didUseLegacyFlag;
  bool doesLegacyStretchFlagAffectsLayout;

  PilatesLayout();

  bool operator==(PilatesLayout layout) const;
  bool operator!=(PilatesLayout layout) const;
};
