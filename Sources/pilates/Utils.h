/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include "PilatesNode.h"
#include "Pilates-internal.h"

// This struct is an helper model to hold the data for step 4 of flexbox
// algo, which is collecting the flex items in a line.
//
// - itemsOnLine: Number of items which can fit in a line considering the
// available Inner dimension, the flex items computed flexbasis and their
// margin. It may be different than the difference between start and end
// indicates because we skip over absolute-positioned items.
//
// - sizeConsumedOnCurrentLine: It is accumulation of the dimensions and margin
// of all the children on the current line. This will be used in order to either
// set the dimensions of the node if none already exist or to compute the
// remaining space left for the flexible children.
//
// - totalFlexGrowFactors: total flex grow factors of flex items which are to be
// layed in the current line
//
// - totalFlexShrinkFactors: total flex shrink factors of flex items which are
// to be layed in the current line
//
// - endOfLineIndex: Its the end index of the last flex item which was examined
// and it may or may not be part of the current line(as it may be absolutely
// positioned or inculding it may have caused to overshoot availableInnerDim)
//
// - relativeChildren: Maintain a vector of the child nodes that can shrink
// and/or grow.

struct PilatesCollectFlexItemsRowValues {
  uint32_t itemsOnLine;
  float sizeConsumedOnCurrentLine;
  float totalFlexGrowFactors;
  float totalFlexShrinkScaledFactors;
  float endOfLineIndex;
  std::vector<PilatesNodeRef> relativeChildren;
  float remainingFreeSpace;
  // The size of the mainDim for the row after considering size, padding, margin
  // and border of flex items. This is used to calculate maxLineDim after going
  // through all the rows to decide on the main axis size of parent.
  float mainDim;
  // The size of the crossDim for the row after considering size, padding,
  // margin and border of flex items. Used for calculating containers crossSize.
  float crossDim;
};

bool PilatesValueEqual(const PilatesValue a, const PilatesValue b);

PilatesFlexDirection PilatesFlexDirectionCross(
    const PilatesFlexDirection flexDirection,
    const PilatesDirection direction);

inline bool PilatesFlexDirectionIsRow(const PilatesFlexDirection flexDirection) {
  return flexDirection == PilatesFlexDirectionRow ||
      flexDirection == PilatesFlexDirectionRowReverse;
}

inline float PilatesResolveValue(const PilatesValue value, const float parentSize) {
  switch (value.unit) {
    case PilatesUnitUndefined:
    case PilatesUnitAuto:
      return PilatesUndefined;
    case PilatesUnitPoint:
      return value.value;
    case PilatesUnitPercent:
      return value.value * parentSize / 100.0f;
  }
  return PilatesUndefined;
}

inline bool PilatesFlexDirectionIsColumn(const PilatesFlexDirection flexDirection) {
  return flexDirection == PilatesFlexDirectionColumn ||
      flexDirection == PilatesFlexDirectionColumnReverse;
}

inline PilatesFlexDirection PilatesResolveFlexDirection(
    const PilatesFlexDirection flexDirection,
    const PilatesDirection direction) {
  if (direction == PilatesDirectionRTL) {
    if (flexDirection == PilatesFlexDirectionRow) {
      return PilatesFlexDirectionRowReverse;
    } else if (flexDirection == PilatesFlexDirectionRowReverse) {
      return PilatesFlexDirectionRow;
    }
  }

  return flexDirection;
}

static inline float PilatesResolveValueMargin(
    const PilatesValue value,
    const float parentSize) {
  return value.unit == PilatesUnitAuto ? 0 : PilatesResolveValue(value, parentSize);
}
