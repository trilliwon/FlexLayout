/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include "Pilates.h"

using PilatesVector = std::vector<PilatesNodeRef>;

Pilates_EXTERN_C_BEGIN

WIN_EXPORT float PilatesRoundValueToPixelGrid(const float value,
                                         const float pointScaleFactor,
                                         const bool forceCeil,
                                         const bool forceFloor);

Pilates_EXTERN_C_END

extern const std::array<PilatesEdge, 4> trailing;
extern const std::array<PilatesEdge, 4> leading;
extern bool PilatesValueEqual(const PilatesValue a, const PilatesValue b);
extern const PilatesValue PilatesValueUndefined;
extern const PilatesValue PilatesValueAuto;
extern const PilatesValue PilatesValueZero;

template <std::size_t size>
bool PilatesValueArrayEqual(
    const std::array<PilatesValue, size> val1,
    const std::array<PilatesValue, size> val2) {
  bool areEqual = true;
  for (uint32_t i = 0; i < size && areEqual; ++i) {
    areEqual = PilatesValueEqual(val1[i], val2[i]);
  }
  return areEqual;
}

struct PilatesCachedMeasurement {
  float availableWidth;
  float availableHeight;
  PilatesMeasureMode widthMeasureMode;
  PilatesMeasureMode heightMeasureMode;

  float computedWidth;
  float computedHeight;

  PilatesCachedMeasurement()
      : availableWidth(0),
        availableHeight(0),
        widthMeasureMode((PilatesMeasureMode)-1),
        heightMeasureMode((PilatesMeasureMode)-1),
        computedWidth(-1),
        computedHeight(-1) {}

  bool operator==(PilatesCachedMeasurement measurement) const {
    bool isEqual = widthMeasureMode == measurement.widthMeasureMode &&
        heightMeasureMode == measurement.heightMeasureMode;

    if (!std::isnan(availableWidth) ||
        !std::isnan(measurement.availableWidth)) {
      isEqual = isEqual && availableWidth == measurement.availableWidth;
    }
    if (!std::isnan(availableHeight) ||
        !std::isnan(measurement.availableHeight)) {
      isEqual = isEqual && availableHeight == measurement.availableHeight;
    }
    if (!std::isnan(computedWidth) || !std::isnan(measurement.computedWidth)) {
      isEqual = isEqual && computedWidth == measurement.computedWidth;
    }
    if (!std::isnan(computedHeight) ||
        !std::isnan(measurement.computedHeight)) {
      isEqual = isEqual && computedHeight == measurement.computedHeight;
    }

    return isEqual;
  }
};

// This value was chosen based on empiracle data. Even the most complicated
// layouts should not require more than 16 entries to fit within the cache.
#define Pilates_MAX_CACHED_RESULT_COUNT 16

struct PilatesConfig {
  bool experimentalFeatures[PilatesExperimentalFeatureCount + 1];
  bool useWebDefaults;
  bool useLegacyStretchBehaviour;
  bool shouldDiffLayoutWithoutLegacyStretchBehaviour;
  float pointScaleFactor;
  PilatesLogger logger;
  PilatesNodeClonedFunc cloneNodeCallback;
  void* context;
};

static const float kDefaultFlexGrow = 0.0f;
static const float kDefaultFlexShrink = 0.0f;
static const float kWebDefaultFlexShrink = 1.0f;

extern bool PilatesFloatsEqual(const float a, const float b);
extern bool PilatesValueEqual(const PilatesValue a, const PilatesValue b);
extern const PilatesValue* PilatesComputedEdgeValue(
    const std::array<PilatesValue, PilatesEdgeCount>& edges,
    const PilatesEdge edge,
    const PilatesValue* const defaultValue);
