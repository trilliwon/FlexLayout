/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

// Not defined in MSVC++
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *) __nan)
#endif

#define PilatesUndefined NAN

#include "PilatesEnums.h"
#include "PilatesMacros.h"

Pilates_EXTERN_C_BEGIN

typedef struct PilatesSize {
  float width;
  float height;
} PilatesSize;

typedef struct PilatesValue {
  float value;
  PilatesUnit unit;
} PilatesValue;

extern const PilatesValue PilatesValueUndefined;
extern const PilatesValue PilatesValueAuto;

typedef struct PilatesConfig *PilatesConfigRef;

typedef struct PilatesNode* PilatesNodeRef;

typedef PilatesSize (*PilatesMeasureFunc)(PilatesNodeRef node,
                                float width,
                                PilatesMeasureMode widthMode,
                                float height,
                                PilatesMeasureMode heightMode);
typedef float (*PilatesBaselineFunc)(PilatesNodeRef node, const float width, const float height);
typedef void (*PilatesDirtiedFunc)(PilatesNodeRef node);
typedef void (*PilatesPrintFunc)(PilatesNodeRef node);
typedef int (*PilatesLogger)(const PilatesConfigRef config,
                        const PilatesNodeRef node,
                        PilatesLogLevel level,
                        const char *format,
                        va_list args);
typedef void (*PilatesNodeClonedFunc)(PilatesNodeRef oldNode,
                                 PilatesNodeRef newNode,
                                 PilatesNodeRef parent,
                                 int childIndex);

// PilatesNode
WIN_EXPORT PilatesNodeRef PilatesNodeNew(void);
WIN_EXPORT PilatesNodeRef PilatesNodeNewWithConfig(const PilatesConfigRef config);
WIN_EXPORT PilatesNodeRef PilatesNodeClone(const PilatesNodeRef node);
WIN_EXPORT void PilatesNodeFree(const PilatesNodeRef node);
WIN_EXPORT void PilatesNodeFreeRecursive(const PilatesNodeRef node);
WIN_EXPORT void PilatesNodeReset(const PilatesNodeRef node);
WIN_EXPORT int32_t PilatesNodeGetInstanceCount(void);

WIN_EXPORT void PilatesNodeInsertChild(const PilatesNodeRef node,
                                  const PilatesNodeRef child,
                                  const uint32_t index);
WIN_EXPORT void PilatesNodeRemoveChild(const PilatesNodeRef node, const PilatesNodeRef child);
WIN_EXPORT void PilatesNodeRemoveAllChildren(const PilatesNodeRef node);
WIN_EXPORT PilatesNodeRef PilatesNodeGetChild(const PilatesNodeRef node, const uint32_t index);
WIN_EXPORT PilatesNodeRef PilatesNodeGetParent(const PilatesNodeRef node);
WIN_EXPORT uint32_t PilatesNodeGetChildCount(const PilatesNodeRef node);

WIN_EXPORT void PilatesNodeCalculateLayout(const PilatesNodeRef node,
                                      const float availableWidth,
                                      const float availableHeight,
                                      const PilatesDirection parentDirection);

// Mark a node as dirty. Only valid for nodes with a custom measure function
// set.
// Pilates knows when to mark all other nodes as dirty but because nodes with
// measure functions
// depends on information not known to Pilates they must perform this dirty
// marking manually.
WIN_EXPORT void PilatesNodeMarkDirty(const PilatesNodeRef node);

// This function marks the current node and all its descendants as dirty. This function is added to test pilates benchmarks.
// This function is not expected to be used in production as calling `PilatesCalculateLayout` will cause the recalculation of each and every node.
WIN_EXPORT void PilatesNodeMarkDirtyAndPropogateToDescendants(const PilatesNodeRef node);

WIN_EXPORT void PilatesNodePrint(const PilatesNodeRef node, const PilatesPrintOptions options);

WIN_EXPORT bool PilatesFloatIsUndefined(const float value);

WIN_EXPORT bool PilatesNodeCanUseCachedMeasurement(const PilatesMeasureMode widthMode,
                                              const float width,
                                              const PilatesMeasureMode heightMode,
                                              const float height,
                                              const PilatesMeasureMode lastWidthMode,
                                              const float lastWidth,
                                              const PilatesMeasureMode lastHeightMode,
                                              const float lastHeight,
                                              const float lastComputedWidth,
                                              const float lastComputedHeight,
                                              const float marginRow,
                                              const float marginColumn,
                                              const PilatesConfigRef config);

WIN_EXPORT void PilatesNodeCopyStyle(const PilatesNodeRef dstNode, const PilatesNodeRef srcNode);

#define Pilates_NODE_PROPERTY(type, name, paramName)                          \
  WIN_EXPORT void PilatesNodeSet##name(const PilatesNodeRef node, type paramName); \
  WIN_EXPORT type PilatesNodeGet##name(const PilatesNodeRef node);

#define Pilates_NODE_STYLE_PROPERTY(type, name, paramName)                               \
  WIN_EXPORT void PilatesNodeStyleSet##name(const PilatesNodeRef node, const type paramName); \
  WIN_EXPORT type PilatesNodeStyleGet##name(const PilatesNodeRef node);

#define Pilates_NODE_STYLE_PROPERTY_UNIT(type, name, paramName)                                    \
  WIN_EXPORT void PilatesNodeStyleSet##name(const PilatesNodeRef node, const float paramName);          \
  WIN_EXPORT void PilatesNodeStyleSet##name##Percent(const PilatesNodeRef node, const float paramName); \
  WIN_EXPORT type PilatesNodeStyleGet##name(const PilatesNodeRef node);

#define Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO(type, name, paramName) \
  Pilates_NODE_STYLE_PROPERTY_UNIT(type, name, paramName)            \
  WIN_EXPORT void PilatesNodeStyleSet##name##Auto(const PilatesNodeRef node);

#define Pilates_NODE_STYLE_EDGE_PROPERTY(type, name, paramName)    \
  WIN_EXPORT void PilatesNodeStyleSet##name(const PilatesNodeRef node,  \
                                       const PilatesEdge edge,     \
                                       const type paramName); \
  WIN_EXPORT type PilatesNodeStyleGet##name(const PilatesNodeRef node, const PilatesEdge edge);

#define Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT(type, name, paramName)         \
  WIN_EXPORT void PilatesNodeStyleSet##name(const PilatesNodeRef node,            \
                                       const PilatesEdge edge,               \
                                       const float paramName);          \
  WIN_EXPORT void PilatesNodeStyleSet##name##Percent(const PilatesNodeRef node,   \
                                                const PilatesEdge edge,      \
                                                const float paramName); \
  WIN_EXPORT WIN_STRUCT(type) PilatesNodeStyleGet##name(const PilatesNodeRef node, const PilatesEdge edge);

#define Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO(type, name) \
  WIN_EXPORT void PilatesNodeStyleSet##name##Auto(const PilatesNodeRef node, const PilatesEdge edge);

#define Pilates_NODE_LAYOUT_PROPERTY(type, name) \
  WIN_EXPORT type PilatesNodeLayoutGet##name(const PilatesNodeRef node);

#define Pilates_NODE_LAYOUT_EDGE_PROPERTY(type, name) \
  WIN_EXPORT type PilatesNodeLayoutGet##name(const PilatesNodeRef node, const PilatesEdge edge);

void* PilatesNodeGetContext(PilatesNodeRef node);
void PilatesNodeSetContext(PilatesNodeRef node, void* context);
PilatesMeasureFunc PilatesNodeGetMeasureFunc(PilatesNodeRef node);
void PilatesNodeSetMeasureFunc(PilatesNodeRef node, PilatesMeasureFunc measureFunc);
PilatesBaselineFunc PilatesNodeGetBaselineFunc(PilatesNodeRef node);
void PilatesNodeSetBaselineFunc(PilatesNodeRef node, PilatesBaselineFunc baselineFunc);
PilatesDirtiedFunc PilatesNodeGetDirtiedFunc(PilatesNodeRef node);
void PilatesNodeSetDirtiedFunc(PilatesNodeRef node, PilatesDirtiedFunc dirtiedFunc);
PilatesPrintFunc PilatesNodeGetPrintFunc(PilatesNodeRef node);
void PilatesNodeSetPrintFunc(PilatesNodeRef node, PilatesPrintFunc printFunc);
bool PilatesNodeGetHasNewLayout(PilatesNodeRef node);
void PilatesNodeSetHasNewLayout(PilatesNodeRef node, bool hasNewLayout);
PilatesNodeType PilatesNodeGetNodeType(PilatesNodeRef node);
void PilatesNodeSetNodeType(PilatesNodeRef node, PilatesNodeType nodeType);
bool PilatesNodeIsDirty(PilatesNodeRef node);
bool PilatesNodeLayoutGetDidUseLegacyFlag(const PilatesNodeRef node);

Pilates_NODE_STYLE_PROPERTY(PilatesDirection, Direction, direction);
Pilates_NODE_STYLE_PROPERTY(PilatesFlexDirection, FlexDirection, flexDirection);
Pilates_NODE_STYLE_PROPERTY(PilatesJustify, JustifyContent, justifyContent);
Pilates_NODE_STYLE_PROPERTY(PilatesAlign, AlignContent, alignContent);
Pilates_NODE_STYLE_PROPERTY(PilatesAlign, AlignItems, alignItems);
Pilates_NODE_STYLE_PROPERTY(PilatesAlign, AlignSelf, alignSelf);
Pilates_NODE_STYLE_PROPERTY(PilatesPositionType, PositionType, positionType);
Pilates_NODE_STYLE_PROPERTY(PilatesWrap, FlexWrap, flexWrap);
Pilates_NODE_STYLE_PROPERTY(PilatesOverflow, Overflow, overflow);
Pilates_NODE_STYLE_PROPERTY(PilatesDisplay, Display, display);

Pilates_NODE_STYLE_PROPERTY(float, Flex, flex);
Pilates_NODE_STYLE_PROPERTY(float, FlexGrow, flexGrow);
Pilates_NODE_STYLE_PROPERTY(float, FlexShrink, flexShrink);
Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO(PilatesValue, FlexBasis, flexBasis);

Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT(PilatesValue, Position, position);
Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT(PilatesValue, Margin, margin);
Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO(PilatesValue, Margin);
Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT(PilatesValue, Padding, padding);
Pilates_NODE_STYLE_EDGE_PROPERTY(float, Border, border);

Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO(PilatesValue, Width, width);
Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO(PilatesValue, Height, height);
Pilates_NODE_STYLE_PROPERTY_UNIT(PilatesValue, MinWidth, minWidth);
Pilates_NODE_STYLE_PROPERTY_UNIT(PilatesValue, MinHeight, minHeight);
Pilates_NODE_STYLE_PROPERTY_UNIT(PilatesValue, MaxWidth, maxWidth);
Pilates_NODE_STYLE_PROPERTY_UNIT(PilatesValue, MaxHeight, maxHeight);

// Pilates specific properties, not compatible with flexbox specification
// Aspect ratio control the size of the undefined dimension of a node.
// Aspect ratio is encoded as a floating point value width/height. e.g. A value of 2 leads to a node
// with a width twice the size of its height while a value of 0.5 gives the opposite effect.
//
// - On a node with a set width/height aspect ratio control the size of the unset dimension
// - On a node with a set flex basis aspect ratio controls the size of the node in the cross axis if
// unset
// - On a node with a measure function aspect ratio works as though the measure function measures
// the flex basis
// - On a node with flex grow/shrink aspect ratio controls the size of the node in the cross axis if
// unset
// - Aspect ratio takes min/max dimensions into account
Pilates_NODE_STYLE_PROPERTY(float, AspectRatio, aspectRatio);

Pilates_NODE_LAYOUT_PROPERTY(float, Left);
Pilates_NODE_LAYOUT_PROPERTY(float, Top);
Pilates_NODE_LAYOUT_PROPERTY(float, Right);
Pilates_NODE_LAYOUT_PROPERTY(float, Bottom);
Pilates_NODE_LAYOUT_PROPERTY(float, Width);
Pilates_NODE_LAYOUT_PROPERTY(float, Height);
Pilates_NODE_LAYOUT_PROPERTY(PilatesDirection, Direction);
Pilates_NODE_LAYOUT_PROPERTY(bool, HadOverflow);

// Get the computed values for these nodes after performing layout. If they were set using
// point values then the returned value will be the same as PilatesNodeStyleGetXXX. However if
// they were set using a percentage value then the returned value is the computed value used
// during layout.
Pilates_NODE_LAYOUT_EDGE_PROPERTY(float, Margin);
Pilates_NODE_LAYOUT_EDGE_PROPERTY(float, Border);
Pilates_NODE_LAYOUT_EDGE_PROPERTY(float, Padding);

WIN_EXPORT void PilatesConfigSetLogger(const PilatesConfigRef config, PilatesLogger logger);
WIN_EXPORT void PilatesLog(const PilatesNodeRef node, PilatesLogLevel level, const char *message, ...);
WIN_EXPORT void PilatesLogWithConfig(const PilatesConfigRef config, PilatesLogLevel level, const char *format, ...);
WIN_EXPORT void PilatesAssert(const bool condition, const char *message);
WIN_EXPORT void PilatesAssertWithNode(const PilatesNodeRef node, const bool condition, const char *message);
WIN_EXPORT void PilatesAssertWithConfig(const PilatesConfigRef config,
                                   const bool condition,
                                   const char *message);

// Set this to number of pixels in 1 point to round calculation results
// If you want to avoid rounding - set PointScaleFactor to 0
WIN_EXPORT void PilatesConfigSetPointScaleFactor(const PilatesConfigRef config, const float pixelsInPoint);

// Pilates previously had an error where containers would take the maximum space possible instead of
// the minimum
// like they are supposed to. In practice this resulted in implicit behaviour similar to align-self:
// stretch;
// Because this was such a long-standing bug we must allow legacy users to switch back to this
// behaviour.
WIN_EXPORT void PilatesConfigSetUseLegacyStretchBehaviour(const PilatesConfigRef config,
                                                     const bool useLegacyStretchBehaviour);

// PilatesConfig
WIN_EXPORT PilatesConfigRef PilatesConfigNew(void);
WIN_EXPORT void PilatesConfigFree(const PilatesConfigRef config);
WIN_EXPORT void PilatesConfigCopy(const PilatesConfigRef dest, const PilatesConfigRef src);
WIN_EXPORT int32_t PilatesConfigGetInstanceCount(void);

WIN_EXPORT void PilatesConfigSetExperimentalFeatureEnabled(const PilatesConfigRef config,
                                                      const PilatesExperimentalFeature feature,
                                                      const bool enabled);
WIN_EXPORT bool PilatesConfigIsExperimentalFeatureEnabled(const PilatesConfigRef config,
                                                     const PilatesExperimentalFeature feature);

// Using the web defaults is the prefered configuration for new projects.
// Usage of non web defaults should be considered as legacy.
WIN_EXPORT void PilatesConfigSetUseWebDefaults(const PilatesConfigRef config, const bool enabled);
WIN_EXPORT bool PilatesConfigGetUseWebDefaults(const PilatesConfigRef config);

WIN_EXPORT void PilatesConfigSetNodeClonedFunc(const PilatesConfigRef config,
                                          const PilatesNodeClonedFunc callback);

// Export only for C#
WIN_EXPORT PilatesConfigRef PilatesConfigGetDefault(void);

WIN_EXPORT void PilatesConfigSetContext(const PilatesConfigRef config, void *context);
WIN_EXPORT void *PilatesConfigGetContext(const PilatesConfigRef config);

WIN_EXPORT float PilatesRoundValueToPixelGrid(
    const float value,
    const float pointScaleFactor,
    const bool forceCeil,
    const bool forceFloor);

Pilates_EXTERN_C_END
