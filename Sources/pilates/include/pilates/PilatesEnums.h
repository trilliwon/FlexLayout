/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "PilatesMacros.h"

Pilates_EXTERN_C_BEGIN

#define PilatesAlignCount 8
typedef Pilates_ENUM_BEGIN(PilatesAlign) {
  PilatesAlignAuto,
  PilatesAlignFlexStart,
  PilatesAlignCenter,
  PilatesAlignFlexEnd,
  PilatesAlignStretch,
  PilatesAlignBaseline,
  PilatesAlignSpaceBetween,
  PilatesAlignSpaceAround,
} Pilates_ENUM_END(PilatesAlign);
WIN_EXPORT const char *PilatesAlignToString(const PilatesAlign value);

#define PilatesDimensionCount 2
typedef Pilates_ENUM_BEGIN(PilatesDimension) {
  PilatesDimensionWidth,
  PilatesDimensionHeight,
} Pilates_ENUM_END(PilatesDimension);
WIN_EXPORT const char *PilatesDimensionToString(const PilatesDimension value);

#define PilatesDirectionCount 3
typedef Pilates_ENUM_BEGIN(PilatesDirection) {
  PilatesDirectionInherit,
  PilatesDirectionLTR,
  PilatesDirectionRTL,
} Pilates_ENUM_END(PilatesDirection);
WIN_EXPORT const char *PilatesDirectionToString(const PilatesDirection value);

#define PilatesDisplayCount 2
typedef Pilates_ENUM_BEGIN(PilatesDisplay) {
  PilatesDisplayFlex,
  PilatesDisplayNone,
} Pilates_ENUM_END(PilatesDisplay);
WIN_EXPORT const char *PilatesDisplayToString(const PilatesDisplay value);

#define PilatesEdgeCount 9
typedef Pilates_ENUM_BEGIN(PilatesEdge) {
  PilatesEdgeLeft,
  PilatesEdgeTop,
  PilatesEdgeRight,
  PilatesEdgeBottom,
  PilatesEdgeStart,
  PilatesEdgeEnd,
  PilatesEdgeHorizontal,
  PilatesEdgeVertical,
  PilatesEdgeAll,
} Pilates_ENUM_END(PilatesEdge);
WIN_EXPORT const char *PilatesEdgeToString(const PilatesEdge value);

#define PilatesExperimentalFeatureCount 1
typedef Pilates_ENUM_BEGIN(PilatesExperimentalFeature) {
  PilatesExperimentalFeatureWebFlexBasis,
} Pilates_ENUM_END(PilatesExperimentalFeature);
WIN_EXPORT const char *PilatesExperimentalFeatureToString(const PilatesExperimentalFeature value);

#define PilatesFlexDirectionCount 4
typedef Pilates_ENUM_BEGIN(PilatesFlexDirection) {
  PilatesFlexDirectionColumn,
  PilatesFlexDirectionColumnReverse,
  PilatesFlexDirectionRow,
  PilatesFlexDirectionRowReverse,
} Pilates_ENUM_END(PilatesFlexDirection);
WIN_EXPORT const char *PilatesFlexDirectionToString(const PilatesFlexDirection value);

#define PilatesJustifyCount 6
typedef Pilates_ENUM_BEGIN(PilatesJustify){
    PilatesJustifyFlexStart,
    PilatesJustifyCenter,
    PilatesJustifyFlexEnd,
    PilatesJustifySpaceBetween,
    PilatesJustifySpaceAround,
    PilatesJustifySpaceEvenly,
} Pilates_ENUM_END(PilatesJustify);
WIN_EXPORT const char *PilatesJustifyToString(const PilatesJustify value);

#define PilatesLogLevelCount 6
typedef Pilates_ENUM_BEGIN(PilatesLogLevel) {
  PilatesLogLevelError,
  PilatesLogLevelWarn,
  PilatesLogLevelInfo,
  PilatesLogLevelDebug,
  PilatesLogLevelVerbose,
  PilatesLogLevelFatal,
} Pilates_ENUM_END(PilatesLogLevel);
WIN_EXPORT const char *PilatesLogLevelToString(const PilatesLogLevel value);

#define PilatesMeasureModeCount 3
typedef Pilates_ENUM_BEGIN(PilatesMeasureMode) {
  PilatesMeasureModeUndefined,
  PilatesMeasureModeExactly,
  PilatesMeasureModeAtMost,
} Pilates_ENUM_END(PilatesMeasureMode);
WIN_EXPORT const char *PilatesMeasureModeToString(const PilatesMeasureMode value);

#define PilatesNodeTypeCount 2
typedef Pilates_ENUM_BEGIN(PilatesNodeType) {
  PilatesNodeTypeDefault,
  PilatesNodeTypeText,
} Pilates_ENUM_END(PilatesNodeType);
WIN_EXPORT const char *PilatesNodeTypeToString(const PilatesNodeType value);

#define PilatesOverflowCount 3
typedef Pilates_ENUM_BEGIN(PilatesOverflow) {
  PilatesOverflowVisible,
  PilatesOverflowHidden,
  PilatesOverflowScroll,
} Pilates_ENUM_END(PilatesOverflow);
WIN_EXPORT const char *PilatesOverflowToString(const PilatesOverflow value);

#define PilatesPositionTypeCount 2
typedef Pilates_ENUM_BEGIN(PilatesPositionType) {
  PilatesPositionTypeRelative,
  PilatesPositionTypeAbsolute,
} Pilates_ENUM_END(PilatesPositionType);
WIN_EXPORT const char *PilatesPositionTypeToString(const PilatesPositionType value);

#define PilatesPrintOptionsCount 3
typedef Pilates_ENUM_BEGIN(PilatesPrintOptions) {
  PilatesPrintOptionsLayout = 1,
  PilatesPrintOptionsStyle = 2,
  PilatesPrintOptionsChildren = 4,
} Pilates_ENUM_END(PilatesPrintOptions);
WIN_EXPORT const char *PilatesPrintOptionsToString(const PilatesPrintOptions value);

#define PilatesUnitCount 4
typedef Pilates_ENUM_BEGIN(PilatesUnit) {
  PilatesUnitUndefined,
  PilatesUnitPoint,
  PilatesUnitPercent,
  PilatesUnitAuto,
} Pilates_ENUM_END(PilatesUnit);
WIN_EXPORT const char *PilatesUnitToString(const PilatesUnit value);

#define PilatesWrapCount 3
typedef Pilates_ENUM_BEGIN(PilatesWrap) {
  PilatesWrapNoWrap,
  PilatesWrapWrap,
  PilatesWrapWrapReverse,
} Pilates_ENUM_END(PilatesWrap);
WIN_EXPORT const char *PilatesWrapToString(const PilatesWrap value);

Pilates_EXTERN_C_END
