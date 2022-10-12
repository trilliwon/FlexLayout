/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Utils.h"

PilatesFlexDirection PilatesFlexDirectionCross(
    const PilatesFlexDirection flexDirection,
    const PilatesDirection direction) {
  return PilatesFlexDirectionIsColumn(flexDirection)
      ? PilatesResolveFlexDirection(PilatesFlexDirectionRow, direction)
      : PilatesFlexDirectionColumn;
}

bool PilatesValueEqual(const PilatesValue a, const PilatesValue b) {
  if (a.unit != b.unit) {
    return false;
  }

  if (a.unit == PilatesUnitUndefined ||
      (std::isnan(a.value) && std::isnan(b.value))) {
    return true;
  }

  return fabs(a.value - b.value) < 0.0001f;
}
