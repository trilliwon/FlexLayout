/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Pilates.h"
#include <string.h>
#include <algorithm>
#include "Utils.h"
#include "PilatesNode.h"
#include "PilatesNodePrint.h"
#include "Pilates-internal.h"

#ifdef _MSC_VER
#include <float.h>

/* define fmaxf if < VC12 */
#if _MSC_VER < 1800
__forceinline const float fmaxf(const float a, const float b) {
  return (a > b) ? a : b;
}
#endif
#endif

#ifdef ANDROID
static int PilatesAndroidLog(const PilatesConfigRef config,
                        const PilatesNodeRef node,
                        PilatesLogLevel level,
                        const char *format,
                        va_list args);
#else
static int PilatesDefaultLog(const PilatesConfigRef config,
                        const PilatesNodeRef node,
                        PilatesLogLevel level,
                        const char *format,
                        va_list args);
#endif

static PilatesConfig gPilatesConfigDefaults = {
    .experimentalFeatures =
        {
            [PilatesExperimentalFeatureWebFlexBasis] = false,
        },
    .useWebDefaults = false,
    .useLegacyStretchBehaviour = false,
    .shouldDiffLayoutWithoutLegacyStretchBehaviour = false,
    .pointScaleFactor = 1.0f,
#ifdef ANDROID
    .logger = &PilatesAndroidLog,
#else
    .logger = &PilatesDefaultLog,
#endif
    .cloneNodeCallback = nullptr,
    .context = nullptr,
};

const PilatesValue PilatesValueZero = {.value = 0, .unit = PilatesUnitPoint};
const PilatesValue PilatesValueUndefined = {PilatesUndefined, PilatesUnitUndefined};
const PilatesValue PilatesValueAuto = {PilatesUndefined, PilatesUnitAuto};

#ifdef ANDROID
#include <android/log.h>
static int PilatesAndroidLog(const PilatesConfigRef config,
                        const PilatesNodeRef node,
                        PilatesLogLevel level,
                        const char *format,
                        va_list args) {
  int androidLevel = PilatesLogLevelDebug;
  switch (level) {
    case PilatesLogLevelFatal:
      androidLevel = ANDROID_LOG_FATAL;
      break;
    case PilatesLogLevelError:
      androidLevel = ANDROID_LOG_ERROR;
      break;
    case PilatesLogLevelWarn:
      androidLevel = ANDROID_LOG_WARN;
      break;
    case PilatesLogLevelInfo:
      androidLevel = ANDROID_LOG_INFO;
      break;
    case PilatesLogLevelDebug:
      androidLevel = ANDROID_LOG_DEBUG;
      break;
    case PilatesLogLevelVerbose:
      androidLevel = ANDROID_LOG_VERBOSE;
      break;
  }
  const int result = __android_log_vprint(androidLevel, "pilates", format, args);
  return result;
}
#else
#define Pilates_UNUSED(x) (void)(x);

static int PilatesDefaultLog(const PilatesConfigRef config,
                        const PilatesNodeRef node,
                        PilatesLogLevel level,
                        const char *format,
                        va_list args) {
  Pilates_UNUSED(config);
  Pilates_UNUSED(node);
  switch (level) {
    case PilatesLogLevelError:
    case PilatesLogLevelFatal:
      return vfprintf(stderr, format, args);
    case PilatesLogLevelWarn:
    case PilatesLogLevelInfo:
    case PilatesLogLevelDebug:
    case PilatesLogLevelVerbose:
    default:
      return vprintf(format, args);
  }
}

#undef Pilates_UNUSED
#endif

bool PilatesFloatIsUndefined(const float value) {
  return std::isnan(value);
}

const PilatesValue* PilatesComputedEdgeValue(
    const std::array<PilatesValue, PilatesEdgeCount>& edges,
    const PilatesEdge edge,
    const PilatesValue* const defaultValue) {
  if (edges[edge].unit != PilatesUnitUndefined) {
    return &edges[edge];
  }

  if ((edge == PilatesEdgeTop || edge == PilatesEdgeBottom) &&
      edges[PilatesEdgeVertical].unit != PilatesUnitUndefined) {
    return &edges[PilatesEdgeVertical];
  }

  if ((edge == PilatesEdgeLeft || edge == PilatesEdgeRight || edge == PilatesEdgeStart || edge == PilatesEdgeEnd) &&
      edges[PilatesEdgeHorizontal].unit != PilatesUnitUndefined) {
    return &edges[PilatesEdgeHorizontal];
  }

  if (edges[PilatesEdgeAll].unit != PilatesUnitUndefined) {
    return &edges[PilatesEdgeAll];
  }

  if (edge == PilatesEdgeStart || edge == PilatesEdgeEnd) {
    return &PilatesValueUndefined;
  }

  return defaultValue;
}

void* PilatesNodeGetContext(PilatesNodeRef node) {
  return node->getContext();
}

void PilatesNodeSetContext(PilatesNodeRef node, void* context) {
  return node->setContext(context);
}

PilatesMeasureFunc PilatesNodeGetMeasureFunc(PilatesNodeRef node) {
  return node->getMeasure();
}

void PilatesNodeSetMeasureFunc(PilatesNodeRef node, PilatesMeasureFunc measureFunc) {
  node->setMeasureFunc(measureFunc);
}

PilatesBaselineFunc PilatesNodeGetBaselineFunc(PilatesNodeRef node) {
  return node->getBaseline();
}

void PilatesNodeSetBaselineFunc(PilatesNodeRef node, PilatesBaselineFunc baselineFunc) {
  node->setBaseLineFunc(baselineFunc);
}

PilatesDirtiedFunc PilatesNodeGetDirtiedFunc(PilatesNodeRef node) {
  return node->getDirtied();
}

void PilatesNodeSetDirtiedFunc(PilatesNodeRef node, PilatesDirtiedFunc dirtiedFunc) {
  node->setDirtiedFunc(dirtiedFunc);
}

PilatesPrintFunc PilatesNodeGetPrintFunc(PilatesNodeRef node) {
  return node->getPrintFunc();
}

void PilatesNodeSetPrintFunc(PilatesNodeRef node, PilatesPrintFunc printFunc) {
  node->setPrintFunc(printFunc);
}

bool PilatesNodeGetHasNewLayout(PilatesNodeRef node) {
  return node->getHasNewLayout();
}

void PilatesNodeSetHasNewLayout(PilatesNodeRef node, bool hasNewLayout) {
  node->setHasNewLayout(hasNewLayout);
}

PilatesNodeType PilatesNodeGetNodeType(PilatesNodeRef node) {
  return node->getNodeType();
}

void PilatesNodeSetNodeType(PilatesNodeRef node, PilatesNodeType nodeType) {
  return node->setNodeType(nodeType);
}

bool PilatesNodeIsDirty(PilatesNodeRef node) {
  return node->isDirty();
}

bool PilatesNodeLayoutGetDidUseLegacyFlag(const PilatesNodeRef node) {
  return node->didUseLegacyFlag();
}

void PilatesNodeMarkDirtyAndPropogateToDescendants(const PilatesNodeRef node) {
  return node->markDirtyAndPropogateDownwards();
}

int32_t gNodeInstanceCount = 0;
int32_t gConfigInstanceCount = 0;

WIN_EXPORT PilatesNodeRef PilatesNodeNewWithConfig(const PilatesConfigRef config) {
  const PilatesNodeRef node = new PilatesNode();
  PilatesAssertWithConfig(
      config, node != nullptr, "Could not allocate memory for node");
  gNodeInstanceCount++;

  if (config->useWebDefaults) {
    node->setStyleFlexDirection(PilatesFlexDirectionRow);
    node->setStyleAlignContent(PilatesAlignStretch);
  }
  node->setConfig(config);
  return node;
}

PilatesNodeRef PilatesNodeNew(void) {
  return PilatesNodeNewWithConfig(&gPilatesConfigDefaults);
}

PilatesNodeRef PilatesNodeClone(PilatesNodeRef oldNode) {
  PilatesNodeRef node = new PilatesNode(*oldNode);
  PilatesAssertWithConfig(
      oldNode->getConfig(),
      node != nullptr,
      "Could not allocate memory for node");
  gNodeInstanceCount++;
  node->setParent(nullptr);
  return node;
}

static PilatesConfigRef PilatesConfigClone(const PilatesConfig& oldConfig) {
  const PilatesConfigRef config = new PilatesConfig(oldConfig);
  PilatesAssert(config != nullptr, "Could not allocate memory for config");
  if (config == nullptr) {
    abort();
  }
  gConfigInstanceCount++;
  return config;
}

static PilatesNodeRef PilatesNodeDeepClone(PilatesNodeRef oldNode) {
  PilatesNodeRef node = PilatesNodeClone(oldNode);
  PilatesVector vec = PilatesVector();
  vec.reserve(oldNode->getChildren().size());
  PilatesNodeRef childNode = nullptr;
  for (auto& item : oldNode->getChildren()) {
    childNode = PilatesNodeDeepClone(item);
    childNode->setParent(node);
    vec.push_back(childNode);
  }
  node->setChildren(vec);

  if (oldNode->getConfig() != nullptr) {
    node->setConfig(PilatesConfigClone(*(oldNode->getConfig())));
  }

  if (oldNode->getNextChild() != nullptr) {
    node->setNextChild(PilatesNodeDeepClone(oldNode->getNextChild()));
  }

  return node;
}

void PilatesNodeFree(const PilatesNodeRef node) {
  if (node->getParent()) {
    node->getParent()->removeChild(node);
    node->setParent(nullptr);
  }

  const uint32_t childCount = PilatesNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const PilatesNodeRef child = PilatesNodeGetChild(node, i);
    child->setParent(nullptr);
  }

  node->clearChildren();
  delete node;
  gNodeInstanceCount--;
}

static void PilatesConfigFreeRecursive(const PilatesNodeRef root) {
  if (root->getConfig() != nullptr) {
    gConfigInstanceCount--;
    delete root->getConfig();
  }
  // Delete configs recursively for childrens
  for (uint32_t i = 0; i < root->getChildrenCount(); ++i) {
    PilatesConfigFreeRecursive(root->getChild(i));
  }
}

void PilatesNodeFreeRecursive(const PilatesNodeRef root) {
  while (PilatesNodeGetChildCount(root) > 0) {
    const PilatesNodeRef child = PilatesNodeGetChild(root, 0);
    if (child->getParent() != root) {
      // Don't free shared nodes that we don't own.
      break;
    }
    PilatesNodeRemoveChild(root, child);
    PilatesNodeFreeRecursive(child);
  }
  PilatesNodeFree(root);
}

void PilatesNodeReset(const PilatesNodeRef node) {
  PilatesAssertWithNode(node,
                   PilatesNodeGetChildCount(node) == 0,
                   "Cannot reset a node which still has children attached");
  PilatesAssertWithNode(
      node,
      node->getParent() == nullptr,
      "Cannot reset a node still attached to a parent");

  node->clearChildren();

  const PilatesConfigRef config = node->getConfig();
  *node = PilatesNode();
  if (config->useWebDefaults) {
    node->setStyleFlexDirection(PilatesFlexDirectionRow);
    node->setStyleAlignContent(PilatesAlignStretch);
  }
  node->setConfig(config);
}

int32_t PilatesNodeGetInstanceCount(void) {
  return gNodeInstanceCount;
}

int32_t PilatesConfigGetInstanceCount(void) {
  return gConfigInstanceCount;
}

// Export only for C#
PilatesConfigRef PilatesConfigGetDefault() {
  return &gPilatesConfigDefaults;
}

PilatesConfigRef PilatesConfigNew(void) {
  const PilatesConfigRef config = (const PilatesConfigRef)malloc(sizeof(PilatesConfig));
  PilatesAssert(config != nullptr, "Could not allocate memory for config");
  if (config == nullptr) {
    abort();
  }
  gConfigInstanceCount++;
  memcpy(config, &gPilatesConfigDefaults, sizeof(PilatesConfig));
  return config;
}

void PilatesConfigFree(const PilatesConfigRef config) {
  free(config);
  gConfigInstanceCount--;
}

void PilatesConfigCopy(const PilatesConfigRef dest, const PilatesConfigRef src) {
  memcpy(dest, src, sizeof(PilatesConfig));
}

void PilatesNodeInsertChild(const PilatesNodeRef node, const PilatesNodeRef child, const uint32_t index) {
  PilatesAssertWithNode(
      node,
      child->getParent() == nullptr,
      "Child already has a parent, it must be removed first.");
  PilatesAssertWithNode(
      node,
      node->getMeasure() == nullptr,
      "Cannot add child: Nodes with measure functions cannot have children.");

  node->cloneChildrenIfNeeded();
  node->insertChild(child, index);
  child->setParent(node);
  node->markDirtyAndPropogate();
}

void PilatesNodeRemoveChild(const PilatesNodeRef parent, const PilatesNodeRef excludedChild) {
  // This algorithm is a forked variant from cloneChildrenIfNeeded in PilatesNode
  // that excludes a child.
  const uint32_t childCount = PilatesNodeGetChildCount(parent);

  if (childCount == 0) {
    // This is an empty set. Nothing to remove.
    return;
  }
  const PilatesNodeRef firstChild = PilatesNodeGetChild(parent, 0);
  if (firstChild->getParent() == parent) {
    // If the first child has this node as its parent, we assume that it is already unique.
    // We can now try to delete a child in this list.
    if (parent->removeChild(excludedChild)) {
      excludedChild->setLayout(
          PilatesNode().getLayout()); // layout is no longer valid
      excludedChild->setParent(nullptr);
      parent->markDirtyAndPropogate();
    }
    return;
  }
  // Otherwise we have to clone the node list except for the child we're trying to delete.
  // We don't want to simply clone all children, because then the host will need to free
  // the clone of the child that was just deleted.
  const PilatesNodeClonedFunc cloneNodeCallback =
      parent->getConfig()->cloneNodeCallback;
  uint32_t nextInsertIndex = 0;
  for (uint32_t i = 0; i < childCount; i++) {
    const PilatesNodeRef oldChild = parent->getChild(i);
    if (excludedChild == oldChild) {
      // Ignore the deleted child. Don't reset its layout or parent since it is still valid
      // in the other parent. However, since this parent has now changed, we need to mark it
      // as dirty.
      parent->markDirtyAndPropogate();
      continue;
    }
    const PilatesNodeRef newChild = PilatesNodeClone(oldChild);
    parent->replaceChild(newChild, nextInsertIndex);
    newChild->setParent(parent);
    if (cloneNodeCallback) {
      cloneNodeCallback(oldChild, newChild, parent, nextInsertIndex);
    }
    nextInsertIndex++;
  }
  while (nextInsertIndex < childCount) {
    parent->removeChild(nextInsertIndex);
    nextInsertIndex++;
  }
}

void PilatesNodeRemoveAllChildren(const PilatesNodeRef parent) {
  const uint32_t childCount = PilatesNodeGetChildCount(parent);
  if (childCount == 0) {
    // This is an empty set already. Nothing to do.
    return;
  }
  const PilatesNodeRef firstChild = PilatesNodeGetChild(parent, 0);
  if (firstChild->getParent() == parent) {
    // If the first child has this node as its parent, we assume that this child set is unique.
    for (uint32_t i = 0; i < childCount; i++) {
      const PilatesNodeRef oldChild = PilatesNodeGetChild(parent, i);
      oldChild->setLayout(PilatesNode().getLayout()); // layout is no longer valid
      oldChild->setParent(nullptr);
    }
    parent->clearChildren();
    parent->markDirtyAndPropogate();
    return;
  }
  // Otherwise, we are not the owner of the child set. We don't have to do anything to clear it.
  parent->setChildren(PilatesVector());
  parent->markDirtyAndPropogate();
}

PilatesNodeRef PilatesNodeGetChild(const PilatesNodeRef node, const uint32_t index) {
  if (index < node->getChildren().size()) {
    return node->getChild(index);
  }
  return nullptr;
}

uint32_t PilatesNodeGetChildCount(const PilatesNodeRef node) {
  return static_cast<uint32_t>(node->getChildren().size());
}

PilatesNodeRef PilatesNodeGetParent(const PilatesNodeRef node) {
  return node->getParent();
}

void PilatesNodeMarkDirty(const PilatesNodeRef node) {
  PilatesAssertWithNode(
      node,
      node->getMeasure() != nullptr,
      "Only leaf nodes with custom measure functions"
      "should manually mark themselves as dirty");

  node->markDirtyAndPropogate();
}

void PilatesNodeCopyStyle(const PilatesNodeRef dstNode, const PilatesNodeRef srcNode) {
  if (!(dstNode->getStyle() == srcNode->getStyle())) {
    dstNode->setStyle(srcNode->getStyle());
    dstNode->markDirtyAndPropogate();
  }
}

float PilatesNodeStyleGetFlexGrow(const PilatesNodeRef node) {
  return PilatesFloatIsUndefined(node->getStyle().flexGrow)
      ? kDefaultFlexGrow
      : node->getStyle().flexGrow;
}

float PilatesNodeStyleGetFlexShrink(const PilatesNodeRef node) {
  return PilatesFloatIsUndefined(node->getStyle().flexShrink)
      ? (node->getConfig()->useWebDefaults ? kWebDefaultFlexShrink
                                           : kDefaultFlexShrink)
      : node->getStyle().flexShrink;
}

#define Pilates_NODE_STYLE_PROPERTY_SETTER_IMPL(                               \
    type, name, paramName, instanceName)                                  \
  void PilatesNodeStyleSet##name(const PilatesNodeRef node, const type paramName) { \
    if (node->getStyle().instanceName != paramName) {                     \
      PilatesStyle style = node->getStyle();                                   \
      style.instanceName = paramName;                                     \
      node->setStyle(style);                                              \
      node->markDirtyAndPropogate();                                      \
    }                                                                     \
  }

#define Pilates_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(                               \
    type, name, paramName, instanceName)                                       \
  void PilatesNodeStyleSet##name(const PilatesNodeRef node, const type paramName) {      \
    PilatesValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = PilatesFloatIsUndefined(paramName) ? PilatesUnitUndefined : PilatesUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != PilatesUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void PilatesNodeStyleSet##name##Percent(                                          \
      const PilatesNodeRef node, const type paramName) {                            \
    PilatesValue value = {                                                          \
        .value = paramName,                                                    \
        .unit =                                                                \
            PilatesFloatIsUndefined(paramName) ? PilatesUnitUndefined : PilatesUnitPercent,   \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != PilatesUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
      PilatesStyle style = node->getStyle();                                        \
                                                                               \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }

#define Pilates_NODE_STYLE_PROPERTY_SETTER_UNIT_AUTO_IMPL(                          \
    type, name, paramName, instanceName)                                       \
  void PilatesNodeStyleSet##name(const PilatesNodeRef node, const type paramName) {      \
    PilatesValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = PilatesFloatIsUndefined(paramName) ? PilatesUnitUndefined : PilatesUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != PilatesUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void PilatesNodeStyleSet##name##Percent(                                          \
      const PilatesNodeRef node, const type paramName) {                            \
    if (node->getStyle().instanceName.value != paramName ||                    \
        node->getStyle().instanceName.unit != PilatesUnitPercent) {                 \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName.value = paramName;                                    \
      style.instanceName.unit =                                                \
          PilatesFloatIsUndefined(paramName) ? PilatesUnitAuto : PilatesUnitPercent;          \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void PilatesNodeStyleSet##name##Auto(const PilatesNodeRef node) {                      \
    if (node->getStyle().instanceName.unit != PilatesUnitAuto) {                    \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName.value = PilatesUndefined;                                  \
      style.instanceName.unit = PilatesUnitAuto;                                    \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }

#define Pilates_NODE_STYLE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
  Pilates_NODE_STYLE_PROPERTY_SETTER_IMPL(type, name, paramName, instanceName) \
                                                                          \
  type PilatesNodeStyleGet##name(const PilatesNodeRef node) {                       \
    return node->getStyle().instanceName;                                 \
  }

#define Pilates_NODE_STYLE_PROPERTY_UNIT_IMPL(type, name, paramName, instanceName) \
  Pilates_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(                                    \
      float, name, paramName, instanceName)                                   \
                                                                              \
  type PilatesNodeStyleGet##name(const PilatesNodeRef node) {                           \
    return node->getStyle().instanceName;                                     \
  }

#define Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(      \
    type, name, paramName, instanceName)            \
  Pilates_NODE_STYLE_PROPERTY_SETTER_UNIT_AUTO_IMPL(     \
      float, name, paramName, instanceName)         \
                                                    \
  type PilatesNodeStyleGet##name(const PilatesNodeRef node) { \
    return node->getStyle().instanceName;           \
  }

#define Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO_IMPL(type, name, instanceName) \
  void PilatesNodeStyleSet##name##Auto(const PilatesNodeRef node, const PilatesEdge edge) { \
    if (node->getStyle().instanceName[edge].unit != PilatesUnitAuto) {            \
      PilatesStyle style = node->getStyle();                                      \
      style.instanceName[edge].value = PilatesUndefined;                          \
      style.instanceName[edge].unit = PilatesUnitAuto;                            \
      node->setStyle(style);                                                 \
      node->markDirtyAndPropogate();                                         \
    }                                                                        \
  }

#define Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(                                 \
    type, name, paramName, instanceName)                                       \
  void PilatesNodeStyleSet##name(                                                   \
      const PilatesNodeRef node, const PilatesEdge edge, const float paramName) {        \
    PilatesValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = PilatesFloatIsUndefined(paramName) ? PilatesUnitUndefined : PilatesUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != PilatesUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void PilatesNodeStyleSet##name##Percent(                                          \
      const PilatesNodeRef node, const PilatesEdge edge, const float paramName) {        \
    PilatesValue value = {                                                          \
        .value = paramName,                                                    \
        .unit =                                                                \
            PilatesFloatIsUndefined(paramName) ? PilatesUnitUndefined : PilatesUnitPercent,   \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != PilatesUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  WIN_STRUCT(type)                                                             \
  PilatesNodeStyleGet##name(const PilatesNodeRef node, const PilatesEdge edge) {              \
    return WIN_STRUCT_REF(node->getStyle().instanceName[edge]);                \
  }

#define Pilates_NODE_STYLE_EDGE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
  void PilatesNodeStyleSet##name(                                                   \
      const PilatesNodeRef node, const PilatesEdge edge, const float paramName) {        \
    PilatesValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = PilatesFloatIsUndefined(paramName) ? PilatesUnitUndefined : PilatesUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != PilatesUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
      PilatesStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  float PilatesNodeStyleGet##name(const PilatesNodeRef node, const PilatesEdge edge) {        \
    return node->getStyle().instanceName[edge].value;                          \
  }

#define Pilates_NODE_LAYOUT_PROPERTY_IMPL(type, name, instanceName) \
  type PilatesNodeLayoutGet##name(const PilatesNodeRef node) {           \
    return node->getLayout().instanceName;                     \
  }

#define Pilates_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(type, name, instanceName) \
  type PilatesNodeLayoutGet##name(const PilatesNodeRef node, const PilatesEdge edge) { \
    PilatesAssertWithNode(                                                   \
        node,                                                           \
        edge <= PilatesEdgeEnd,                                              \
        "Cannot get layout properties of multi-edge shorthands");       \
                                                                        \
    if (edge == PilatesEdgeLeft) {                                           \
      if (node->getLayout().direction == PilatesDirectionRTL) {              \
        return node->getLayout().instanceName[PilatesEdgeEnd];               \
      } else {                                                          \
        return node->getLayout().instanceName[PilatesEdgeStart];             \
      }                                                                 \
    }                                                                   \
                                                                        \
    if (edge == PilatesEdgeRight) {                                          \
      if (node->getLayout().direction == PilatesDirectionRTL) {              \
        return node->getLayout().instanceName[PilatesEdgeStart];             \
      } else {                                                          \
        return node->getLayout().instanceName[PilatesEdgeEnd];               \
      }                                                                 \
    }                                                                   \
                                                                        \
    return node->getLayout().instanceName[edge];                        \
  }

// Pilates_NODE_PROPERTY_IMPL(void *, Context, context, context);
// Pilates_NODE_PROPERTY_IMPL(PilatesPrintFunc, PrintFunc, printFunc, print);
// Pilates_NODE_PROPERTY_IMPL(bool, HasNewLayout, hasNewLayout, hasNewLayout);
// Pilates_NODE_PROPERTY_IMPL(PilatesNodeType, NodeType, nodeType, nodeType);

Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesDirection, Direction, direction, direction);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesFlexDirection, FlexDirection, flexDirection, flexDirection);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesJustify, JustifyContent, justifyContent, justifyContent);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesAlign, AlignContent, alignContent, alignContent);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesAlign, AlignItems, alignItems, alignItems);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesAlign, AlignSelf, alignSelf, alignSelf);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesPositionType, PositionType, positionType, positionType);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesWrap, FlexWrap, flexWrap, flexWrap);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesOverflow, Overflow, overflow, overflow);
Pilates_NODE_STYLE_PROPERTY_IMPL(PilatesDisplay, Display, display, display);

Pilates_NODE_STYLE_PROPERTY_IMPL(float, Flex, flex, flex);
Pilates_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexGrow, flexGrow, flexGrow);
Pilates_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexShrink, flexShrink, flexShrink);
Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(PilatesValue, FlexBasis, flexBasis, flexBasis);

Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(PilatesValue, Position, position, position);
Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(PilatesValue, Margin, margin, margin);
Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO_IMPL(PilatesValue, Margin, margin);
Pilates_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(PilatesValue, Padding, padding, padding);
Pilates_NODE_STYLE_EDGE_PROPERTY_IMPL(float, Border, border, border);

Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(PilatesValue, Width, width, dimensions[PilatesDimensionWidth]);
Pilates_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(PilatesValue, Height, height, dimensions[PilatesDimensionHeight]);
Pilates_NODE_STYLE_PROPERTY_UNIT_IMPL(PilatesValue, MinWidth, minWidth, minDimensions[PilatesDimensionWidth]);
Pilates_NODE_STYLE_PROPERTY_UNIT_IMPL(PilatesValue, MinHeight, minHeight, minDimensions[PilatesDimensionHeight]);
Pilates_NODE_STYLE_PROPERTY_UNIT_IMPL(PilatesValue, MaxWidth, maxWidth, maxDimensions[PilatesDimensionWidth]);
Pilates_NODE_STYLE_PROPERTY_UNIT_IMPL(PilatesValue, MaxHeight, maxHeight, maxDimensions[PilatesDimensionHeight]);

// Pilates specific properties, not compatible with flexbox specification
Pilates_NODE_STYLE_PROPERTY_IMPL(float, AspectRatio, aspectRatio, aspectRatio);

Pilates_NODE_LAYOUT_PROPERTY_IMPL(float, Left, position[PilatesEdgeLeft]);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(float, Top, position[PilatesEdgeTop]);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(float, Right, position[PilatesEdgeRight]);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(float, Bottom, position[PilatesEdgeBottom]);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(float, Width, dimensions[PilatesDimensionWidth]);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(float, Height, dimensions[PilatesDimensionHeight]);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(PilatesDirection, Direction, direction);
Pilates_NODE_LAYOUT_PROPERTY_IMPL(bool, HadOverflow, hadOverflow);

Pilates_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Margin, margin);
Pilates_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Border, border);
Pilates_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Padding, padding);

uint32_t gCurrentGenerationCount = 0;

bool PilatesLayoutNodeInternal(const PilatesNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const PilatesDirection parentDirection,
                          const PilatesMeasureMode widthMeasureMode,
                          const PilatesMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason,
                          const PilatesConfigRef config);

bool PilatesFloatsEqualWithPrecision(const float a, const float b, const float precision) {
  assert(precision > 0);

  if (PilatesFloatIsUndefined(a)) {
    return PilatesFloatIsUndefined(b);
  }
  return fabs(a - b) < precision;
}

bool PilatesFloatsEqual(const float a, const float b) {
  return PilatesFloatsEqualWithPrecision(a, b, 0.0001f);
}

static void PilatesNodePrintInternal(const PilatesNodeRef node,
                                const PilatesPrintOptions options) {
  std::string str;
  facebook::pilates::PilatesNodeToString(&str, node, options, 0);
  PilatesLog(node, PilatesLogLevelDebug, str.c_str());
}

void PilatesNodePrint(const PilatesNodeRef node, const PilatesPrintOptions options) {
  PilatesNodePrintInternal(node, options);
}

const std::array<PilatesEdge, 4> leading = {
    {PilatesEdgeTop, PilatesEdgeBottom, PilatesEdgeLeft, PilatesEdgeRight}};

const std::array<PilatesEdge, 4> trailing = {
    {PilatesEdgeBottom, PilatesEdgeTop, PilatesEdgeRight, PilatesEdgeLeft}};
static const std::array<PilatesEdge, 4> pos = {{
    PilatesEdgeTop,
    PilatesEdgeBottom,
    PilatesEdgeLeft,
    PilatesEdgeRight,
}};

static const std::array<PilatesDimension, 4> dim = {
    {PilatesDimensionHeight, PilatesDimensionHeight, PilatesDimensionWidth, PilatesDimensionWidth}};

static inline float PilatesNodePaddingAndBorderForAxis(const PilatesNodeRef node,
                                                  const PilatesFlexDirection axis,
                                                  const float widthSize) {
  return node->getLeadingPaddingAndBorder(axis, widthSize) +
      node->getTrailingPaddingAndBorder(axis, widthSize);
}

static inline PilatesAlign PilatesNodeAlignItem(const PilatesNodeRef node, const PilatesNodeRef child) {
  const PilatesAlign align = child->getStyle().alignSelf == PilatesAlignAuto
      ? node->getStyle().alignItems
      : child->getStyle().alignSelf;
  if (align == PilatesAlignBaseline &&
      PilatesFlexDirectionIsColumn(node->getStyle().flexDirection)) {
    return PilatesAlignFlexStart;
  }
  return align;
}

static float PilatesBaseline(const PilatesNodeRef node) {
  if (node->getBaseline() != nullptr) {
    const float baseline = node->getBaseline()(
        node,
        node->getLayout().measuredDimensions[PilatesDimensionWidth],
        node->getLayout().measuredDimensions[PilatesDimensionHeight]);
    PilatesAssertWithNode(node,
                     !PilatesFloatIsUndefined(baseline),
                     "Expect custom baseline function to not return NaN");
    return baseline;
  }

  PilatesNodeRef baselineChild = nullptr;
  const uint32_t childCount = PilatesNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const PilatesNodeRef child = PilatesNodeGetChild(node, i);
    if (child->getLineIndex() > 0) {
      break;
    }
    if (child->getStyle().positionType == PilatesPositionTypeAbsolute) {
      continue;
    }
    if (PilatesNodeAlignItem(node, child) == PilatesAlignBaseline) {
      baselineChild = child;
      break;
    }

    if (baselineChild == nullptr) {
      baselineChild = child;
    }
  }

  if (baselineChild == nullptr) {
    return node->getLayout().measuredDimensions[PilatesDimensionHeight];
  }

  const float baseline = PilatesBaseline(baselineChild);
  return baseline + baselineChild->getLayout().position[PilatesEdgeTop];
}

static bool PilatesIsBaselineLayout(const PilatesNodeRef node) {
  if (PilatesFlexDirectionIsColumn(node->getStyle().flexDirection)) {
    return false;
  }
  if (node->getStyle().alignItems == PilatesAlignBaseline) {
    return true;
  }
  const uint32_t childCount = PilatesNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const PilatesNodeRef child = PilatesNodeGetChild(node, i);
    if (child->getStyle().positionType == PilatesPositionTypeRelative &&
        child->getStyle().alignSelf == PilatesAlignBaseline) {
      return true;
    }
  }

  return false;
}

static inline float PilatesNodeDimWithMargin(const PilatesNodeRef node,
                                        const PilatesFlexDirection axis,
                                        const float widthSize) {
  return node->getLayout().measuredDimensions[dim[axis]] +
      node->getLeadingMargin(axis, widthSize) +
      node->getTrailingMargin(axis, widthSize);
}

static inline bool PilatesNodeIsStyleDimDefined(const PilatesNodeRef node,
                                           const PilatesFlexDirection axis,
                                           const float parentSize) {
  return !(
      node->getResolvedDimension(dim[axis]).unit == PilatesUnitAuto ||
      node->getResolvedDimension(dim[axis]).unit == PilatesUnitUndefined ||
      (node->getResolvedDimension(dim[axis]).unit == PilatesUnitPoint &&
       node->getResolvedDimension(dim[axis]).value < 0.0f) ||
      (node->getResolvedDimension(dim[axis]).unit == PilatesUnitPercent &&
       (node->getResolvedDimension(dim[axis]).value < 0.0f ||
        PilatesFloatIsUndefined(parentSize))));
}

static inline bool PilatesNodeIsLayoutDimDefined(const PilatesNodeRef node, const PilatesFlexDirection axis) {
  const float value = node->getLayout().measuredDimensions[dim[axis]];
  return !PilatesFloatIsUndefined(value) && value >= 0.0f;
}

static float PilatesNodeBoundAxisWithinMinAndMax(const PilatesNodeRef node,
                                            const PilatesFlexDirection axis,
                                            const float value,
                                            const float axisSize) {
  float min = PilatesUndefined;
  float max = PilatesUndefined;

  if (PilatesFlexDirectionIsColumn(axis)) {
    min = PilatesResolveValue(
        node->getStyle().minDimensions[PilatesDimensionHeight], axisSize);
    max = PilatesResolveValue(
        node->getStyle().maxDimensions[PilatesDimensionHeight], axisSize);
  } else if (PilatesFlexDirectionIsRow(axis)) {
    min = PilatesResolveValue(
        node->getStyle().minDimensions[PilatesDimensionWidth], axisSize);
    max = PilatesResolveValue(
        node->getStyle().maxDimensions[PilatesDimensionWidth], axisSize);
  }

  float boundValue = value;

  if (!PilatesFloatIsUndefined(max) && max >= 0.0f && boundValue > max) {
    boundValue = max;
  }

  if (!PilatesFloatIsUndefined(min) && min >= 0.0f && boundValue < min) {
    boundValue = min;
  }

  return boundValue;
}

// Like PilatesNodeBoundAxisWithinMinAndMax but also ensures that the value doesn't go
// below the
// padding and border amount.
static inline float PilatesNodeBoundAxis(const PilatesNodeRef node,
                                    const PilatesFlexDirection axis,
                                    const float value,
                                    const float axisSize,
                                    const float widthSize) {
  return fmaxf(PilatesNodeBoundAxisWithinMinAndMax(node, axis, value, axisSize),
               PilatesNodePaddingAndBorderForAxis(node, axis, widthSize));
}

static void PilatesNodeSetChildTrailingPosition(const PilatesNodeRef node,
                                           const PilatesNodeRef child,
                                           const PilatesFlexDirection axis) {
  const float size = child->getLayout().measuredDimensions[dim[axis]];
  child->setLayoutPosition(
      node->getLayout().measuredDimensions[dim[axis]] - size -
          child->getLayout().position[pos[axis]],
      trailing[axis]);
}

static void PilatesConstrainMaxSizeForMode(const PilatesNodeRef node,
                                      const enum PilatesFlexDirection axis,
                                      const float parentAxisSize,
                                      const float parentWidth,
                                      PilatesMeasureMode *mode,
                                      float *size) {
  const float maxSize =
      PilatesResolveValue(
          node->getStyle().maxDimensions[dim[axis]], parentAxisSize) +
      node->getMarginForAxis(axis, parentWidth);
  switch (*mode) {
    case PilatesMeasureModeExactly:
    case PilatesMeasureModeAtMost:
      *size = (PilatesFloatIsUndefined(maxSize) || *size < maxSize) ? *size : maxSize;
      break;
    case PilatesMeasureModeUndefined:
      if (!PilatesFloatIsUndefined(maxSize)) {
        *mode = PilatesMeasureModeAtMost;
        *size = maxSize;
      }
      break;
  }
}

static void PilatesNodeComputeFlexBasisForChild(const PilatesNodeRef node,
                                           const PilatesNodeRef child,
                                           const float width,
                                           const PilatesMeasureMode widthMode,
                                           const float height,
                                           const float parentWidth,
                                           const float parentHeight,
                                           const PilatesMeasureMode heightMode,
                                           const PilatesDirection direction,
                                           const PilatesConfigRef config) {
  const PilatesFlexDirection mainAxis =
      PilatesResolveFlexDirection(node->getStyle().flexDirection, direction);
  const bool isMainAxisRow = PilatesFlexDirectionIsRow(mainAxis);
  const float mainAxisSize = isMainAxisRow ? width : height;
  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;

  float childWidth;
  float childHeight;
  PilatesMeasureMode childWidthMeasureMode;
  PilatesMeasureMode childHeightMeasureMode;

  const float resolvedFlexBasis =
      PilatesResolveValue(child->resolveFlexBasisPtr(), mainAxisParentSize);
  const bool isRowStyleDimDefined = PilatesNodeIsStyleDimDefined(child, PilatesFlexDirectionRow, parentWidth);
  const bool isColumnStyleDimDefined =
      PilatesNodeIsStyleDimDefined(child, PilatesFlexDirectionColumn, parentHeight);

  if (!PilatesFloatIsUndefined(resolvedFlexBasis) && !PilatesFloatIsUndefined(mainAxisSize)) {
    if (PilatesFloatIsUndefined(child->getLayout().computedFlexBasis) ||
        (PilatesConfigIsExperimentalFeatureEnabled(
             child->getConfig(), PilatesExperimentalFeatureWebFlexBasis) &&
         child->getLayout().computedFlexBasisGeneration !=
             gCurrentGenerationCount)) {
      child->setLayoutComputedFlexBasis(fmaxf(
          resolvedFlexBasis,
          PilatesNodePaddingAndBorderForAxis(child, mainAxis, parentWidth)));
    }
  } else if (isMainAxisRow && isRowStyleDimDefined) {
    // The width is definite, so use that as the flex basis.
    child->setLayoutComputedFlexBasis(fmaxf(
        PilatesResolveValue(
            child->getResolvedDimension(PilatesDimensionWidth), parentWidth),
        PilatesNodePaddingAndBorderForAxis(child, PilatesFlexDirectionRow, parentWidth)));
  } else if (!isMainAxisRow && isColumnStyleDimDefined) {
    // The height is definite, so use that as the flex basis.
    child->setLayoutComputedFlexBasis(fmaxf(
        PilatesResolveValue(
            child->getResolvedDimension(PilatesDimensionHeight), parentHeight),
        PilatesNodePaddingAndBorderForAxis(
            child, PilatesFlexDirectionColumn, parentWidth)));
  } else {
    // Compute the flex basis and hypothetical main size (i.e. the clamped
    // flex basis).
    childWidth = PilatesUndefined;
    childHeight = PilatesUndefined;
    childWidthMeasureMode = PilatesMeasureModeUndefined;
    childHeightMeasureMode = PilatesMeasureModeUndefined;

    const float marginRow =
        child->getMarginForAxis(PilatesFlexDirectionRow, parentWidth);
    const float marginColumn =
        child->getMarginForAxis(PilatesFlexDirectionColumn, parentWidth);

    if (isRowStyleDimDefined) {
      childWidth =
          PilatesResolveValue(
              child->getResolvedDimension(PilatesDimensionWidth), parentWidth) +
          marginRow;
      childWidthMeasureMode = PilatesMeasureModeExactly;
    }
    if (isColumnStyleDimDefined) {
      childHeight =
          PilatesResolveValue(
              child->getResolvedDimension(PilatesDimensionHeight), parentHeight) +
          marginColumn;
      childHeightMeasureMode = PilatesMeasureModeExactly;
    }

    // The W3C spec doesn't say anything about the 'overflow' property,
    // but all major browsers appear to implement the following logic.
    if ((!isMainAxisRow && node->getStyle().overflow == PilatesOverflowScroll) ||
        node->getStyle().overflow != PilatesOverflowScroll) {
      if (PilatesFloatIsUndefined(childWidth) && !PilatesFloatIsUndefined(width)) {
        childWidth = width;
        childWidthMeasureMode = PilatesMeasureModeAtMost;
      }
    }

    if ((isMainAxisRow && node->getStyle().overflow == PilatesOverflowScroll) ||
        node->getStyle().overflow != PilatesOverflowScroll) {
      if (PilatesFloatIsUndefined(childHeight) && !PilatesFloatIsUndefined(height)) {
        childHeight = height;
        childHeightMeasureMode = PilatesMeasureModeAtMost;
      }
    }

    if (!PilatesFloatIsUndefined(child->getStyle().aspectRatio)) {
      if (!isMainAxisRow && childWidthMeasureMode == PilatesMeasureModeExactly) {
        childHeight = marginColumn +
            (childWidth - marginRow) / child->getStyle().aspectRatio;
        childHeightMeasureMode = PilatesMeasureModeExactly;
      } else if (isMainAxisRow && childHeightMeasureMode == PilatesMeasureModeExactly) {
        childWidth = marginRow +
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
        childWidthMeasureMode = PilatesMeasureModeExactly;
      }
    }

    // If child has no defined size in the cross axis and is set to stretch,
    // set the cross
    // axis to be measured exactly with the available inner width

    const bool hasExactWidth = !PilatesFloatIsUndefined(width) && widthMode == PilatesMeasureModeExactly;
    const bool childWidthStretch = PilatesNodeAlignItem(node, child) == PilatesAlignStretch &&
                                   childWidthMeasureMode != PilatesMeasureModeExactly;
    if (!isMainAxisRow && !isRowStyleDimDefined && hasExactWidth && childWidthStretch) {
      childWidth = width;
      childWidthMeasureMode = PilatesMeasureModeExactly;
      if (!PilatesFloatIsUndefined(child->getStyle().aspectRatio)) {
        childHeight = (childWidth - marginRow) / child->getStyle().aspectRatio;
        childHeightMeasureMode = PilatesMeasureModeExactly;
      }
    }

    const bool hasExactHeight = !PilatesFloatIsUndefined(height) && heightMode == PilatesMeasureModeExactly;
    const bool childHeightStretch = PilatesNodeAlignItem(node, child) == PilatesAlignStretch &&
                                    childHeightMeasureMode != PilatesMeasureModeExactly;
    if (isMainAxisRow && !isColumnStyleDimDefined && hasExactHeight && childHeightStretch) {
      childHeight = height;
      childHeightMeasureMode = PilatesMeasureModeExactly;

      if (!PilatesFloatIsUndefined(child->getStyle().aspectRatio)) {
        childWidth =
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
        childWidthMeasureMode = PilatesMeasureModeExactly;
      }
    }

    PilatesConstrainMaxSizeForMode(
        child, PilatesFlexDirectionRow, parentWidth, parentWidth, &childWidthMeasureMode, &childWidth);
    PilatesConstrainMaxSizeForMode(child,
                              PilatesFlexDirectionColumn,
                              parentHeight,
                              parentWidth,
                              &childHeightMeasureMode,
                              &childHeight);

    // Measure the child
    PilatesLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         parentWidth,
                         parentHeight,
                         false,
                         "measure",
                         config);

    child->setLayoutComputedFlexBasis(fmaxf(
        child->getLayout().measuredDimensions[dim[mainAxis]],
        PilatesNodePaddingAndBorderForAxis(child, mainAxis, parentWidth)));
  }
  child->setLayoutComputedFlexBasisGeneration(gCurrentGenerationCount);
}

static void PilatesNodeAbsoluteLayoutChild(const PilatesNodeRef node,
                                      const PilatesNodeRef child,
                                      const float width,
                                      const PilatesMeasureMode widthMode,
                                      const float height,
                                      const PilatesDirection direction,
                                      const PilatesConfigRef config) {
  const PilatesFlexDirection mainAxis =
      PilatesResolveFlexDirection(node->getStyle().flexDirection, direction);
  const PilatesFlexDirection crossAxis = PilatesFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = PilatesFlexDirectionIsRow(mainAxis);

  float childWidth = PilatesUndefined;
  float childHeight = PilatesUndefined;
  PilatesMeasureMode childWidthMeasureMode = PilatesMeasureModeUndefined;
  PilatesMeasureMode childHeightMeasureMode = PilatesMeasureModeUndefined;

  const float marginRow = child->getMarginForAxis(PilatesFlexDirectionRow, width);
  const float marginColumn =
      child->getMarginForAxis(PilatesFlexDirectionColumn, width);

  if (PilatesNodeIsStyleDimDefined(child, PilatesFlexDirectionRow, width)) {
    childWidth =
        PilatesResolveValue(child->getResolvedDimension(PilatesDimensionWidth), width) +
        marginRow;
  } else {
    // If the child doesn't have a specified width, compute the width based
    // on the left/right
    // offsets if they're defined.
    if (child->isLeadingPositionDefined(PilatesFlexDirectionRow) &&
        child->isTrailingPosDefined(PilatesFlexDirectionRow)) {
      childWidth = node->getLayout().measuredDimensions[PilatesDimensionWidth] -
          (node->getLeadingBorder(PilatesFlexDirectionRow) +
           node->getTrailingBorder(PilatesFlexDirectionRow)) -
          (child->getLeadingPosition(PilatesFlexDirectionRow, width) +
           child->getTrailingPosition(PilatesFlexDirectionRow, width));
      childWidth = PilatesNodeBoundAxis(child, PilatesFlexDirectionRow, childWidth, width, width);
    }
  }

  if (PilatesNodeIsStyleDimDefined(child, PilatesFlexDirectionColumn, height)) {
    childHeight =
        PilatesResolveValue(child->getResolvedDimension(PilatesDimensionHeight), height) +
        marginColumn;
  } else {
    // If the child doesn't have a specified height, compute the height
    // based on the top/bottom
    // offsets if they're defined.
    if (child->isLeadingPositionDefined(PilatesFlexDirectionColumn) &&
        child->isTrailingPosDefined(PilatesFlexDirectionColumn)) {
      childHeight = node->getLayout().measuredDimensions[PilatesDimensionHeight] -
          (node->getLeadingBorder(PilatesFlexDirectionColumn) +
           node->getTrailingBorder(PilatesFlexDirectionColumn)) -
          (child->getLeadingPosition(PilatesFlexDirectionColumn, height) +
           child->getTrailingPosition(PilatesFlexDirectionColumn, height));
      childHeight = PilatesNodeBoundAxis(child, PilatesFlexDirectionColumn, childHeight, height, width);
    }
  }

  // Exactly one dimension needs to be defined for us to be able to do aspect ratio
  // calculation. One dimension being the anchor and the other being flexible.
  if (PilatesFloatIsUndefined(childWidth) ^ PilatesFloatIsUndefined(childHeight)) {
    if (!PilatesFloatIsUndefined(child->getStyle().aspectRatio)) {
      if (PilatesFloatIsUndefined(childWidth)) {
        childWidth = marginRow +
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
      } else if (PilatesFloatIsUndefined(childHeight)) {
        childHeight = marginColumn +
            (childWidth - marginRow) / child->getStyle().aspectRatio;
      }
    }
  }

  // If we're still missing one or the other dimension, measure the content.
  if (PilatesFloatIsUndefined(childWidth) || PilatesFloatIsUndefined(childHeight)) {
    childWidthMeasureMode =
        PilatesFloatIsUndefined(childWidth) ? PilatesMeasureModeUndefined : PilatesMeasureModeExactly;
    childHeightMeasureMode =
        PilatesFloatIsUndefined(childHeight) ? PilatesMeasureModeUndefined : PilatesMeasureModeExactly;

    // If the size of the parent is defined then try to constrain the absolute child to that size
    // as well. This allows text within the absolute child to wrap to the size of its parent.
    // This is the same behavior as many browsers implement.
    if (!isMainAxisRow && PilatesFloatIsUndefined(childWidth) && widthMode != PilatesMeasureModeUndefined &&
        width > 0) {
      childWidth = width;
      childWidthMeasureMode = PilatesMeasureModeAtMost;
    }

    PilatesLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         childWidth,
                         childHeight,
                         false,
                         "abs-measure",
                         config);
    childWidth = child->getLayout().measuredDimensions[PilatesDimensionWidth] +
        child->getMarginForAxis(PilatesFlexDirectionRow, width);
    childHeight = child->getLayout().measuredDimensions[PilatesDimensionHeight] +
        child->getMarginForAxis(PilatesFlexDirectionColumn, width);
  }

  PilatesLayoutNodeInternal(child,
                       childWidth,
                       childHeight,
                       direction,
                       PilatesMeasureModeExactly,
                       PilatesMeasureModeExactly,
                       childWidth,
                       childHeight,
                       true,
                       "abs-layout",
                       config);

  if (child->isTrailingPosDefined(mainAxis) &&
      !child->isLeadingPositionDefined(mainAxis)) {
    child->setLayoutPosition(
        node->getLayout().measuredDimensions[dim[mainAxis]] -
            child->getLayout().measuredDimensions[dim[mainAxis]] -
            node->getTrailingBorder(mainAxis) -
            child->getTrailingMargin(mainAxis, width) -
            child->getTrailingPosition(
                mainAxis, isMainAxisRow ? width : height),
        leading[mainAxis]);
  } else if (
      !child->isLeadingPositionDefined(mainAxis) &&
      node->getStyle().justifyContent == PilatesJustifyCenter) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[mainAxis]] -
         child->getLayout().measuredDimensions[dim[mainAxis]]) /
            2.0f,
        leading[mainAxis]);
  } else if (
      !child->isLeadingPositionDefined(mainAxis) &&
      node->getStyle().justifyContent == PilatesJustifyFlexEnd) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[mainAxis]] -
         child->getLayout().measuredDimensions[dim[mainAxis]]),
        leading[mainAxis]);
  }

  if (child->isTrailingPosDefined(crossAxis) &&
      !child->isLeadingPositionDefined(crossAxis)) {
    child->setLayoutPosition(
        node->getLayout().measuredDimensions[dim[crossAxis]] -
            child->getLayout().measuredDimensions[dim[crossAxis]] -
            node->getTrailingBorder(crossAxis) -
            child->getTrailingMargin(crossAxis, width) -
            child->getTrailingPosition(
                crossAxis, isMainAxisRow ? height : width),
        leading[crossAxis]);

  } else if (
      !child->isLeadingPositionDefined(crossAxis) &&
      PilatesNodeAlignItem(node, child) == PilatesAlignCenter) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[crossAxis]] -
         child->getLayout().measuredDimensions[dim[crossAxis]]) /
            2.0f,
        leading[crossAxis]);
  } else if (
      !child->isLeadingPositionDefined(crossAxis) &&
      ((PilatesNodeAlignItem(node, child) == PilatesAlignFlexEnd) ^
       (node->getStyle().flexWrap == PilatesWrapWrapReverse))) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[crossAxis]] -
         child->getLayout().measuredDimensions[dim[crossAxis]]),
        leading[crossAxis]);
  }
}

static void PilatesNodeWithMeasureFuncSetMeasuredDimensions(const PilatesNodeRef node,
                                                       const float availableWidth,
                                                       const float availableHeight,
                                                       const PilatesMeasureMode widthMeasureMode,
                                                       const PilatesMeasureMode heightMeasureMode,
                                                       const float parentWidth,
                                                       const float parentHeight) {
  PilatesAssertWithNode(
      node,
      node->getMeasure() != nullptr,
      "Expected node to have custom measure function");

  const float paddingAndBorderAxisRow =
      PilatesNodePaddingAndBorderForAxis(node, PilatesFlexDirectionRow, availableWidth);
  const float paddingAndBorderAxisColumn =
      PilatesNodePaddingAndBorderForAxis(node, PilatesFlexDirectionColumn, availableWidth);
  const float marginAxisRow =
      node->getMarginForAxis(PilatesFlexDirectionRow, availableWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(PilatesFlexDirectionColumn, availableWidth);

  // We want to make sure we don't call measure with negative size
  const float innerWidth = PilatesFloatIsUndefined(availableWidth)
      ? availableWidth
      : fmaxf(0, availableWidth - marginAxisRow - paddingAndBorderAxisRow);
  const float innerHeight = PilatesFloatIsUndefined(availableHeight)
      ? availableHeight
      : fmaxf(
            0, availableHeight - marginAxisColumn - paddingAndBorderAxisColumn);

  if (widthMeasureMode == PilatesMeasureModeExactly &&
      heightMeasureMode == PilatesMeasureModeExactly) {
    // Don't bother sizing the text if both dimensions are already defined.
    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            PilatesFlexDirectionRow,
            availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        PilatesDimensionWidth);
    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            PilatesFlexDirectionColumn,
            availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
        PilatesDimensionHeight);
  } else {
    // Measure the text under the current constraints.
    const PilatesSize measuredSize = node->getMeasure()(
        node, innerWidth, widthMeasureMode, innerHeight, heightMeasureMode);

    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            PilatesFlexDirectionRow,
            (widthMeasureMode == PilatesMeasureModeUndefined ||
             widthMeasureMode == PilatesMeasureModeAtMost)
                ? measuredSize.width + paddingAndBorderAxisRow
                : availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        PilatesDimensionWidth);

    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            PilatesFlexDirectionColumn,
            (heightMeasureMode == PilatesMeasureModeUndefined ||
             heightMeasureMode == PilatesMeasureModeAtMost)
                ? measuredSize.height + paddingAndBorderAxisColumn
                : availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
        PilatesDimensionHeight);
  }
}

// For nodes with no children, use the available values if they were provided,
// or the minimum size as indicated by the padding and border sizes.
static void PilatesNodeEmptyContainerSetMeasuredDimensions(const PilatesNodeRef node,
                                                      const float availableWidth,
                                                      const float availableHeight,
                                                      const PilatesMeasureMode widthMeasureMode,
                                                      const PilatesMeasureMode heightMeasureMode,
                                                      const float parentWidth,
                                                      const float parentHeight) {
  const float paddingAndBorderAxisRow =
      PilatesNodePaddingAndBorderForAxis(node, PilatesFlexDirectionRow, parentWidth);
  const float paddingAndBorderAxisColumn =
      PilatesNodePaddingAndBorderForAxis(node, PilatesFlexDirectionColumn, parentWidth);
  const float marginAxisRow =
      node->getMarginForAxis(PilatesFlexDirectionRow, parentWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(PilatesFlexDirectionColumn, parentWidth);

  node->setLayoutMeasuredDimension(
      PilatesNodeBoundAxis(
          node,
          PilatesFlexDirectionRow,
          (widthMeasureMode == PilatesMeasureModeUndefined ||
           widthMeasureMode == PilatesMeasureModeAtMost)
              ? paddingAndBorderAxisRow
              : availableWidth - marginAxisRow,
          parentWidth,
          parentWidth),
      PilatesDimensionWidth);

  node->setLayoutMeasuredDimension(
      PilatesNodeBoundAxis(
          node,
          PilatesFlexDirectionColumn,
          (heightMeasureMode == PilatesMeasureModeUndefined ||
           heightMeasureMode == PilatesMeasureModeAtMost)
              ? paddingAndBorderAxisColumn
              : availableHeight - marginAxisColumn,
          parentHeight,
          parentWidth),
      PilatesDimensionHeight);
}

static bool PilatesNodeFixedSizeSetMeasuredDimensions(const PilatesNodeRef node,
                                                 const float availableWidth,
                                                 const float availableHeight,
                                                 const PilatesMeasureMode widthMeasureMode,
                                                 const PilatesMeasureMode heightMeasureMode,
                                                 const float parentWidth,
                                                 const float parentHeight) {
  if ((widthMeasureMode == PilatesMeasureModeAtMost && availableWidth <= 0.0f) ||
      (heightMeasureMode == PilatesMeasureModeAtMost && availableHeight <= 0.0f) ||
      (widthMeasureMode == PilatesMeasureModeExactly && heightMeasureMode == PilatesMeasureModeExactly)) {
    const float marginAxisColumn =
        node->getMarginForAxis(PilatesFlexDirectionColumn, parentWidth);
    const float marginAxisRow =
        node->getMarginForAxis(PilatesFlexDirectionRow, parentWidth);

    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            PilatesFlexDirectionRow,
            PilatesFloatIsUndefined(availableWidth) ||
                    (widthMeasureMode == PilatesMeasureModeAtMost &&
                     availableWidth < 0.0f)
                ? 0.0f
                : availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        PilatesDimensionWidth);

    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            PilatesFlexDirectionColumn,
            PilatesFloatIsUndefined(availableHeight) ||
                    (heightMeasureMode == PilatesMeasureModeAtMost &&
                     availableHeight < 0.0f)
                ? 0.0f
                : availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
        PilatesDimensionHeight);
    return true;
  }

  return false;
}

static void PilatesZeroOutLayoutRecursivly(const PilatesNodeRef node) {
  memset(&(node->getLayout()), 0, sizeof(PilatesLayout));
  node->setHasNewLayout(true);
  node->cloneChildrenIfNeeded();
  const uint32_t childCount = PilatesNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const PilatesNodeRef child = node->getChild(i);
    PilatesZeroOutLayoutRecursivly(child);
  }
}

static float PilatesNodeCalculateAvailableInnerDim(
    const PilatesNodeRef node,
    PilatesFlexDirection axis,
    float availableDim,
    float parentDim) {
  PilatesFlexDirection direction =
      PilatesFlexDirectionIsRow(axis) ? PilatesFlexDirectionRow : PilatesFlexDirectionColumn;
  PilatesDimension dimension =
      PilatesFlexDirectionIsRow(axis) ? PilatesDimensionWidth : PilatesDimensionHeight;

  const float margin = node->getMarginForAxis(direction, parentDim);
  const float paddingAndBorder =
      PilatesNodePaddingAndBorderForAxis(node, direction, parentDim);

  float availableInnerDim = availableDim - margin - paddingAndBorder;
  // Max dimension overrides predefined dimension value; Min dimension in turn
  // overrides both of the above
  if (!PilatesFloatIsUndefined(availableInnerDim)) {
    // We want to make sure our available height does not violate min and max
    // constraints
    const float minInnerDim =
        PilatesResolveValue(node->getStyle().minDimensions[dimension], parentDim) -
        paddingAndBorder;
    const float maxInnerDim =
        PilatesResolveValue(node->getStyle().maxDimensions[dimension], parentDim) -
        paddingAndBorder;
    availableInnerDim =
        fmaxf(fminf(availableInnerDim, maxInnerDim), minInnerDim);
  }

  return availableInnerDim;
}

static void PilatesNodeComputeFlexBasisForChildren(
    const PilatesNodeRef node,
    const float availableInnerWidth,
    const float availableInnerHeight,
    PilatesMeasureMode widthMeasureMode,
    PilatesMeasureMode heightMeasureMode,
    PilatesDirection direction,
    PilatesFlexDirection mainAxis,
    const PilatesConfigRef config,
    bool performLayout,
    float& totalOuterFlexBasis) {
  PilatesNodeRef singleFlexChild = nullptr;
  PilatesVector children = node->getChildren();
  PilatesMeasureMode measureModeMainDim =
      PilatesFlexDirectionIsRow(mainAxis) ? widthMeasureMode : heightMeasureMode;
  // If there is only one child with flexGrow + flexShrink it means we can set
  // the computedFlexBasis to 0 instead of measuring and shrinking / flexing the
  // child to exactly match the remaining space
  if (measureModeMainDim == PilatesMeasureModeExactly) {
    for (auto child : children) {
      if (singleFlexChild != nullptr) {
        if (child->isNodeFlexible()) {
          // There is already a flexible child, abort
          singleFlexChild = nullptr;
          break;
        }
      } else if (
          child->resolveFlexGrow() > 0.0f &&
          child->resolveFlexShrink() > 0.0f) {
        singleFlexChild = child;
      }
    }
  }

  for (auto child : children) {
    child->resolveDimension();
    if (child->getStyle().display == PilatesDisplayNone) {
      PilatesZeroOutLayoutRecursivly(child);
      child->setHasNewLayout(true);
      child->setDirty(false);
      continue;
    }
    if (performLayout) {
      // Set the initial position (relative to the parent).
      const PilatesDirection childDirection = child->resolveDirection(direction);
      const float mainDim = PilatesFlexDirectionIsRow(mainAxis)
          ? availableInnerWidth
          : availableInnerHeight;
      const float crossDim = PilatesFlexDirectionIsRow(mainAxis)
          ? availableInnerHeight
          : availableInnerWidth;
      child->setPosition(
          childDirection, mainDim, crossDim, availableInnerWidth);
    }

    if (child->getStyle().positionType == PilatesPositionTypeAbsolute) {
      continue;
    }
    if (child == singleFlexChild) {
      child->setLayoutComputedFlexBasisGeneration(gCurrentGenerationCount);
      child->setLayoutComputedFlexBasis(0);
    } else {
      PilatesNodeComputeFlexBasisForChild(
          node,
          child,
          availableInnerWidth,
          widthMeasureMode,
          availableInnerHeight,
          availableInnerWidth,
          availableInnerHeight,
          heightMeasureMode,
          direction,
          config);
    }

    totalOuterFlexBasis += child->getLayout().computedFlexBasis +
        child->getMarginForAxis(mainAxis, availableInnerWidth);
  }
}

// This function assumes that all the children of node have their
// computedFlexBasis properly computed(To do this use
// PilatesNodeComputeFlexBasisForChildren function).
// This function calculates PilatesCollectFlexItemsRowMeasurement
static PilatesCollectFlexItemsRowValues PilatesCalculateCollectFlexItemsRowValues(
    const PilatesNodeRef& node,
    const PilatesDirection parentDirection,
    const float mainAxisParentSize,
    const float availableInnerWidth,
    const float availableInnerMainDim,
    const uint32_t startOfLineIndex,
    const uint32_t lineCount) {
  PilatesCollectFlexItemsRowValues flexAlgoRowMeasurement = {};
  flexAlgoRowMeasurement.relativeChildren.reserve(node->getChildren().size());

  float sizeConsumedOnCurrentLineIncludingMinConstraint = 0;
  const PilatesFlexDirection mainAxis = PilatesResolveFlexDirection(
      node->getStyle().flexDirection, node->resolveDirection(parentDirection));
  const bool isNodeFlexWrap = node->getStyle().flexWrap != PilatesWrapNoWrap;

  // Add items to the current line until it's full or we run out of items.
  uint32_t endOfLineIndex = startOfLineIndex;
  for (; endOfLineIndex < node->getChildrenCount(); endOfLineIndex++) {
    const PilatesNodeRef child = node->getChild(endOfLineIndex);
    if (child->getStyle().display == PilatesDisplayNone ||
        child->getStyle().positionType == PilatesPositionTypeAbsolute) {
      continue;
    }
    child->setLineIndex(lineCount);
    const float childMarginMainAxis =
        child->getMarginForAxis(mainAxis, availableInnerWidth);
    const float flexBasisWithMinAndMaxConstraints =
        PilatesNodeBoundAxisWithinMinAndMax(
            child,
            mainAxis,
            child->getLayout().computedFlexBasis,
            mainAxisParentSize);

    // If this is a multi-line flow and this item pushes us over the
    // available size, we've
    // hit the end of the current line. Break out of the loop and lay out
    // the current line.
    if (sizeConsumedOnCurrentLineIncludingMinConstraint +
                flexBasisWithMinAndMaxConstraints + childMarginMainAxis >
            availableInnerMainDim &&
        isNodeFlexWrap && flexAlgoRowMeasurement.itemsOnLine > 0) {
      break;
    }

    sizeConsumedOnCurrentLineIncludingMinConstraint +=
        flexBasisWithMinAndMaxConstraints + childMarginMainAxis;
    flexAlgoRowMeasurement.sizeConsumedOnCurrentLine +=
        flexBasisWithMinAndMaxConstraints + childMarginMainAxis;
    flexAlgoRowMeasurement.itemsOnLine++;

    if (child->isNodeFlexible()) {
      flexAlgoRowMeasurement.totalFlexGrowFactors += child->resolveFlexGrow();

      // Unlike the grow factor, the shrink factor is scaled relative to the
      // child dimension.
      flexAlgoRowMeasurement.totalFlexShrinkScaledFactors +=
          -child->resolveFlexShrink() * child->getLayout().computedFlexBasis;
    }

    flexAlgoRowMeasurement.relativeChildren.push_back(child);
  }

  // The total flex factor needs to be floored to 1.
  if (flexAlgoRowMeasurement.totalFlexGrowFactors > 0 &&
      flexAlgoRowMeasurement.totalFlexGrowFactors < 1) {
    flexAlgoRowMeasurement.totalFlexGrowFactors = 1;
  }

  // The total flex shrink factor needs to be floored to 1.
  if (flexAlgoRowMeasurement.totalFlexShrinkScaledFactors > 0 &&
      flexAlgoRowMeasurement.totalFlexShrinkScaledFactors < 1) {
    flexAlgoRowMeasurement.totalFlexShrinkScaledFactors = 1;
  }
  flexAlgoRowMeasurement.endOfLineIndex = endOfLineIndex;
  return flexAlgoRowMeasurement;
}

// It distributes the free space to the flexible items and ensures that the size
// of the flex items abide the min and max constraints. At the end of this
// function the child nodes would have proper size. Prior using this function
// please ensure that PilatesDistributeFreeSpaceFirstPass is called.
static float PilatesDistributeFreeSpaceSecondPass(
    PilatesCollectFlexItemsRowValues& collectedFlexItemsValues,
    const PilatesNodeRef node,
    const PilatesFlexDirection mainAxis,
    const PilatesFlexDirection crossAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerCrossDim,
    const float availableInnerWidth,
    const float availableInnerHeight,
    const bool flexBasisOverflows,
    const PilatesMeasureMode measureModeCrossDim,
    const bool performLayout,
    const PilatesConfigRef config) {
  float childFlexBasis = 0;
  float flexShrinkScaledFactor = 0;
  float flexGrowFactor = 0;
  float deltaFreeSpace = 0;
  const bool isMainAxisRow = PilatesFlexDirectionIsRow(mainAxis);
  const bool isNodeFlexWrap = node->getStyle().flexWrap != PilatesWrapNoWrap;

  for (auto currentRelativeChild : collectedFlexItemsValues.relativeChildren) {
    childFlexBasis = PilatesNodeBoundAxisWithinMinAndMax(
        currentRelativeChild,
        mainAxis,
        currentRelativeChild->getLayout().computedFlexBasis,
        mainAxisParentSize);
    float updatedMainSize = childFlexBasis;

    if (collectedFlexItemsValues.remainingFreeSpace < 0) {
      flexShrinkScaledFactor =
          -currentRelativeChild->resolveFlexShrink() * childFlexBasis;
      // Is this child able to shrink?
      if (flexShrinkScaledFactor != 0) {
        float childSize;

        if (collectedFlexItemsValues.totalFlexShrinkScaledFactors == 0) {
          childSize = childFlexBasis + flexShrinkScaledFactor;
        } else {
          childSize = childFlexBasis +
              (collectedFlexItemsValues.remainingFreeSpace /
               collectedFlexItemsValues.totalFlexShrinkScaledFactors) *
                  flexShrinkScaledFactor;
        }

        updatedMainSize = PilatesNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            childSize,
            availableInnerMainDim,
            availableInnerWidth);
      }
    } else if (collectedFlexItemsValues.remainingFreeSpace > 0) {
      flexGrowFactor = currentRelativeChild->resolveFlexGrow();

      // Is this child able to grow?
      if (flexGrowFactor != 0) {
        updatedMainSize = PilatesNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            childFlexBasis +
                collectedFlexItemsValues.remainingFreeSpace /
                    collectedFlexItemsValues.totalFlexGrowFactors *
                    flexGrowFactor,
            availableInnerMainDim,
            availableInnerWidth);
      }
    }

    deltaFreeSpace += updatedMainSize - childFlexBasis;

    const float marginMain =
        currentRelativeChild->getMarginForAxis(mainAxis, availableInnerWidth);
    const float marginCross =
        currentRelativeChild->getMarginForAxis(crossAxis, availableInnerWidth);

    float childCrossSize;
    float childMainSize = updatedMainSize + marginMain;
    PilatesMeasureMode childCrossMeasureMode;
    PilatesMeasureMode childMainMeasureMode = PilatesMeasureModeExactly;

    if (!PilatesFloatIsUndefined(currentRelativeChild->getStyle().aspectRatio)) {
      childCrossSize = isMainAxisRow ? (childMainSize - marginMain) /
              currentRelativeChild->getStyle().aspectRatio
                                     : (childMainSize - marginMain) *
              currentRelativeChild->getStyle().aspectRatio;
      childCrossMeasureMode = PilatesMeasureModeExactly;

      childCrossSize += marginCross;
    } else if (
        !PilatesFloatIsUndefined(availableInnerCrossDim) &&
        !PilatesNodeIsStyleDimDefined(
            currentRelativeChild, crossAxis, availableInnerCrossDim) &&
        measureModeCrossDim == PilatesMeasureModeExactly &&
        !(isNodeFlexWrap && flexBasisOverflows) &&
        PilatesNodeAlignItem(node, currentRelativeChild) == PilatesAlignStretch &&
        currentRelativeChild->marginLeadingValue(crossAxis).unit !=
            PilatesUnitAuto &&
        currentRelativeChild->marginTrailingValue(crossAxis).unit !=
            PilatesUnitAuto) {
      childCrossSize = availableInnerCrossDim;
      childCrossMeasureMode = PilatesMeasureModeExactly;
    } else if (!PilatesNodeIsStyleDimDefined(
                   currentRelativeChild, crossAxis, availableInnerCrossDim)) {
      childCrossSize = availableInnerCrossDim;
      childCrossMeasureMode = PilatesFloatIsUndefined(childCrossSize)
          ? PilatesMeasureModeUndefined
          : PilatesMeasureModeAtMost;
    } else {
      childCrossSize =
          PilatesResolveValue(
              currentRelativeChild->getResolvedDimension(dim[crossAxis]),
              availableInnerCrossDim) +
          marginCross;
      const bool isLoosePercentageMeasurement =
          currentRelativeChild->getResolvedDimension(dim[crossAxis]).unit ==
              PilatesUnitPercent &&
          measureModeCrossDim != PilatesMeasureModeExactly;
      childCrossMeasureMode =
          PilatesFloatIsUndefined(childCrossSize) || isLoosePercentageMeasurement
          ? PilatesMeasureModeUndefined
          : PilatesMeasureModeExactly;
    }

    PilatesConstrainMaxSizeForMode(
        currentRelativeChild,
        mainAxis,
        availableInnerMainDim,
        availableInnerWidth,
        &childMainMeasureMode,
        &childMainSize);
    PilatesConstrainMaxSizeForMode(
        currentRelativeChild,
        crossAxis,
        availableInnerCrossDim,
        availableInnerWidth,
        &childCrossMeasureMode,
        &childCrossSize);

    const bool requiresStretchLayout =
        !PilatesNodeIsStyleDimDefined(
            currentRelativeChild, crossAxis, availableInnerCrossDim) &&
        PilatesNodeAlignItem(node, currentRelativeChild) == PilatesAlignStretch &&
        currentRelativeChild->marginLeadingValue(crossAxis).unit !=
            PilatesUnitAuto &&
        currentRelativeChild->marginTrailingValue(crossAxis).unit != PilatesUnitAuto;

    const float childWidth = isMainAxisRow ? childMainSize : childCrossSize;
    const float childHeight = !isMainAxisRow ? childMainSize : childCrossSize;

    const PilatesMeasureMode childWidthMeasureMode =
        isMainAxisRow ? childMainMeasureMode : childCrossMeasureMode;
    const PilatesMeasureMode childHeightMeasureMode =
        !isMainAxisRow ? childMainMeasureMode : childCrossMeasureMode;

    // Recursively call the layout algorithm for this child with the updated
    // main size.
    PilatesLayoutNodeInternal(
        currentRelativeChild,
        childWidth,
        childHeight,
        node->getLayout().direction,
        childWidthMeasureMode,
        childHeightMeasureMode,
        availableInnerWidth,
        availableInnerHeight,
        performLayout && !requiresStretchLayout,
        "flex",
        config);
    node->setLayoutHadOverflow(
        node->getLayout().hadOverflow |
        currentRelativeChild->getLayout().hadOverflow);
  }
  return deltaFreeSpace;
}

// It distributes the free space to the flexible items.For those flexible items
// whose min and max constraints are triggered, those flex item's clamped size
// is removed from the remaingfreespace.
static void PilatesDistributeFreeSpaceFirstPass(
    PilatesCollectFlexItemsRowValues& collectedFlexItemsValues,
    const PilatesFlexDirection mainAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerWidth) {
  float flexShrinkScaledFactor = 0;
  float flexGrowFactor = 0;
  float baseMainSize = 0;
  float boundMainSize = 0;
  float deltaFreeSpace = 0;

  for (auto currentRelativeChild : collectedFlexItemsValues.relativeChildren) {
    float childFlexBasis = PilatesNodeBoundAxisWithinMinAndMax(
        currentRelativeChild,
        mainAxis,
        currentRelativeChild->getLayout().computedFlexBasis,
        mainAxisParentSize);

    if (collectedFlexItemsValues.remainingFreeSpace < 0) {
      flexShrinkScaledFactor =
          -currentRelativeChild->resolveFlexShrink() * childFlexBasis;

      // Is this child able to shrink?
      if (flexShrinkScaledFactor != 0) {
        baseMainSize = childFlexBasis +
            collectedFlexItemsValues.remainingFreeSpace /
                collectedFlexItemsValues.totalFlexShrinkScaledFactors *
                flexShrinkScaledFactor;
        boundMainSize = PilatesNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            baseMainSize,
            availableInnerMainDim,
            availableInnerWidth);
        if (baseMainSize != boundMainSize) {
          // By excluding this item's size and flex factor from remaining,
          // this item's
          // min/max constraints should also trigger in the second pass
          // resulting in the
          // item's size calculation being identical in the first and second
          // passes.
          deltaFreeSpace += boundMainSize - childFlexBasis;
          collectedFlexItemsValues.totalFlexShrinkScaledFactors -=
              flexShrinkScaledFactor;
        }
      }
    } else if (collectedFlexItemsValues.remainingFreeSpace > 0) {
      flexGrowFactor = currentRelativeChild->resolveFlexGrow();

      // Is this child able to grow?
      if (flexGrowFactor != 0) {
        baseMainSize = childFlexBasis +
            collectedFlexItemsValues.remainingFreeSpace /
                collectedFlexItemsValues.totalFlexGrowFactors * flexGrowFactor;
        boundMainSize = PilatesNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            baseMainSize,
            availableInnerMainDim,
            availableInnerWidth);

        if (baseMainSize != boundMainSize) {
          // By excluding this item's size and flex factor from remaining,
          // this item's
          // min/max constraints should also trigger in the second pass
          // resulting in the
          // item's size calculation being identical in the first and second
          // passes.
          deltaFreeSpace += boundMainSize - childFlexBasis;
          collectedFlexItemsValues.totalFlexGrowFactors -= flexGrowFactor;
        }
      }
    }
  }
  collectedFlexItemsValues.remainingFreeSpace -= deltaFreeSpace;
}

// Do two passes over the flex items to figure out how to distribute the
// remaining space.
// The first pass finds the items whose min/max constraints trigger,
// freezes them at those
// sizes, and excludes those sizes from the remaining space. The second
// pass sets the size
// of each flexible item. It distributes the remaining space amongst the
// items whose min/max
// constraints didn't trigger in pass 1. For the other items, it sets
// their sizes by forcing
// their min/max constraints to trigger again.
//
// This two pass approach for resolving min/max constraints deviates from
// the spec. The
// spec (https://www.w3.org/TR/Pilates-flexbox-1/#resolve-flexible-lengths)
// describes a process
// that needs to be repeated a variable number of times. The algorithm
// implemented here
// won't handle all cases but it was simpler to implement and it mitigates
// performance
// concerns because we know exactly how many passes it'll do.
//
// At the end of this function the child nodes would have the proper size
// assigned to them.
//
static void PilatesResolveFlexibleLength(
    const PilatesNodeRef node,
    PilatesCollectFlexItemsRowValues& collectedFlexItemsValues,
    const PilatesFlexDirection mainAxis,
    const PilatesFlexDirection crossAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerCrossDim,
    const float availableInnerWidth,
    const float availableInnerHeight,
    const bool flexBasisOverflows,
    const PilatesMeasureMode measureModeCrossDim,
    const bool performLayout,
    const PilatesConfigRef config) {
  const float originalFreeSpace = collectedFlexItemsValues.remainingFreeSpace;
  // First pass: detect the flex items whose min/max constraints trigger
  PilatesDistributeFreeSpaceFirstPass(
      collectedFlexItemsValues,
      mainAxis,
      mainAxisParentSize,
      availableInnerMainDim,
      availableInnerWidth);

  // Second pass: resolve the sizes of the flexible items
  const float distributedFreeSpace = PilatesDistributeFreeSpaceSecondPass(
      collectedFlexItemsValues,
      node,
      mainAxis,
      crossAxis,
      mainAxisParentSize,
      availableInnerMainDim,
      availableInnerCrossDim,
      availableInnerWidth,
      availableInnerHeight,
      flexBasisOverflows,
      measureModeCrossDim,
      performLayout,
      config);

  collectedFlexItemsValues.remainingFreeSpace =
      originalFreeSpace - distributedFreeSpace;
}

static void PilatesJustifyMainAxis(
    const PilatesNodeRef node,
    PilatesCollectFlexItemsRowValues& collectedFlexItemsValues,
    const uint32_t& startOfLineIndex,
    const PilatesFlexDirection& mainAxis,
    const PilatesFlexDirection& crossAxis,
    const PilatesMeasureMode& measureModeMainDim,
    const PilatesMeasureMode& measureModeCrossDim,
    const float& mainAxisParentSize,
    const float& parentWidth,
    const float& availableInnerMainDim,
    const float& availableInnerCrossDim,
    const float& availableInnerWidth,
    const bool& performLayout) {
  const PilatesStyle style = node->getStyle();

  // If we are using "at most" rules in the main axis. Calculate the remaining
  // space when constraint by the min size defined for the main axis.
  if (measureModeMainDim == PilatesMeasureModeAtMost &&
      collectedFlexItemsValues.remainingFreeSpace > 0) {
    if (style.minDimensions[dim[mainAxis]].unit != PilatesUnitUndefined &&
        PilatesResolveValue(
            style.minDimensions[dim[mainAxis]], mainAxisParentSize) >= 0) {
      collectedFlexItemsValues.remainingFreeSpace = fmaxf(
          0,
          PilatesResolveValue(
              style.minDimensions[dim[mainAxis]], mainAxisParentSize) -
              (availableInnerMainDim -
               collectedFlexItemsValues.remainingFreeSpace));
    } else {
      collectedFlexItemsValues.remainingFreeSpace = 0;
    }
  }

  int numberOfAutoMarginsOnCurrentLine = 0;
  for (uint32_t i = startOfLineIndex;
       i < collectedFlexItemsValues.endOfLineIndex;
       i++) {
    const PilatesNodeRef child = node->getChild(i);
    if (child->getStyle().positionType == PilatesPositionTypeRelative) {
      if (child->marginLeadingValue(mainAxis).unit == PilatesUnitAuto) {
        numberOfAutoMarginsOnCurrentLine++;
      }
      if (child->marginTrailingValue(mainAxis).unit == PilatesUnitAuto) {
        numberOfAutoMarginsOnCurrentLine++;
      }
    }
  }

  // In order to position the elements in the main axis, we have two
  // controls. The space between the beginning and the first element
  // and the space between each two elements.
  float leadingMainDim = 0;
  float betweenMainDim = 0;
  const PilatesJustify justifyContent = node->getStyle().justifyContent;

  if (numberOfAutoMarginsOnCurrentLine == 0) {
    switch (justifyContent) {
      case PilatesJustifyCenter:
        leadingMainDim = collectedFlexItemsValues.remainingFreeSpace / 2;
        break;
      case PilatesJustifyFlexEnd:
        leadingMainDim = collectedFlexItemsValues.remainingFreeSpace;
        break;
      case PilatesJustifySpaceBetween:
        if (collectedFlexItemsValues.itemsOnLine > 1) {
          betweenMainDim =
              fmaxf(collectedFlexItemsValues.remainingFreeSpace, 0) /
              (collectedFlexItemsValues.itemsOnLine - 1);
        } else {
          betweenMainDim = 0;
        }
        break;
      case PilatesJustifySpaceEvenly:
        // Space is distributed evenly across all elements
        betweenMainDim = collectedFlexItemsValues.remainingFreeSpace /
            (collectedFlexItemsValues.itemsOnLine + 1);
        leadingMainDim = betweenMainDim;
        break;
      case PilatesJustifySpaceAround:
        // Space on the edges is half of the space between elements
        betweenMainDim = collectedFlexItemsValues.remainingFreeSpace /
            collectedFlexItemsValues.itemsOnLine;
        leadingMainDim = betweenMainDim / 2;
        break;
      case PilatesJustifyFlexStart:
        break;
    }
  }

  const float leadingPaddingAndBorderMain =
      node->getLeadingPaddingAndBorder(mainAxis, parentWidth);
  collectedFlexItemsValues.mainDim =
      leadingPaddingAndBorderMain + leadingMainDim;
  collectedFlexItemsValues.crossDim = 0;

  for (uint32_t i = startOfLineIndex;
       i < collectedFlexItemsValues.endOfLineIndex;
       i++) {
    const PilatesNodeRef child = node->getChild(i);
    const PilatesStyle childStyle = child->getStyle();
    const PilatesLayout childLayout = child->getLayout();
    if (childStyle.display == PilatesDisplayNone) {
      continue;
    }
    if (childStyle.positionType == PilatesPositionTypeAbsolute &&
        child->isLeadingPositionDefined(mainAxis)) {
      if (performLayout) {
        // In case the child is position absolute and has left/top being
        // defined, we override the position to whatever the user said
        // (and margin/border).
        child->setLayoutPosition(
            child->getLeadingPosition(mainAxis, availableInnerMainDim) +
                node->getLeadingBorder(mainAxis) +
                child->getLeadingMargin(mainAxis, availableInnerWidth),
            pos[mainAxis]);
      }
    } else {
      // Now that we placed the element, we need to update the variables.
      // We need to do that only for relative elements. Absolute elements
      // do not take part in that phase.
      if (childStyle.positionType == PilatesPositionTypeRelative) {
        if (child->marginLeadingValue(mainAxis).unit == PilatesUnitAuto) {
          collectedFlexItemsValues.mainDim +=
              collectedFlexItemsValues.remainingFreeSpace /
              numberOfAutoMarginsOnCurrentLine;
        }

        if (performLayout) {
          child->setLayoutPosition(
              childLayout.position[pos[mainAxis]] +
                  collectedFlexItemsValues.mainDim,
              pos[mainAxis]);
        }

        if (child->marginTrailingValue(mainAxis).unit == PilatesUnitAuto) {
          collectedFlexItemsValues.mainDim +=
              collectedFlexItemsValues.remainingFreeSpace /
              numberOfAutoMarginsOnCurrentLine;
        }
        bool canSkipFlex =
            !performLayout && measureModeCrossDim == PilatesMeasureModeExactly;
        if (canSkipFlex) {
          // If we skipped the flex step, then we can't rely on the
          // measuredDims because
          // they weren't computed. This means we can't call
          // PilatesNodeDimWithMargin.
          collectedFlexItemsValues.mainDim += betweenMainDim +
              child->getMarginForAxis(mainAxis, availableInnerWidth) +
              childLayout.computedFlexBasis;
          collectedFlexItemsValues.crossDim = availableInnerCrossDim;
        } else {
          // The main dimension is the sum of all the elements dimension plus
          // the spacing.
          collectedFlexItemsValues.mainDim += betweenMainDim +
              PilatesNodeDimWithMargin(child, mainAxis, availableInnerWidth);

          // The cross dimension is the max of the elements dimension since
          // there can only be one element in that cross dimension.
          collectedFlexItemsValues.crossDim = fmaxf(
              collectedFlexItemsValues.crossDim,
              PilatesNodeDimWithMargin(child, crossAxis, availableInnerWidth));
        }
      } else if (performLayout) {
        child->setLayoutPosition(
            childLayout.position[pos[mainAxis]] +
                node->getLeadingBorder(mainAxis) + leadingMainDim,
            pos[mainAxis]);
      }
    }
  }
  collectedFlexItemsValues.mainDim +=
      node->getTrailingPaddingAndBorder(mainAxis, parentWidth);
}

//
// This is the main routine that implements a subset of the flexbox layout
// algorithm
// described in the W3C Pilates documentation: https://www.w3.org/TR/Pilates3-flexbox/.
//
// Limitations of this algorithm, compared to the full standard:
//  * Display property is always assumed to be 'flex' except for Text nodes,
//  which
//    are assumed to be 'inline-flex'.
//  * The 'zIndex' property (or any form of z ordering) is not supported. Nodes
//  are
//    stacked in document order.
//  * The 'order' property is not supported. The order of flex items is always
//  defined
//    by document order.
//  * The 'visibility' property is always assumed to be 'visible'. Values of
//  'collapse'
//    and 'hidden' are not supported.
//  * There is no support for forced breaks.
//  * It does not support vertical inline directions (top-to-bottom or
//  bottom-to-top text).
//
// Deviations from standard:
//  * Section 4.5 of the spec indicates that all flex items have a default
//  minimum
//    main size. For text blocks, for example, this is the width of the widest
//    word.
//    Calculating the minimum width is expensive, so we forego it and assume a
//    default
//    minimum main size of 0.
//  * Min/Max sizes in the main axis are not honored when resolving flexible
//  lengths.
//  * The spec indicates that the default value for 'flexDirection' is 'row',
//  but
//    the algorithm below assumes a default of 'column'.
//
// Input parameters:
//    - node: current node to be sized and layed out
//    - availableWidth & availableHeight: available size to be used for sizing
//    the node
//      or PilatesUndefined if the size is not available; interpretation depends on
//      layout
//      flags
//    - parentDirection: the inline (text) direction within the parent
//    (left-to-right or
//      right-to-left)
//    - widthMeasureMode: indicates the sizing rules for the width (see below
//    for explanation)
//    - heightMeasureMode: indicates the sizing rules for the height (see below
//    for explanation)
//    - performLayout: specifies whether the caller is interested in just the
//    dimensions
//      of the node or it requires the entire node and its subtree to be layed
//      out
//      (with final positions)
//
// Details:
//    This routine is called recursively to lay out subtrees of flexbox
//    elements. It uses the
//    information in node.style, which is treated as a read-only input. It is
//    responsible for
//    setting the layout.direction and layout.measuredDimensions fields for the
//    input node as well
//    as the layout.position and layout.lineIndex fields for its child nodes.
//    The
//    layout.measuredDimensions field includes any border or padding for the
//    node but does
//    not include margins.
//
//    The spec describes four different layout modes: "fill available", "max
//    content", "min
//    content",
//    and "fit content". Of these, we don't use "min content" because we don't
//    support default
//    minimum main sizes (see above for details). Each of our measure modes maps
//    to a layout mode
//    from the spec (https://www.w3.org/TR/Pilates3-sizing/#terms):
//      - PilatesMeasureModeUndefined: max content
//      - PilatesMeasureModeExactly: fill available
//      - PilatesMeasureModeAtMost: fit content
//
//    When calling PilatesNodelayoutImpl and PilatesLayoutNodeInternal, if the caller passes
//    an available size of
//    undefined then it must also pass a measure mode of PilatesMeasureModeUndefined
//    in that dimension.
//
static void PilatesNodelayoutImpl(const PilatesNodeRef node,
                             const float availableWidth,
                             const float availableHeight,
                             const PilatesDirection parentDirection,
                             const PilatesMeasureMode widthMeasureMode,
                             const PilatesMeasureMode heightMeasureMode,
                             const float parentWidth,
                             const float parentHeight,
                             const bool performLayout,
                             const PilatesConfigRef config) {
  PilatesAssertWithNode(node,
                   PilatesFloatIsUndefined(availableWidth) ? widthMeasureMode == PilatesMeasureModeUndefined
                                                      : true,
                   "availableWidth is indefinite so widthMeasureMode must be "
                   "PilatesMeasureModeUndefined");
  PilatesAssertWithNode(node,
                   PilatesFloatIsUndefined(availableHeight) ? heightMeasureMode == PilatesMeasureModeUndefined
                                                       : true,
                   "availableHeight is indefinite so heightMeasureMode must be "
                   "PilatesMeasureModeUndefined");

  // Set the resolved resolution in the node's layout.
  const PilatesDirection direction = node->resolveDirection(parentDirection);
  node->setLayoutDirection(direction);

  const PilatesFlexDirection flexRowDirection = PilatesResolveFlexDirection(PilatesFlexDirectionRow, direction);
  const PilatesFlexDirection flexColumnDirection =
      PilatesResolveFlexDirection(PilatesFlexDirectionColumn, direction);

  node->setLayoutMargin(
      node->getLeadingMargin(flexRowDirection, parentWidth), PilatesEdgeStart);
  node->setLayoutMargin(
      node->getTrailingMargin(flexRowDirection, parentWidth), PilatesEdgeEnd);
  node->setLayoutMargin(
      node->getLeadingMargin(flexColumnDirection, parentWidth), PilatesEdgeTop);
  node->setLayoutMargin(
      node->getTrailingMargin(flexColumnDirection, parentWidth), PilatesEdgeBottom);

  node->setLayoutBorder(node->getLeadingBorder(flexRowDirection), PilatesEdgeStart);
  node->setLayoutBorder(node->getTrailingBorder(flexRowDirection), PilatesEdgeEnd);
  node->setLayoutBorder(node->getLeadingBorder(flexColumnDirection), PilatesEdgeTop);
  node->setLayoutBorder(
      node->getTrailingBorder(flexColumnDirection), PilatesEdgeBottom);

  node->setLayoutPadding(
      node->getLeadingPadding(flexRowDirection, parentWidth), PilatesEdgeStart);
  node->setLayoutPadding(
      node->getTrailingPadding(flexRowDirection, parentWidth), PilatesEdgeEnd);
  node->setLayoutPadding(
      node->getLeadingPadding(flexColumnDirection, parentWidth), PilatesEdgeTop);
  node->setLayoutPadding(
      node->getTrailingPadding(flexColumnDirection, parentWidth), PilatesEdgeBottom);

  if (node->getMeasure() != nullptr) {
    PilatesNodeWithMeasureFuncSetMeasuredDimensions(node,
                                               availableWidth,
                                               availableHeight,
                                               widthMeasureMode,
                                               heightMeasureMode,
                                               parentWidth,
                                               parentHeight);
    return;
  }

  const uint32_t childCount = PilatesNodeGetChildCount(node);
  if (childCount == 0) {
    PilatesNodeEmptyContainerSetMeasuredDimensions(node,
                                              availableWidth,
                                              availableHeight,
                                              widthMeasureMode,
                                              heightMeasureMode,
                                              parentWidth,
                                              parentHeight);
    return;
  }

  // If we're not being asked to perform a full layout we can skip the algorithm if we already know
  // the size
  if (!performLayout && PilatesNodeFixedSizeSetMeasuredDimensions(node,
                                                             availableWidth,
                                                             availableHeight,
                                                             widthMeasureMode,
                                                             heightMeasureMode,
                                                             parentWidth,
                                                             parentHeight)) {
    return;
  }

  // At this point we know we're going to perform work. Ensure that each child has a mutable copy.
  node->cloneChildrenIfNeeded();
  // Reset layout flags, as they could have changed.
  node->setLayoutHadOverflow(false);

  // STEP 1: CALCULATE VALUES FOR REMAINDER OF ALGORITHM
  const PilatesFlexDirection mainAxis =
      PilatesResolveFlexDirection(node->getStyle().flexDirection, direction);
  const PilatesFlexDirection crossAxis = PilatesFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = PilatesFlexDirectionIsRow(mainAxis);
  const bool isNodeFlexWrap = node->getStyle().flexWrap != PilatesWrapNoWrap;

  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;
  const float crossAxisParentSize = isMainAxisRow ? parentHeight : parentWidth;

  const float leadingPaddingAndBorderCross =
      node->getLeadingPaddingAndBorder(crossAxis, parentWidth);
  const float paddingAndBorderAxisMain = PilatesNodePaddingAndBorderForAxis(node, mainAxis, parentWidth);
  const float paddingAndBorderAxisCross =
      PilatesNodePaddingAndBorderForAxis(node, crossAxis, parentWidth);

  PilatesMeasureMode measureModeMainDim = isMainAxisRow ? widthMeasureMode : heightMeasureMode;
  PilatesMeasureMode measureModeCrossDim = isMainAxisRow ? heightMeasureMode : widthMeasureMode;

  const float paddingAndBorderAxisRow =
      isMainAxisRow ? paddingAndBorderAxisMain : paddingAndBorderAxisCross;
  const float paddingAndBorderAxisColumn =
      isMainAxisRow ? paddingAndBorderAxisCross : paddingAndBorderAxisMain;

  const float marginAxisRow =
      node->getMarginForAxis(PilatesFlexDirectionRow, parentWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(PilatesFlexDirectionColumn, parentWidth);

  const float minInnerWidth =
      PilatesResolveValue(
          node->getStyle().minDimensions[PilatesDimensionWidth], parentWidth) -
      paddingAndBorderAxisRow;
  const float maxInnerWidth =
      PilatesResolveValue(
          node->getStyle().maxDimensions[PilatesDimensionWidth], parentWidth) -
      paddingAndBorderAxisRow;
  const float minInnerHeight =
      PilatesResolveValue(
          node->getStyle().minDimensions[PilatesDimensionHeight], parentHeight) -
      paddingAndBorderAxisColumn;
  const float maxInnerHeight =
      PilatesResolveValue(
          node->getStyle().maxDimensions[PilatesDimensionHeight], parentHeight) -
      paddingAndBorderAxisColumn;

  const float minInnerMainDim = isMainAxisRow ? minInnerWidth : minInnerHeight;
  const float maxInnerMainDim = isMainAxisRow ? maxInnerWidth : maxInnerHeight;

  // STEP 2: DETERMINE AVAILABLE SIZE IN MAIN AND CROSS DIRECTIONS

  float availableInnerWidth = PilatesNodeCalculateAvailableInnerDim(
      node, PilatesFlexDirectionRow, availableWidth, parentWidth);
  float availableInnerHeight = PilatesNodeCalculateAvailableInnerDim(
      node, PilatesFlexDirectionColumn, availableHeight, parentHeight);

  float availableInnerMainDim =
      isMainAxisRow ? availableInnerWidth : availableInnerHeight;
  const float availableInnerCrossDim =
      isMainAxisRow ? availableInnerHeight : availableInnerWidth;

  float totalOuterFlexBasis = 0;

  // STEP 3: DETERMINE FLEX BASIS FOR EACH ITEM

  PilatesNodeComputeFlexBasisForChildren(
      node,
      availableInnerWidth,
      availableInnerHeight,
      widthMeasureMode,
      heightMeasureMode,
      direction,
      mainAxis,
      config,
      performLayout,
      totalOuterFlexBasis);

  const bool flexBasisOverflows = measureModeMainDim == PilatesMeasureModeUndefined
      ? false
      : totalOuterFlexBasis > availableInnerMainDim;
  if (isNodeFlexWrap && flexBasisOverflows &&
      measureModeMainDim == PilatesMeasureModeAtMost) {
    measureModeMainDim = PilatesMeasureModeExactly;
  }
  // STEP 4: COLLECT FLEX ITEMS INTO FLEX LINES

  // Indexes of children that represent the first and last items in the line.
  uint32_t startOfLineIndex = 0;
  uint32_t endOfLineIndex = 0;

  // Number of lines.
  uint32_t lineCount = 0;

  // Accumulated cross dimensions of all lines so far.
  float totalLineCrossDim = 0;

  // Max main dimension of all the lines.
  float maxLineMainDim = 0;
  PilatesCollectFlexItemsRowValues collectedFlexItemsValues;
  for (; endOfLineIndex < childCount;
       lineCount++, startOfLineIndex = endOfLineIndex) {
    collectedFlexItemsValues = PilatesCalculateCollectFlexItemsRowValues(
        node,
        parentDirection,
        mainAxisParentSize,
        availableInnerWidth,
        availableInnerMainDim,
        startOfLineIndex,
        lineCount);
    endOfLineIndex = collectedFlexItemsValues.endOfLineIndex;

    // If we don't need to measure the cross axis, we can skip the entire flex
    // step.
    const bool canSkipFlex =
        !performLayout && measureModeCrossDim == PilatesMeasureModeExactly;

    // STEP 5: RESOLVING FLEXIBLE LENGTHS ON MAIN AXIS
    // Calculate the remaining available space that needs to be allocated.
    // If the main dimension size isn't known, it is computed based on
    // the line length, so there's no more space left to distribute.

    bool sizeBasedOnContent = false;
    // If we don't measure with exact main dimension we want to ensure we don't violate min and max
    if (measureModeMainDim != PilatesMeasureModeExactly) {
      if (!PilatesFloatIsUndefined(minInnerMainDim) &&
          collectedFlexItemsValues.sizeConsumedOnCurrentLine <
              minInnerMainDim) {
        availableInnerMainDim = minInnerMainDim;
      } else if (
          !PilatesFloatIsUndefined(maxInnerMainDim) &&
          collectedFlexItemsValues.sizeConsumedOnCurrentLine >
              maxInnerMainDim) {
        availableInnerMainDim = maxInnerMainDim;
      } else {
        if (!node->getConfig()->useLegacyStretchBehaviour &&
            (collectedFlexItemsValues.totalFlexGrowFactors == 0 ||
             node->resolveFlexGrow() == 0)) {
          // If we don't have any children to flex or we can't flex the node
          // itself, space we've used is all space we need. Root node also
          // should be shrunk to minimum
          availableInnerMainDim =
              collectedFlexItemsValues.sizeConsumedOnCurrentLine;
        }

        if (node->getConfig()->useLegacyStretchBehaviour) {
          node->setLayoutDidUseLegacyFlag(true);
        }
        sizeBasedOnContent = !node->getConfig()->useLegacyStretchBehaviour;
      }
    }

    if (!sizeBasedOnContent && !PilatesFloatIsUndefined(availableInnerMainDim)) {
      collectedFlexItemsValues.remainingFreeSpace = availableInnerMainDim -
          collectedFlexItemsValues.sizeConsumedOnCurrentLine;
    } else if (collectedFlexItemsValues.sizeConsumedOnCurrentLine < 0) {
      // availableInnerMainDim is indefinite which means the node is being sized based on its
      // content.
      // sizeConsumedOnCurrentLine is negative which means the node will allocate 0 points for
      // its content. Consequently, remainingFreeSpace is 0 - sizeConsumedOnCurrentLine.
      collectedFlexItemsValues.remainingFreeSpace =
          -collectedFlexItemsValues.sizeConsumedOnCurrentLine;
    }

    if (!canSkipFlex) {
      PilatesResolveFlexibleLength(
          node,
          collectedFlexItemsValues,
          mainAxis,
          crossAxis,
          mainAxisParentSize,
          availableInnerMainDim,
          availableInnerCrossDim,
          availableInnerWidth,
          availableInnerHeight,
          flexBasisOverflows,
          measureModeCrossDim,
          performLayout,
          config);
    }

    node->setLayoutHadOverflow(
        node->getLayout().hadOverflow |
        (collectedFlexItemsValues.remainingFreeSpace < 0));

    // STEP 6: MAIN-AXIS JUSTIFICATION & CROSS-AXIS SIZE DETERMINATION

    // At this point, all the children have their dimensions set in the main
    // axis.
    // Their dimensions are also set in the cross axis with the exception of
    // items
    // that are aligned "stretch". We need to compute these stretch values and
    // set the final positions.

    PilatesJustifyMainAxis(
        node,
        collectedFlexItemsValues,
        startOfLineIndex,
        mainAxis,
        crossAxis,
        measureModeMainDim,
        measureModeCrossDim,
        mainAxisParentSize,
        parentWidth,
        availableInnerMainDim,
        availableInnerCrossDim,
        availableInnerWidth,
        performLayout);

    float containerCrossAxis = availableInnerCrossDim;
    if (measureModeCrossDim == PilatesMeasureModeUndefined ||
        measureModeCrossDim == PilatesMeasureModeAtMost) {
      // Compute the cross axis from the max cross dimension of the children.
      containerCrossAxis =
          PilatesNodeBoundAxis(
              node,
              crossAxis,
              collectedFlexItemsValues.crossDim + paddingAndBorderAxisCross,
              crossAxisParentSize,
              parentWidth) -
          paddingAndBorderAxisCross;
    }

    // If there's no flex wrap, the cross dimension is defined by the container.
    if (!isNodeFlexWrap && measureModeCrossDim == PilatesMeasureModeExactly) {
      collectedFlexItemsValues.crossDim = availableInnerCrossDim;
    }

    // Clamp to the min/max size specified on the container.
    collectedFlexItemsValues.crossDim =
        PilatesNodeBoundAxis(
            node,
            crossAxis,
            collectedFlexItemsValues.crossDim + paddingAndBorderAxisCross,
            crossAxisParentSize,
            parentWidth) -
        paddingAndBorderAxisCross;

    // STEP 7: CROSS-AXIS ALIGNMENT
    // We can skip child alignment if we're just measuring the container.
    if (performLayout) {
      for (uint32_t i = startOfLineIndex; i < endOfLineIndex; i++) {
        const PilatesNodeRef child = node->getChild(i);
        if (child->getStyle().display == PilatesDisplayNone) {
          continue;
        }
        if (child->getStyle().positionType == PilatesPositionTypeAbsolute) {
          // If the child is absolutely positioned and has a
          // top/left/bottom/right set, override
          // all the previously computed positions to set it correctly.
          const bool isChildLeadingPosDefined =
              child->isLeadingPositionDefined(crossAxis);
          if (isChildLeadingPosDefined) {
            child->setLayoutPosition(
                child->getLeadingPosition(crossAxis, availableInnerCrossDim) +
                    node->getLeadingBorder(crossAxis) +
                    child->getLeadingMargin(crossAxis, availableInnerWidth),
                pos[crossAxis]);
          }
          // If leading position is not defined or calculations result in Nan, default to border + margin
          if (!isChildLeadingPosDefined ||
              PilatesFloatIsUndefined(child->getLayout().position[pos[crossAxis]])) {
            child->setLayoutPosition(
                node->getLeadingBorder(crossAxis) +
                    child->getLeadingMargin(crossAxis, availableInnerWidth),
                pos[crossAxis]);
          }
        } else {
          float leadingCrossDim = leadingPaddingAndBorderCross;

          // For a relative children, we're either using alignItems (parent) or
          // alignSelf (child) in order to determine the position in the cross
          // axis
          const PilatesAlign alignItem = PilatesNodeAlignItem(node, child);

          // If the child uses align stretch, we need to lay it out one more
          // time, this time
          // forcing the cross-axis size to be the computed cross size for the
          // current line.
          if (alignItem == PilatesAlignStretch &&
              child->marginLeadingValue(crossAxis).unit != PilatesUnitAuto &&
              child->marginTrailingValue(crossAxis).unit != PilatesUnitAuto) {
            // If the child defines a definite size for its cross axis, there's
            // no need to stretch.
            if (!PilatesNodeIsStyleDimDefined(child, crossAxis, availableInnerCrossDim)) {
              float childMainSize =
                  child->getLayout().measuredDimensions[dim[mainAxis]];
              float childCrossSize =
                  !PilatesFloatIsUndefined(child->getStyle().aspectRatio)
                  ? ((child->getMarginForAxis(crossAxis, availableInnerWidth) +
                      (isMainAxisRow
                           ? childMainSize / child->getStyle().aspectRatio
                           : childMainSize * child->getStyle().aspectRatio)))
                  : collectedFlexItemsValues.crossDim;

              childMainSize +=
                  child->getMarginForAxis(mainAxis, availableInnerWidth);

              PilatesMeasureMode childMainMeasureMode = PilatesMeasureModeExactly;
              PilatesMeasureMode childCrossMeasureMode = PilatesMeasureModeExactly;
              PilatesConstrainMaxSizeForMode(child,
                                        mainAxis,
                                        availableInnerMainDim,
                                        availableInnerWidth,
                                        &childMainMeasureMode,
                                        &childMainSize);
              PilatesConstrainMaxSizeForMode(child,
                                        crossAxis,
                                        availableInnerCrossDim,
                                        availableInnerWidth,
                                        &childCrossMeasureMode,
                                        &childCrossSize);

              const float childWidth = isMainAxisRow ? childMainSize : childCrossSize;
              const float childHeight = !isMainAxisRow ? childMainSize : childCrossSize;

              const PilatesMeasureMode childWidthMeasureMode =
                  PilatesFloatIsUndefined(childWidth) ? PilatesMeasureModeUndefined
                                                 : PilatesMeasureModeExactly;
              const PilatesMeasureMode childHeightMeasureMode =
                  PilatesFloatIsUndefined(childHeight) ? PilatesMeasureModeUndefined
                                                  : PilatesMeasureModeExactly;

              PilatesLayoutNodeInternal(
                  child,
                  childWidth,
                  childHeight,
                  direction,
                  childWidthMeasureMode,
                  childHeightMeasureMode,
                  availableInnerWidth,
                  availableInnerHeight,
                  true,
                  "stretch",
                  config);
            }
          } else {
            const float remainingCrossDim = containerCrossAxis -
                PilatesNodeDimWithMargin(child, crossAxis, availableInnerWidth);

            if (child->marginLeadingValue(crossAxis).unit == PilatesUnitAuto &&
                child->marginTrailingValue(crossAxis).unit == PilatesUnitAuto) {
              leadingCrossDim += fmaxf(0.0f, remainingCrossDim / 2);
            } else if (
                child->marginTrailingValue(crossAxis).unit == PilatesUnitAuto) {
              // No-Op
            } else if (
                child->marginLeadingValue(crossAxis).unit == PilatesUnitAuto) {
              leadingCrossDim += fmaxf(0.0f, remainingCrossDim);
            } else if (alignItem == PilatesAlignFlexStart) {
              // No-Op
            } else if (alignItem == PilatesAlignCenter) {
              leadingCrossDim += remainingCrossDim / 2;
            } else {
              leadingCrossDim += remainingCrossDim;
            }
          }
          // And we apply the position
          child->setLayoutPosition(
              child->getLayout().position[pos[crossAxis]] + totalLineCrossDim +
                  leadingCrossDim,
              pos[crossAxis]);
        }
      }
    }

    totalLineCrossDim += collectedFlexItemsValues.crossDim;
    maxLineMainDim = fmaxf(maxLineMainDim, collectedFlexItemsValues.mainDim);
  }

  // STEP 8: MULTI-LINE CONTENT ALIGNMENT
  if (performLayout && (lineCount > 1 || PilatesIsBaselineLayout(node)) &&
      !PilatesFloatIsUndefined(availableInnerCrossDim)) {
    const float remainingAlignContentDim = availableInnerCrossDim - totalLineCrossDim;

    float crossDimLead = 0;
    float currentLead = leadingPaddingAndBorderCross;

    switch (node->getStyle().alignContent) {
      case PilatesAlignFlexEnd:
        currentLead += remainingAlignContentDim;
        break;
      case PilatesAlignCenter:
        currentLead += remainingAlignContentDim / 2;
        break;
      case PilatesAlignStretch:
        if (availableInnerCrossDim > totalLineCrossDim) {
          crossDimLead = remainingAlignContentDim / lineCount;
        }
        break;
      case PilatesAlignSpaceAround:
        if (availableInnerCrossDim > totalLineCrossDim) {
          currentLead += remainingAlignContentDim / (2 * lineCount);
          if (lineCount > 1) {
            crossDimLead = remainingAlignContentDim / lineCount;
          }
        } else {
          currentLead += remainingAlignContentDim / 2;
        }
        break;
      case PilatesAlignSpaceBetween:
        if (availableInnerCrossDim > totalLineCrossDim && lineCount > 1) {
          crossDimLead = remainingAlignContentDim / (lineCount - 1);
        }
        break;
      case PilatesAlignAuto:
      case PilatesAlignFlexStart:
      case PilatesAlignBaseline:
        break;
    }

    uint32_t endIndex = 0;
    for (uint32_t i = 0; i < lineCount; i++) {
      const uint32_t startIndex = endIndex;
      uint32_t ii;

      // compute the line's height and find the endIndex
      float lineHeight = 0;
      float maxAscentForCurrentLine = 0;
      float maxDescentForCurrentLine = 0;
      for (ii = startIndex; ii < childCount; ii++) {
        const PilatesNodeRef child = node->getChild(ii);
        if (child->getStyle().display == PilatesDisplayNone) {
          continue;
        }
        if (child->getStyle().positionType == PilatesPositionTypeRelative) {
          if (child->getLineIndex() != i) {
            break;
          }
          if (PilatesNodeIsLayoutDimDefined(child, crossAxis)) {
            lineHeight = fmaxf(
                lineHeight,
                child->getLayout().measuredDimensions[dim[crossAxis]] +
                    child->getMarginForAxis(crossAxis, availableInnerWidth));
          }
          if (PilatesNodeAlignItem(node, child) == PilatesAlignBaseline) {
            const float ascent = PilatesBaseline(child) +
                child->getLeadingMargin(
                    PilatesFlexDirectionColumn, availableInnerWidth);
            const float descent =
                child->getLayout().measuredDimensions[PilatesDimensionHeight] +
                child->getMarginForAxis(
                    PilatesFlexDirectionColumn, availableInnerWidth) -
                ascent;
            maxAscentForCurrentLine = fmaxf(maxAscentForCurrentLine, ascent);
            maxDescentForCurrentLine = fmaxf(maxDescentForCurrentLine, descent);
            lineHeight = fmaxf(lineHeight, maxAscentForCurrentLine + maxDescentForCurrentLine);
          }
        }
      }
      endIndex = ii;
      lineHeight += crossDimLead;

      if (performLayout) {
        for (ii = startIndex; ii < endIndex; ii++) {
          const PilatesNodeRef child = node->getChild(ii);
          if (child->getStyle().display == PilatesDisplayNone) {
            continue;
          }
          if (child->getStyle().positionType == PilatesPositionTypeRelative) {
            switch (PilatesNodeAlignItem(node, child)) {
              case PilatesAlignFlexStart: {
                child->setLayoutPosition(
                    currentLead +
                        child->getLeadingMargin(crossAxis, availableInnerWidth),
                    pos[crossAxis]);
                break;
              }
              case PilatesAlignFlexEnd: {
                child->setLayoutPosition(
                    currentLead + lineHeight -
                        child->getTrailingMargin(
                            crossAxis, availableInnerWidth) -
                        child->getLayout().measuredDimensions[dim[crossAxis]],
                    pos[crossAxis]);
                break;
              }
              case PilatesAlignCenter: {
                float childHeight =
                    child->getLayout().measuredDimensions[dim[crossAxis]];

                child->setLayoutPosition(
                    currentLead + (lineHeight - childHeight) / 2,
                    pos[crossAxis]);
                break;
              }
              case PilatesAlignStretch: {
                child->setLayoutPosition(
                    currentLead +
                        child->getLeadingMargin(crossAxis, availableInnerWidth),
                    pos[crossAxis]);

                // Remeasure child with the line height as it as been only measured with the
                // parents height yet.
                if (!PilatesNodeIsStyleDimDefined(child, crossAxis, availableInnerCrossDim)) {
                  const float childWidth = isMainAxisRow
                      ? (child->getLayout()
                             .measuredDimensions[PilatesDimensionWidth] +
                         child->getMarginForAxis(mainAxis, availableInnerWidth))
                      : lineHeight;

                  const float childHeight = !isMainAxisRow
                      ? (child->getLayout()
                             .measuredDimensions[PilatesDimensionHeight] +
                         child->getMarginForAxis(
                             crossAxis, availableInnerWidth))
                      : lineHeight;

                  if (!(PilatesFloatsEqual(
                            childWidth,
                            child->getLayout()
                                .measuredDimensions[PilatesDimensionWidth]) &&
                        PilatesFloatsEqual(
                            childHeight,
                            child->getLayout()
                                .measuredDimensions[PilatesDimensionHeight]))) {
                    PilatesLayoutNodeInternal(child,
                                         childWidth,
                                         childHeight,
                                         direction,
                                         PilatesMeasureModeExactly,
                                         PilatesMeasureModeExactly,
                                         availableInnerWidth,
                                         availableInnerHeight,
                                         true,
                                         "multiline-stretch",
                                         config);
                  }
                }
                break;
              }
              case PilatesAlignBaseline: {
                child->setLayoutPosition(
                    currentLead + maxAscentForCurrentLine - PilatesBaseline(child) +
                        child->getLeadingPosition(
                            PilatesFlexDirectionColumn, availableInnerCrossDim),
                    PilatesEdgeTop);

                break;
              }
              case PilatesAlignAuto:
              case PilatesAlignSpaceBetween:
              case PilatesAlignSpaceAround:
                break;
            }
          }
        }
      }

      currentLead += lineHeight;
    }
  }

  // STEP 9: COMPUTING FINAL DIMENSIONS

  node->setLayoutMeasuredDimension(
      PilatesNodeBoundAxis(
          node,
          PilatesFlexDirectionRow,
          availableWidth - marginAxisRow,
          parentWidth,
          parentWidth),
      PilatesDimensionWidth);

  node->setLayoutMeasuredDimension(
      PilatesNodeBoundAxis(
          node,
          PilatesFlexDirectionColumn,
          availableHeight - marginAxisColumn,
          parentHeight,
          parentWidth),
      PilatesDimensionHeight);

  // If the user didn't specify a width or height for the node, set the
  // dimensions based on the children.
  if (measureModeMainDim == PilatesMeasureModeUndefined ||
      (node->getStyle().overflow != PilatesOverflowScroll &&
       measureModeMainDim == PilatesMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.
    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node, mainAxis, maxLineMainDim, mainAxisParentSize, parentWidth),
        dim[mainAxis]);

  } else if (
      measureModeMainDim == PilatesMeasureModeAtMost &&
      node->getStyle().overflow == PilatesOverflowScroll) {
    node->setLayoutMeasuredDimension(
        fmaxf(
            fminf(
                availableInnerMainDim + paddingAndBorderAxisMain,
                PilatesNodeBoundAxisWithinMinAndMax(
                    node, mainAxis, maxLineMainDim, mainAxisParentSize)),
            paddingAndBorderAxisMain),
        dim[mainAxis]);
  }

  if (measureModeCrossDim == PilatesMeasureModeUndefined ||
      (node->getStyle().overflow != PilatesOverflowScroll &&
       measureModeCrossDim == PilatesMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.

    node->setLayoutMeasuredDimension(
        PilatesNodeBoundAxis(
            node,
            crossAxis,
            totalLineCrossDim + paddingAndBorderAxisCross,
            crossAxisParentSize,
            parentWidth),
        dim[crossAxis]);

  } else if (
      measureModeCrossDim == PilatesMeasureModeAtMost &&
      node->getStyle().overflow == PilatesOverflowScroll) {
    node->setLayoutMeasuredDimension(
        fmaxf(
            fminf(
                availableInnerCrossDim + paddingAndBorderAxisCross,
                PilatesNodeBoundAxisWithinMinAndMax(
                    node,
                    crossAxis,
                    totalLineCrossDim + paddingAndBorderAxisCross,
                    crossAxisParentSize)),
            paddingAndBorderAxisCross),
        dim[crossAxis]);
  }

  // As we only wrapped in normal direction yet, we need to reverse the positions on wrap-reverse.
  if (performLayout && node->getStyle().flexWrap == PilatesWrapWrapReverse) {
    for (uint32_t i = 0; i < childCount; i++) {
      const PilatesNodeRef child = PilatesNodeGetChild(node, i);
      if (child->getStyle().positionType == PilatesPositionTypeRelative) {
        child->setLayoutPosition(
            node->getLayout().measuredDimensions[dim[crossAxis]] -
                child->getLayout().position[pos[crossAxis]] -
                child->getLayout().measuredDimensions[dim[crossAxis]],
            pos[crossAxis]);
      }
    }
  }

  if (performLayout) {
    // STEP 10: SIZING AND POSITIONING ABSOLUTE CHILDREN
    for (auto child : node->getChildren()) {
      if (child->getStyle().positionType != PilatesPositionTypeAbsolute) {
        continue;
      }
      PilatesNodeAbsoluteLayoutChild(
          node,
          child,
          availableInnerWidth,
          isMainAxisRow ? measureModeMainDim : measureModeCrossDim,
          availableInnerHeight,
          direction,
          config);
    }

    // STEP 11: SETTING TRAILING POSITIONS FOR CHILDREN
    const bool needsMainTrailingPos =
        mainAxis == PilatesFlexDirectionRowReverse || mainAxis == PilatesFlexDirectionColumnReverse;
    const bool needsCrossTrailingPos =
        crossAxis == PilatesFlexDirectionRowReverse || crossAxis == PilatesFlexDirectionColumnReverse;

    // Set trailing position if necessary.
    if (needsMainTrailingPos || needsCrossTrailingPos) {
      for (uint32_t i = 0; i < childCount; i++) {
        const PilatesNodeRef child = node->getChild(i);
        if (child->getStyle().display == PilatesDisplayNone) {
          continue;
        }
        if (needsMainTrailingPos) {
          PilatesNodeSetChildTrailingPosition(node, child, mainAxis);
        }

        if (needsCrossTrailingPos) {
          PilatesNodeSetChildTrailingPosition(node, child, crossAxis);
        }
      }
    }
  }
}

uint32_t gDepth = 0;
bool gPrintTree = false;
bool gPrintChanges = false;
bool gPrintSkips = false;

static const char *spacer = "                                                            ";

static const char *PilatesSpacer(const unsigned long level) {
  const size_t spacerLen = strlen(spacer);
  if (level > spacerLen) {
    return &spacer[0];
  } else {
    return &spacer[spacerLen - level];
  }
}

static const char *PilatesMeasureModeName(const PilatesMeasureMode mode, const bool performLayout) {
  const char *kMeasureModeNames[PilatesMeasureModeCount] = {"UNDEFINED", "EXACTLY", "AT_MOST"};
  const char *kLayoutModeNames[PilatesMeasureModeCount] = {"LAY_UNDEFINED",
                                                      "LAY_EXACTLY",
                                                      "LAY_AT_"
                                                      "MOST"};

  if (mode >= PilatesMeasureModeCount) {
    return "";
  }

  return performLayout ? kLayoutModeNames[mode] : kMeasureModeNames[mode];
}

static inline bool PilatesMeasureModeSizeIsExactAndMatchesOldMeasuredSize(PilatesMeasureMode sizeMode,
                                                                     float size,
                                                                     float lastComputedSize) {
  return sizeMode == PilatesMeasureModeExactly && PilatesFloatsEqual(size, lastComputedSize);
}

static inline bool PilatesMeasureModeOldSizeIsUnspecifiedAndStillFits(PilatesMeasureMode sizeMode,
                                                                 float size,
                                                                 PilatesMeasureMode lastSizeMode,
                                                                 float lastComputedSize) {
  return sizeMode == PilatesMeasureModeAtMost && lastSizeMode == PilatesMeasureModeUndefined &&
         (size >= lastComputedSize || PilatesFloatsEqual(size, lastComputedSize));
}

static inline bool PilatesMeasureModeNewMeasureSizeIsStricterAndStillValid(PilatesMeasureMode sizeMode,
                                                                      float size,
                                                                      PilatesMeasureMode lastSizeMode,
                                                                      float lastSize,
                                                                      float lastComputedSize) {
  return lastSizeMode == PilatesMeasureModeAtMost && sizeMode == PilatesMeasureModeAtMost &&
         lastSize > size && (lastComputedSize <= size || PilatesFloatsEqual(size, lastComputedSize));
}

float PilatesRoundValueToPixelGrid(const float value,
                              const float pointScaleFactor,
                              const bool forceCeil,
                              const bool forceFloor) {
  const float roundingError = fmax(0.0001, 0.01 * pointScaleFactor);
  float scaledValue = value * pointScaleFactor;
  float fractial = fmodf(scaledValue, 1.0);
  if (PilatesFloatsEqualWithPrecision(fractial, 0.0, roundingError)) {
    // First we check if the value is already rounded
    scaledValue = scaledValue - fractial;
  } else if (PilatesFloatsEqualWithPrecision(fractial, 1.0, roundingError)) {
    scaledValue = scaledValue - fractial + 1.0;
  } else if (forceCeil) {
    // Next we check if we need to use forced rounding
    scaledValue = scaledValue - fractial + 1.0f;
  } else if (forceFloor) {
    scaledValue = scaledValue - fractial;
  } else {
    // Finally we just round the value
    scaledValue = scaledValue - fractial +
        (fractial > 0.5f || PilatesFloatsEqualWithPrecision(fractial, 0.5f, roundingError) ? 1.0f : 0.0f);
  }
  return scaledValue / pointScaleFactor;
}

bool PilatesNodeCanUseCachedMeasurement(const PilatesMeasureMode widthMode,
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
                                   const PilatesConfigRef config) {
  if (lastComputedHeight < 0 || lastComputedWidth < 0) {
    return false;
  }
  bool useRoundedComparison =
      config != nullptr && config->pointScaleFactor != 0;
  const float effectiveWidth =
      useRoundedComparison ? PilatesRoundValueToPixelGrid(width, config->pointScaleFactor, false, false)
                           : width;
  const float effectiveHeight =
      useRoundedComparison ? PilatesRoundValueToPixelGrid(height, config->pointScaleFactor, false, false)
                           : height;
  const float effectiveLastWidth =
      useRoundedComparison
          ? PilatesRoundValueToPixelGrid(lastWidth, config->pointScaleFactor, false, false)
          : lastWidth;
  const float effectiveLastHeight =
      useRoundedComparison
          ? PilatesRoundValueToPixelGrid(lastHeight, config->pointScaleFactor, false, false)
          : lastHeight;

  const bool hasSameWidthSpec =
      lastWidthMode == widthMode && PilatesFloatsEqual(effectiveLastWidth, effectiveWidth);
  const bool hasSameHeightSpec =
      lastHeightMode == heightMode && PilatesFloatsEqual(effectiveLastHeight, effectiveHeight);

  const bool widthIsCompatible =
      hasSameWidthSpec || PilatesMeasureModeSizeIsExactAndMatchesOldMeasuredSize(widthMode,
                                                                            width - marginRow,
                                                                            lastComputedWidth) ||
      PilatesMeasureModeOldSizeIsUnspecifiedAndStillFits(widthMode,
                                                    width - marginRow,
                                                    lastWidthMode,
                                                    lastComputedWidth) ||
      PilatesMeasureModeNewMeasureSizeIsStricterAndStillValid(
          widthMode, width - marginRow, lastWidthMode, lastWidth, lastComputedWidth);

  const bool heightIsCompatible =
      hasSameHeightSpec || PilatesMeasureModeSizeIsExactAndMatchesOldMeasuredSize(heightMode,
                                                                             height - marginColumn,
                                                                             lastComputedHeight) ||
      PilatesMeasureModeOldSizeIsUnspecifiedAndStillFits(heightMode,
                                                    height - marginColumn,
                                                    lastHeightMode,
                                                    lastComputedHeight) ||
      PilatesMeasureModeNewMeasureSizeIsStricterAndStillValid(
          heightMode, height - marginColumn, lastHeightMode, lastHeight, lastComputedHeight);

  return widthIsCompatible && heightIsCompatible;
}

//
// This is a wrapper around the PilatesNodelayoutImpl function. It determines
// whether the layout request is redundant and can be skipped.
//
// Parameters:
//  Input parameters are the same as PilatesNodelayoutImpl (see above)
//  Return parameter is true if layout was performed, false if skipped
//
bool PilatesLayoutNodeInternal(const PilatesNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const PilatesDirection parentDirection,
                          const PilatesMeasureMode widthMeasureMode,
                          const PilatesMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason,
                          const PilatesConfigRef config) {
  PilatesLayout* layout = &node->getLayout();

  gDepth++;

  const bool needToVisitNode =
      (node->isDirty() && layout->generationCount != gCurrentGenerationCount) ||
      layout->lastParentDirection != parentDirection;

  if (needToVisitNode) {
    // Invalidate the cached results.
    layout->nextCachedMeasurementsIndex = 0;
    layout->cachedLayout.widthMeasureMode = (PilatesMeasureMode) -1;
    layout->cachedLayout.heightMeasureMode = (PilatesMeasureMode) -1;
    layout->cachedLayout.computedWidth = -1;
    layout->cachedLayout.computedHeight = -1;
  }

  PilatesCachedMeasurement* cachedResults = nullptr;

  // Determine whether the results are already cached. We maintain a separate
  // cache for layouts and measurements. A layout operation modifies the
  // positions
  // and dimensions for nodes in the subtree. The algorithm assumes that each
  // node
  // gets layed out a maximum of one time per tree layout, but multiple
  // measurements
  // may be required to resolve all of the flex dimensions.
  // We handle nodes with measure functions specially here because they are the
  // most
  // expensive to measure, so it's worth avoiding redundant measurements if at
  // all possible.
  if (node->getMeasure() != nullptr) {
    const float marginAxisRow =
        node->getMarginForAxis(PilatesFlexDirectionRow, parentWidth);
    const float marginAxisColumn =
        node->getMarginForAxis(PilatesFlexDirectionColumn, parentWidth);

    // First, try to use the layout cache.
    if (PilatesNodeCanUseCachedMeasurement(widthMeasureMode,
                                      availableWidth,
                                      heightMeasureMode,
                                      availableHeight,
                                      layout->cachedLayout.widthMeasureMode,
                                      layout->cachedLayout.availableWidth,
                                      layout->cachedLayout.heightMeasureMode,
                                      layout->cachedLayout.availableHeight,
                                      layout->cachedLayout.computedWidth,
                                      layout->cachedLayout.computedHeight,
                                      marginAxisRow,
                                      marginAxisColumn,
                                      config)) {
      cachedResults = &layout->cachedLayout;
    } else {
      // Try to use the measurement cache.
      for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
        if (PilatesNodeCanUseCachedMeasurement(widthMeasureMode,
                                          availableWidth,
                                          heightMeasureMode,
                                          availableHeight,
                                          layout->cachedMeasurements[i].widthMeasureMode,
                                          layout->cachedMeasurements[i].availableWidth,
                                          layout->cachedMeasurements[i].heightMeasureMode,
                                          layout->cachedMeasurements[i].availableHeight,
                                          layout->cachedMeasurements[i].computedWidth,
                                          layout->cachedMeasurements[i].computedHeight,
                                          marginAxisRow,
                                          marginAxisColumn,
                                          config)) {
          cachedResults = &layout->cachedMeasurements[i];
          break;
        }
      }
    }
  } else if (performLayout) {
    if (PilatesFloatsEqual(layout->cachedLayout.availableWidth, availableWidth) &&
        PilatesFloatsEqual(layout->cachedLayout.availableHeight, availableHeight) &&
        layout->cachedLayout.widthMeasureMode == widthMeasureMode &&
        layout->cachedLayout.heightMeasureMode == heightMeasureMode) {
      cachedResults = &layout->cachedLayout;
    }
  } else {
    for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
      if (PilatesFloatsEqual(layout->cachedMeasurements[i].availableWidth, availableWidth) &&
          PilatesFloatsEqual(layout->cachedMeasurements[i].availableHeight, availableHeight) &&
          layout->cachedMeasurements[i].widthMeasureMode == widthMeasureMode &&
          layout->cachedMeasurements[i].heightMeasureMode == heightMeasureMode) {
        cachedResults = &layout->cachedMeasurements[i];
        break;
      }
    }
  }

  if (!needToVisitNode && cachedResults != nullptr) {
    layout->measuredDimensions[PilatesDimensionWidth] = cachedResults->computedWidth;
    layout->measuredDimensions[PilatesDimensionHeight] = cachedResults->computedHeight;

    if (gPrintChanges && gPrintSkips) {
      PilatesLog(node, PilatesLogLevelVerbose, "%s%d.{[skipped] ", PilatesSpacer(gDepth), gDepth);
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
      PilatesLog(
          node,
          PilatesLogLevelVerbose,
          "wm: %s, hm: %s, aw: %f ah: %f => d: (%f, %f) %s\n",
          PilatesMeasureModeName(widthMeasureMode, performLayout),
          PilatesMeasureModeName(heightMeasureMode, performLayout),
          availableWidth,
          availableHeight,
          cachedResults->computedWidth,
          cachedResults->computedHeight,
          reason);
    }
  } else {
    if (gPrintChanges) {
      PilatesLog(
          node,
          PilatesLogLevelVerbose,
          "%s%d.{%s",
          PilatesSpacer(gDepth),
          gDepth,
          needToVisitNode ? "*" : "");
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
      PilatesLog(
          node,
          PilatesLogLevelVerbose,
          "wm: %s, hm: %s, aw: %f ah: %f %s\n",
          PilatesMeasureModeName(widthMeasureMode, performLayout),
          PilatesMeasureModeName(heightMeasureMode, performLayout),
          availableWidth,
          availableHeight,
          reason);
    }

    PilatesNodelayoutImpl(node,
                     availableWidth,
                     availableHeight,
                     parentDirection,
                     widthMeasureMode,
                     heightMeasureMode,
                     parentWidth,
                     parentHeight,
                     performLayout,
                     config);

    if (gPrintChanges) {
      PilatesLog(
          node,
          PilatesLogLevelVerbose,
          "%s%d.}%s",
          PilatesSpacer(gDepth),
          gDepth,
          needToVisitNode ? "*" : "");
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
      PilatesLog(
          node,
          PilatesLogLevelVerbose,
          "wm: %s, hm: %s, d: (%f, %f) %s\n",
          PilatesMeasureModeName(widthMeasureMode, performLayout),
          PilatesMeasureModeName(heightMeasureMode, performLayout),
          layout->measuredDimensions[PilatesDimensionWidth],
          layout->measuredDimensions[PilatesDimensionHeight],
          reason);
    }

    layout->lastParentDirection = parentDirection;

    if (cachedResults == nullptr) {
      if (layout->nextCachedMeasurementsIndex == Pilates_MAX_CACHED_RESULT_COUNT) {
        if (gPrintChanges) {
          PilatesLog(node, PilatesLogLevelVerbose, "Out of cache entries!\n");
        }
        layout->nextCachedMeasurementsIndex = 0;
      }

      PilatesCachedMeasurement *newCacheEntry;
      if (performLayout) {
        // Use the single layout cache entry.
        newCacheEntry = &layout->cachedLayout;
      } else {
        // Allocate a new measurement cache entry.
        newCacheEntry = &layout->cachedMeasurements[layout->nextCachedMeasurementsIndex];
        layout->nextCachedMeasurementsIndex++;
      }

      newCacheEntry->availableWidth = availableWidth;
      newCacheEntry->availableHeight = availableHeight;
      newCacheEntry->widthMeasureMode = widthMeasureMode;
      newCacheEntry->heightMeasureMode = heightMeasureMode;
      newCacheEntry->computedWidth = layout->measuredDimensions[PilatesDimensionWidth];
      newCacheEntry->computedHeight = layout->measuredDimensions[PilatesDimensionHeight];
    }
  }

  if (performLayout) {
    node->setLayoutDimension(
        node->getLayout().measuredDimensions[PilatesDimensionWidth],
        PilatesDimensionWidth);
    node->setLayoutDimension(
        node->getLayout().measuredDimensions[PilatesDimensionHeight],
        PilatesDimensionHeight);

    node->setHasNewLayout(true);
    node->setDirty(false);
  }

  gDepth--;
  layout->generationCount = gCurrentGenerationCount;
  return (needToVisitNode || cachedResults == nullptr);
}

void PilatesConfigSetPointScaleFactor(const PilatesConfigRef config, const float pixelsInPoint) {
  PilatesAssertWithConfig(config, pixelsInPoint >= 0.0f, "Scale factor should not be less than zero");

  // We store points for Pixel as we will use it for rounding
  if (pixelsInPoint == 0.0f) {
    // Zero is used to skip rounding
    config->pointScaleFactor = 0.0f;
  } else {
    config->pointScaleFactor = pixelsInPoint;
  }
}

static void PilatesRoundToPixelGrid(const PilatesNodeRef node,
                               const float pointScaleFactor,
                               const float absoluteLeft,
                               const float absoluteTop) {
  if (pointScaleFactor == 0.0f) {
    return;
  }

  const float nodeLeft = node->getLayout().position[PilatesEdgeLeft];
  const float nodeTop = node->getLayout().position[PilatesEdgeTop];

  const float nodeWidth = node->getLayout().dimensions[PilatesDimensionWidth];
  const float nodeHeight = node->getLayout().dimensions[PilatesDimensionHeight];

  const float absoluteNodeLeft = absoluteLeft + nodeLeft;
  const float absoluteNodeTop = absoluteTop + nodeTop;

  const float absoluteNodeRight = absoluteNodeLeft + nodeWidth;
  const float absoluteNodeBottom = absoluteNodeTop + nodeHeight;

  // If a node has a custom measure function we never want to round down its size as this could
  // lead to unwanted text truncation.
  const bool textRounding = node->getNodeType() == PilatesNodeTypeText;

  node->setLayoutPosition(
      PilatesRoundValueToPixelGrid(nodeLeft, pointScaleFactor, false, textRounding),
      PilatesEdgeLeft);

  node->setLayoutPosition(
      PilatesRoundValueToPixelGrid(nodeTop, pointScaleFactor, false, textRounding),
      PilatesEdgeTop);

  // We multiply dimension by scale factor and if the result is close to the whole number, we don't
  // have any fraction
  // To verify if the result is close to whole number we want to check both floor and ceil numbers
  const bool hasFractionalWidth = !PilatesFloatsEqual(fmodf(nodeWidth * pointScaleFactor, 1.0), 0) &&
                                  !PilatesFloatsEqual(fmodf(nodeWidth * pointScaleFactor, 1.0), 1.0);
  const bool hasFractionalHeight = !PilatesFloatsEqual(fmodf(nodeHeight * pointScaleFactor, 1.0), 0) &&
                                   !PilatesFloatsEqual(fmodf(nodeHeight * pointScaleFactor, 1.0), 1.0);

  node->setLayoutDimension(
      PilatesRoundValueToPixelGrid(
          absoluteNodeRight,
          pointScaleFactor,
          (textRounding && hasFractionalWidth),
          (textRounding && !hasFractionalWidth)) -
          PilatesRoundValueToPixelGrid(
              absoluteNodeLeft, pointScaleFactor, false, textRounding),
      PilatesDimensionWidth);

  node->setLayoutDimension(
      PilatesRoundValueToPixelGrid(
          absoluteNodeBottom,
          pointScaleFactor,
          (textRounding && hasFractionalHeight),
          (textRounding && !hasFractionalHeight)) -
          PilatesRoundValueToPixelGrid(
              absoluteNodeTop, pointScaleFactor, false, textRounding),
      PilatesDimensionHeight);

  const uint32_t childCount = PilatesNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    PilatesRoundToPixelGrid(PilatesNodeGetChild(node, i), pointScaleFactor, absoluteNodeLeft, absoluteNodeTop);
  }
}

void PilatesNodeCalculateLayout(const PilatesNodeRef node,
                           const float parentWidth,
                           const float parentHeight,
                           const PilatesDirection parentDirection) {
  // Increment the generation count. This will force the recursive routine to
  // visit
  // all dirty nodes at least once. Subsequent visits will be skipped if the
  // input
  // parameters don't change.
  gCurrentGenerationCount++;
  node->resolveDimension();
  float width = PilatesUndefined;
  PilatesMeasureMode widthMeasureMode = PilatesMeasureModeUndefined;
  if (PilatesNodeIsStyleDimDefined(node, PilatesFlexDirectionRow, parentWidth)) {
    width =
        PilatesResolveValue(
            node->getResolvedDimension(dim[PilatesFlexDirectionRow]), parentWidth) +
        node->getMarginForAxis(PilatesFlexDirectionRow, parentWidth);
    widthMeasureMode = PilatesMeasureModeExactly;
  } else if (
      PilatesResolveValue(
          node->getStyle().maxDimensions[PilatesDimensionWidth], parentWidth) >=
      0.0f) {
    width = PilatesResolveValue(
        node->getStyle().maxDimensions[PilatesDimensionWidth], parentWidth);
    widthMeasureMode = PilatesMeasureModeAtMost;
  } else {
    width = parentWidth;
    widthMeasureMode = PilatesFloatIsUndefined(width) ? PilatesMeasureModeUndefined : PilatesMeasureModeExactly;
  }

  float height = PilatesUndefined;
  PilatesMeasureMode heightMeasureMode = PilatesMeasureModeUndefined;
  if (PilatesNodeIsStyleDimDefined(node, PilatesFlexDirectionColumn, parentHeight)) {
    height = PilatesResolveValue(
                 node->getResolvedDimension(dim[PilatesFlexDirectionColumn]),
                 parentHeight) +
        node->getMarginForAxis(PilatesFlexDirectionColumn, parentWidth);
    heightMeasureMode = PilatesMeasureModeExactly;
  } else if (
      PilatesResolveValue(
          node->getStyle().maxDimensions[PilatesDimensionHeight], parentHeight) >=
      0.0f) {
    height = PilatesResolveValue(
        node->getStyle().maxDimensions[PilatesDimensionHeight], parentHeight);
    heightMeasureMode = PilatesMeasureModeAtMost;
  } else {
    height = parentHeight;
    heightMeasureMode = PilatesFloatIsUndefined(height) ? PilatesMeasureModeUndefined : PilatesMeasureModeExactly;
  }

  if (PilatesLayoutNodeInternal(
          node,
          width,
          height,
          parentDirection,
          widthMeasureMode,
          heightMeasureMode,
          parentWidth,
          parentHeight,
          true,
          "initial",
          node->getConfig())) {
    node->setPosition(
        node->getLayout().direction, parentWidth, parentHeight, parentWidth);
    PilatesRoundToPixelGrid(node, node->getConfig()->pointScaleFactor, 0.0f, 0.0f);

    if (gPrintTree) {
      PilatesNodePrint(
          node,
          (PilatesPrintOptions)(
              PilatesPrintOptionsLayout | PilatesPrintOptionsChildren |
              PilatesPrintOptionsStyle));
    }
  }

  // We want to get rid off `useLegacyStretchBehaviour` from PilatesConfig. But we
  // aren't sure whether client's of pilates have gotten rid off this flag or not.
  // So logging this in PilatesLayout would help to find out the call sites depending
  // on this flag. This check would be removed once we are sure no one is
  // dependent on this flag anymore. The flag
  // `shouldDiffLayoutWithoutLegacyStretchBehaviour` in PilatesConfig will help to
  // run experiments.
  if (node->getConfig()->shouldDiffLayoutWithoutLegacyStretchBehaviour &&
      node->didUseLegacyFlag()) {
    const PilatesNodeRef originalNode = PilatesNodeDeepClone(node);
    originalNode->resolveDimension();
    // Recursively mark nodes as dirty
    originalNode->markDirtyAndPropogateDownwards();
    gCurrentGenerationCount++;
    // Rerun the layout, and calculate the diff
    originalNode->setAndPropogateUseLegacyFlag(false);
    if (PilatesLayoutNodeInternal(
            originalNode,
            width,
            height,
            parentDirection,
            widthMeasureMode,
            heightMeasureMode,
            parentWidth,
            parentHeight,
            true,
            "initial",
            originalNode->getConfig())) {
      originalNode->setPosition(
          originalNode->getLayout().direction,
          parentWidth,
          parentHeight,
          parentWidth);
      PilatesRoundToPixelGrid(
          originalNode,
          originalNode->getConfig()->pointScaleFactor,
          0.0f,
          0.0f);

      // Set whether the two layouts are different or not.
      node->setLayoutDoesLegacyFlagAffectsLayout(
          !originalNode->isLayoutTreeEqualToNode(*node));

      if (gPrintTree) {
        PilatesNodePrint(
            originalNode,
            (PilatesPrintOptions)(
                PilatesPrintOptionsLayout | PilatesPrintOptionsChildren |
                PilatesPrintOptionsStyle));
      }
    }
    PilatesConfigFreeRecursive(originalNode);
    PilatesNodeFreeRecursive(originalNode);
  }
}

void PilatesConfigSetLogger(const PilatesConfigRef config, PilatesLogger logger) {
  if (logger != nullptr) {
    config->logger = logger;
  } else {
#ifdef ANDROID
    config->logger = &PilatesAndroidLog;
#else
    config->logger = &PilatesDefaultLog;
#endif
  }
}

static void PilatesVLog(const PilatesConfigRef config,
                   const PilatesNodeRef node,
                   PilatesLogLevel level,
                   const char *format,
                   va_list args) {
  const PilatesConfigRef logConfig = config != nullptr ? config : &gPilatesConfigDefaults;
  logConfig->logger(logConfig, node, level, format, args);

  if (level == PilatesLogLevelFatal) {
    abort();
  }
}

void PilatesLogWithConfig(const PilatesConfigRef config, PilatesLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
  PilatesVLog(config, nullptr, level, format, args);
  va_end(args);
}

void PilatesLog(const PilatesNodeRef node, PilatesLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
  PilatesVLog(
      node == nullptr ? nullptr : node->getConfig(), node, level, format, args);
  va_end(args);
}

void PilatesAssert(const bool condition, const char *message) {
  if (!condition) {
    PilatesLog(nullptr, PilatesLogLevelFatal, "%s\n", message);
  }
}

void PilatesAssertWithNode(const PilatesNodeRef node, const bool condition, const char *message) {
  if (!condition) {
    PilatesLog(node, PilatesLogLevelFatal, "%s\n", message);
  }
}

void PilatesAssertWithConfig(const PilatesConfigRef config, const bool condition, const char *message) {
  if (!condition) {
    PilatesLogWithConfig(config, PilatesLogLevelFatal, "%s\n", message);
  }
}

void PilatesConfigSetExperimentalFeatureEnabled(const PilatesConfigRef config,
                                           const PilatesExperimentalFeature feature,
                                           const bool enabled) {
  config->experimentalFeatures[feature] = enabled;
}

inline bool PilatesConfigIsExperimentalFeatureEnabled(const PilatesConfigRef config,
                                                 const PilatesExperimentalFeature feature) {
  return config->experimentalFeatures[feature];
}

void PilatesConfigSetUseWebDefaults(const PilatesConfigRef config, const bool enabled) {
  config->useWebDefaults = enabled;
}

void PilatesConfigSetUseLegacyStretchBehaviour(const PilatesConfigRef config,
                                          const bool useLegacyStretchBehaviour) {
  config->useLegacyStretchBehaviour = useLegacyStretchBehaviour;
}

bool PilatesConfigGetUseWebDefaults(const PilatesConfigRef config) {
  return config->useWebDefaults;
}

void PilatesConfigSetContext(const PilatesConfigRef config, void *context) {
  config->context = context;
}

void *PilatesConfigGetContext(const PilatesConfigRef config) {
  return config->context;
}

void PilatesConfigSetNodeClonedFunc(const PilatesConfigRef config, const PilatesNodeClonedFunc callback) {
  config->cloneNodeCallback = callback;
}
