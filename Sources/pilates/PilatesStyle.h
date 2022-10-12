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
#include "Pilates.h"

struct PilatesStyle {
  PilatesDirection direction;
  PilatesFlexDirection flexDirection;
  PilatesJustify justifyContent;
  PilatesAlign alignContent;
  PilatesAlign alignItems;
  PilatesAlign alignSelf;
  PilatesPositionType positionType;
  PilatesWrap flexWrap;
  PilatesOverflow overflow;
  PilatesDisplay display;
  float flex;
  float flexGrow;
  float flexShrink;
  PilatesValue flexBasis;
  std::array<PilatesValue, PilatesEdgeCount> margin;
  std::array<PilatesValue, PilatesEdgeCount> position;
  std::array<PilatesValue, PilatesEdgeCount> padding;
  std::array<PilatesValue, PilatesEdgeCount> border;
  std::array<PilatesValue, 2> dimensions;
  std::array<PilatesValue, 2> minDimensions;
  std::array<PilatesValue, 2> maxDimensions;
  float aspectRatio;

  PilatesStyle();
  // Pilates specific properties, not compatible with flexbox specification
  bool operator==(const PilatesStyle& style);

  bool operator!=(PilatesStyle style);
  ~PilatesStyle();
};
