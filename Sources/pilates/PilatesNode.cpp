/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "PilatesNode.h"
#include <iostream>
#include "Utils.h"

void* PilatesNode::getContext() const {
  return context_;
}

PilatesPrintFunc PilatesNode::getPrintFunc() const {
  return print_;
}

bool PilatesNode::getHasNewLayout() const {
  return hasNewLayout_;
}

PilatesNodeType PilatesNode::getNodeType() const {
  return nodeType_;
}

PilatesMeasureFunc PilatesNode::getMeasure() const {
  return measure_;
}

PilatesBaselineFunc PilatesNode::getBaseline() const {
  return baseline_;
}

PilatesDirtiedFunc PilatesNode::getDirtied() const {
  return dirtied_;
}

PilatesStyle& PilatesNode::getStyle() {
  return style_;
}

PilatesLayout& PilatesNode::getLayout() {
  return layout_;
}

uint32_t PilatesNode::getLineIndex() const {
  return lineIndex_;
}

PilatesNodeRef PilatesNode::getParent() const {
  return parent_;
}

PilatesVector PilatesNode::getChildren() const {
  return children_;
}

uint32_t PilatesNode::getChildrenCount() const {
  return static_cast<uint32_t>(children_.size());
}

PilatesNodeRef PilatesNode::getChild(uint32_t index) const {
  return children_.at(index);
}

PilatesNodeRef PilatesNode::getNextChild() const {
  return nextChild_;
}

PilatesConfigRef PilatesNode::getConfig() const {
  return config_;
}

bool PilatesNode::isDirty() const {
  return isDirty_;
}

PilatesValue PilatesNode::getResolvedDimension(int index) {
  return resolvedDimensions_[index];
}

std::array<PilatesValue, 2> PilatesNode::getResolvedDimensions() const {
  return resolvedDimensions_;
}

float PilatesNode::getLeadingPosition(
    const PilatesFlexDirection axis,
    const float axisSize) {
  if (PilatesFlexDirectionIsRow(axis)) {
    const PilatesValue* leadingPosition =
        PilatesComputedEdgeValue(style_.position, PilatesEdgeStart, &PilatesValueUndefined);
    if (leadingPosition->unit != PilatesUnitUndefined) {
      return PilatesResolveValue(*leadingPosition, axisSize);
    }
  }

  const PilatesValue* leadingPosition =
      PilatesComputedEdgeValue(style_.position, leading[axis], &PilatesValueUndefined);

  return leadingPosition->unit == PilatesUnitUndefined
      ? 0.0f
      : PilatesResolveValue(*leadingPosition, axisSize);
}

float PilatesNode::getTrailingPosition(
    const PilatesFlexDirection axis,
    const float axisSize) {
  if (PilatesFlexDirectionIsRow(axis)) {
    const PilatesValue* trailingPosition =
        PilatesComputedEdgeValue(style_.position, PilatesEdgeEnd, &PilatesValueUndefined);
    if (trailingPosition->unit != PilatesUnitUndefined) {
      return PilatesResolveValue(*trailingPosition, axisSize);
    }
  }

  const PilatesValue* trailingPosition =
      PilatesComputedEdgeValue(style_.position, trailing[axis], &PilatesValueUndefined);

  return trailingPosition->unit == PilatesUnitUndefined
      ? 0.0f
      : PilatesResolveValue(*trailingPosition, axisSize);
}

bool PilatesNode::isLeadingPositionDefined(const PilatesFlexDirection axis) {
  return (PilatesFlexDirectionIsRow(axis) &&
          PilatesComputedEdgeValue(style_.position, PilatesEdgeStart, &PilatesValueUndefined)
                  ->unit != PilatesUnitUndefined) ||
      PilatesComputedEdgeValue(style_.position, leading[axis], &PilatesValueUndefined)
          ->unit != PilatesUnitUndefined;
}

bool PilatesNode::isTrailingPosDefined(const PilatesFlexDirection axis) {
  return (PilatesFlexDirectionIsRow(axis) &&
          PilatesComputedEdgeValue(style_.position, PilatesEdgeEnd, &PilatesValueUndefined)
                  ->unit != PilatesUnitUndefined) ||
      PilatesComputedEdgeValue(style_.position, trailing[axis], &PilatesValueUndefined)
          ->unit != PilatesUnitUndefined;
}

float PilatesNode::getLeadingMargin(
    const PilatesFlexDirection axis,
    const float widthSize) {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.margin[PilatesEdgeStart].unit != PilatesUnitUndefined) {
    return PilatesResolveValueMargin(style_.margin[PilatesEdgeStart], widthSize);
  }

  return PilatesResolveValueMargin(
      *PilatesComputedEdgeValue(style_.margin, leading[axis], &PilatesValueZero),
      widthSize);
}

float PilatesNode::getTrailingMargin(
    const PilatesFlexDirection axis,
    const float widthSize) {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.margin[PilatesEdgeEnd].unit != PilatesUnitUndefined) {
    return PilatesResolveValueMargin(style_.margin[PilatesEdgeEnd], widthSize);
  }

  return PilatesResolveValueMargin(
      *PilatesComputedEdgeValue(style_.margin, trailing[axis], &PilatesValueZero),
      widthSize);
}

float PilatesNode::getMarginForAxis(
    const PilatesFlexDirection axis,
    const float widthSize) {
  return getLeadingMargin(axis, widthSize) + getTrailingMargin(axis, widthSize);
}

// Setters

void PilatesNode::setContext(void* context) {
  context_ = context;
}

void PilatesNode::setPrintFunc(PilatesPrintFunc printFunc) {
  print_ = printFunc;
}

void PilatesNode::setHasNewLayout(bool hasNewLayout) {
  hasNewLayout_ = hasNewLayout;
}

void PilatesNode::setNodeType(PilatesNodeType nodeType) {
  nodeType_ = nodeType;
}

void PilatesNode::setStyleFlexDirection(PilatesFlexDirection direction) {
  style_.flexDirection = direction;
}

void PilatesNode::setStyleAlignContent(PilatesAlign alignContent) {
  style_.alignContent = alignContent;
}

void PilatesNode::setMeasureFunc(PilatesMeasureFunc measureFunc) {
  if (measureFunc == nullptr) {
    measure_ = nullptr;
    // TODO: t18095186 Move nodeType to opt-in function and mark appropriate
    // places in Litho
    nodeType_ = PilatesNodeTypeDefault;
  } else {
    PilatesAssertWithNode(
        this,
        children_.size() == 0,
        "Cannot set measure function: Nodes with measure functions cannot have children.");
    measure_ = measureFunc;
    // TODO: t18095186 Move nodeType to opt-in function and mark appropriate
    // places in Litho
    setNodeType(PilatesNodeTypeText);
  }

  measure_ = measureFunc;
}

void PilatesNode::setBaseLineFunc(PilatesBaselineFunc baseLineFunc) {
  baseline_ = baseLineFunc;
}

void PilatesNode::setDirtiedFunc(PilatesDirtiedFunc dirtiedFunc) {
  dirtied_ = dirtiedFunc;
}

void PilatesNode::setStyle(PilatesStyle style) {
  style_ = style;
}

void PilatesNode::setLayout(PilatesLayout layout) {
  layout_ = layout;
}

void PilatesNode::setLineIndex(uint32_t lineIndex) {
  lineIndex_ = lineIndex;
}

void PilatesNode::setParent(PilatesNodeRef parent) {
  parent_ = parent;
}

void PilatesNode::setChildren(PilatesVector children) {
  children_ = children;
}

void PilatesNode::setNextChild(PilatesNodeRef nextChild) {
  nextChild_ = nextChild;
}

void PilatesNode::replaceChild(PilatesNodeRef child, uint32_t index) {
  children_[index] = child;
}

void PilatesNode::replaceChild(PilatesNodeRef oldChild, PilatesNodeRef newChild) {
  std::replace(children_.begin(), children_.end(), oldChild, newChild);
}

void PilatesNode::insertChild(PilatesNodeRef child, uint32_t index) {
  children_.insert(children_.begin() + index, child);
}

void PilatesNode::setConfig(PilatesConfigRef config) {
  config_ = config;
}

void PilatesNode::setDirty(bool isDirty) {
  if (isDirty == isDirty_) {
    return;
  }
  isDirty_ = isDirty;
  if (isDirty && dirtied_) {
    dirtied_(this);
  }
}

bool PilatesNode::removeChild(PilatesNodeRef child) {
  std::vector<PilatesNodeRef>::iterator p =
      std::find(children_.begin(), children_.end(), child);
  if (p != children_.end()) {
    children_.erase(p);
    return true;
  }
  return false;
}

void PilatesNode::removeChild(uint32_t index) {
  children_.erase(children_.begin() + index);
}

void PilatesNode::setLayoutDirection(PilatesDirection direction) {
  layout_.direction = direction;
}

void PilatesNode::setLayoutMargin(float margin, int index) {
  layout_.margin[index] = margin;
}

void PilatesNode::setLayoutBorder(float border, int index) {
  layout_.border[index] = border;
}

void PilatesNode::setLayoutPadding(float padding, int index) {
  layout_.padding[index] = padding;
}

void PilatesNode::setLayoutLastParentDirection(PilatesDirection direction) {
  layout_.lastParentDirection = direction;
}

void PilatesNode::setLayoutComputedFlexBasis(float computedFlexBasis) {
  layout_.computedFlexBasis = computedFlexBasis;
}

void PilatesNode::setLayoutPosition(float position, int index) {
  layout_.position[index] = position;
}

void PilatesNode::setLayoutComputedFlexBasisGeneration(
    uint32_t computedFlexBasisGeneration) {
  layout_.computedFlexBasisGeneration = computedFlexBasisGeneration;
}

void PilatesNode::setLayoutMeasuredDimension(float measuredDimension, int index) {
  layout_.measuredDimensions[index] = measuredDimension;
}

void PilatesNode::setLayoutHadOverflow(bool hadOverflow) {
  layout_.hadOverflow = hadOverflow;
}

void PilatesNode::setLayoutDimension(float dimension, int index) {
  layout_.dimensions[index] = dimension;
}

// If both left and right are defined, then use left. Otherwise return
// +left or -right depending on which is defined.
float PilatesNode::relativePosition(
    const PilatesFlexDirection axis,
    const float axisSize) {
  return isLeadingPositionDefined(axis) ? getLeadingPosition(axis, axisSize)
                                        : -getTrailingPosition(axis, axisSize);
}

void PilatesNode::setPosition(
    const PilatesDirection direction,
    const float mainSize,
    const float crossSize,
    const float parentWidth) {
  /* Root nodes should be always layouted as LTR, so we don't return negative
   * values. */
  const PilatesDirection directionRespectingRoot =
      parent_ != nullptr ? direction : PilatesDirectionLTR;
  const PilatesFlexDirection mainAxis =
      PilatesResolveFlexDirection(style_.flexDirection, directionRespectingRoot);
  const PilatesFlexDirection crossAxis =
      PilatesFlexDirectionCross(mainAxis, directionRespectingRoot);

  const float relativePositionMain = relativePosition(mainAxis, mainSize);
  const float relativePositionCross = relativePosition(crossAxis, crossSize);

  setLayoutPosition(
      getLeadingMargin(mainAxis, parentWidth) + relativePositionMain,
      leading[mainAxis]);
  setLayoutPosition(
      getTrailingMargin(mainAxis, parentWidth) + relativePositionMain,
      trailing[mainAxis]);
  setLayoutPosition(
      getLeadingMargin(crossAxis, parentWidth) + relativePositionCross,
      leading[crossAxis]);
  setLayoutPosition(
      getTrailingMargin(crossAxis, parentWidth) + relativePositionCross,
      trailing[crossAxis]);
}

PilatesNode::PilatesNode()
    : context_(nullptr),
      print_(nullptr),
      hasNewLayout_(true),
      nodeType_(PilatesNodeTypeDefault),
      measure_(nullptr),
      baseline_(nullptr),
      dirtied_(nullptr),
      style_(PilatesStyle()),
      layout_(PilatesLayout()),
      lineIndex_(0),
      parent_(nullptr),
      children_(PilatesVector()),
      nextChild_(nullptr),
      config_(nullptr),
      isDirty_(false),
      resolvedDimensions_({{PilatesValueUndefined, PilatesValueUndefined}}) {}

PilatesNode::PilatesNode(const PilatesNode& node)
    : context_(node.context_),
      print_(node.print_),
      hasNewLayout_(node.hasNewLayout_),
      nodeType_(node.nodeType_),
      measure_(node.measure_),
      baseline_(node.baseline_),
      dirtied_(node.dirtied_),
      style_(node.style_),
      layout_(node.layout_),
      lineIndex_(node.lineIndex_),
      parent_(node.parent_),
      children_(node.children_),
      nextChild_(node.nextChild_),
      config_(node.config_),
      isDirty_(node.isDirty_),
      resolvedDimensions_(node.resolvedDimensions_) {}

PilatesNode::PilatesNode(const PilatesConfigRef newConfig) : PilatesNode() {
  config_ = newConfig;
}

PilatesNode::PilatesNode(
    void* context,
    PilatesPrintFunc print,
    bool hasNewLayout,
    PilatesNodeType nodeType,
    PilatesMeasureFunc measure,
    PilatesBaselineFunc baseline,
    PilatesDirtiedFunc dirtied,
    PilatesStyle style,
    PilatesLayout layout,
    uint32_t lineIndex,
    PilatesNodeRef parent,
    PilatesVector children,
    PilatesNodeRef nextChild,
    PilatesConfigRef config,
    bool isDirty,
    std::array<PilatesValue, 2> resolvedDimensions)
    : context_(context),
      print_(print),
      hasNewLayout_(hasNewLayout),
      nodeType_(nodeType),
      measure_(measure),
      baseline_(baseline),
      dirtied_(dirtied),
      style_(style),
      layout_(layout),
      lineIndex_(lineIndex),
      parent_(parent),
      children_(children),
      nextChild_(nextChild),
      config_(config),
      isDirty_(isDirty),
      resolvedDimensions_(resolvedDimensions) {}

PilatesNode& PilatesNode::operator=(const PilatesNode& node) {
  if (&node == this) {
    return *this;
  }

  for (auto child : children_) {
    delete child;
  }

  context_ = node.getContext();
  print_ = node.getPrintFunc();
  hasNewLayout_ = node.getHasNewLayout();
  nodeType_ = node.getNodeType();
  measure_ = node.getMeasure();
  baseline_ = node.getBaseline();
  dirtied_ = node.getDirtied();
  style_ = node.style_;
  layout_ = node.layout_;
  lineIndex_ = node.getLineIndex();
  parent_ = node.getParent();
  children_ = node.getChildren();
  nextChild_ = node.getNextChild();
  config_ = node.getConfig();
  isDirty_ = node.isDirty();
  resolvedDimensions_ = node.getResolvedDimensions();

  return *this;
}

PilatesValue PilatesNode::marginLeadingValue(const PilatesFlexDirection axis) const {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.margin[PilatesEdgeStart].unit != PilatesUnitUndefined) {
    return style_.margin[PilatesEdgeStart];
  } else {
    return style_.margin[leading[axis]];
  }
}

PilatesValue PilatesNode::marginTrailingValue(const PilatesFlexDirection axis) const {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.margin[PilatesEdgeEnd].unit != PilatesUnitUndefined) {
    return style_.margin[PilatesEdgeEnd];
  } else {
    return style_.margin[trailing[axis]];
  }
}

PilatesValue PilatesNode::resolveFlexBasisPtr() const {
  PilatesValue flexBasis = style_.flexBasis;
  if (flexBasis.unit != PilatesUnitAuto && flexBasis.unit != PilatesUnitUndefined) {
    return flexBasis;
  }
  if (!PilatesFloatIsUndefined(style_.flex) && style_.flex > 0.0f) {
    return config_->useWebDefaults ? PilatesValueAuto : PilatesValueZero;
  }
  return PilatesValueAuto;
}

void PilatesNode::resolveDimension() {
  for (uint32_t dim = PilatesDimensionWidth; dim < PilatesDimensionCount; dim++) {
    if (getStyle().maxDimensions[dim].unit != PilatesUnitUndefined &&
        PilatesValueEqual(
            getStyle().maxDimensions[dim], style_.minDimensions[dim])) {
      resolvedDimensions_[dim] = style_.maxDimensions[dim];
    } else {
      resolvedDimensions_[dim] = style_.dimensions[dim];
    }
  }
}

PilatesDirection PilatesNode::resolveDirection(const PilatesDirection parentDirection) {
  if (style_.direction == PilatesDirectionInherit) {
    return parentDirection > PilatesDirectionInherit ? parentDirection
                                                : PilatesDirectionLTR;
  } else {
    return style_.direction;
  }
}

void PilatesNode::clearChildren() {
  children_.clear();
  children_.shrink_to_fit();
}

PilatesNode::~PilatesNode() {
  // All the member variables are deallocated externally, so no need to
  // deallocate here
}

// Other Methods

void PilatesNode::cloneChildrenIfNeeded() {
  // PilatesNodeRemoveChild in pilates.cpp has a forked variant of this algorithm
  // optimized for deletions.

  const uint32_t childCount = static_cast<uint32_t>(children_.size());
  if (childCount == 0) {
    // This is an empty set. Nothing to clone.
    return;
  }

  const PilatesNodeRef firstChild = children_.front();
  if (firstChild->getParent() == this) {
    // If the first child has this node as its parent, we assume that it is
    // already unique. We can do this because if we have it has a child, that
    // means that its parent was at some point cloned which made that subtree
    // immutable. We also assume that all its sibling are cloned as well.
    return;
  }

  const PilatesNodeClonedFunc cloneNodeCallback = config_->cloneNodeCallback;
  for (uint32_t i = 0; i < childCount; ++i) {
    const PilatesNodeRef oldChild = children_[i];
    const PilatesNodeRef newChild = PilatesNodeClone(oldChild);
    replaceChild(newChild, i);
    newChild->setParent(this);
    if (cloneNodeCallback) {
      cloneNodeCallback(oldChild, newChild, this, i);
    }
  }
}

void PilatesNode::markDirtyAndPropogate() {
  if (!isDirty_) {
    setDirty(true);
    setLayoutComputedFlexBasis(PilatesUndefined);
    if (parent_) {
      parent_->markDirtyAndPropogate();
    }
  }
}

void PilatesNode::markDirtyAndPropogateDownwards() {
  isDirty_ = true;
  for_each(children_.begin(), children_.end(), [](PilatesNodeRef childNode) {
    childNode->markDirtyAndPropogateDownwards();
  });
}

float PilatesNode::resolveFlexGrow() {
  // Root nodes flexGrow should always be 0
  if (parent_ == nullptr) {
    return 0.0;
  }
  if (!PilatesFloatIsUndefined(style_.flexGrow)) {
    return style_.flexGrow;
  }
  if (!PilatesFloatIsUndefined(style_.flex) && style_.flex > 0.0f) {
    return style_.flex;
  }
  return kDefaultFlexGrow;
}

float PilatesNode::resolveFlexShrink() {
  if (parent_ == nullptr) {
    return 0.0;
  }
  if (!PilatesFloatIsUndefined(style_.flexShrink)) {
    return style_.flexShrink;
  }
  if (!config_->useWebDefaults && !PilatesFloatIsUndefined(style_.flex) &&
      style_.flex < 0.0f) {
    return -style_.flex;
  }
  return config_->useWebDefaults ? kWebDefaultFlexShrink : kDefaultFlexShrink;
}

bool PilatesNode::isNodeFlexible() {
  return (
      (style_.positionType == PilatesPositionTypeRelative) &&
      (resolveFlexGrow() != 0 || resolveFlexShrink() != 0));
}

float PilatesNode::getLeadingBorder(const PilatesFlexDirection axis) {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.border[PilatesEdgeStart].unit != PilatesUnitUndefined &&
      style_.border[PilatesEdgeStart].value >= 0.0f) {
    return style_.border[PilatesEdgeStart].value;
  }

  return fmaxf(
      PilatesComputedEdgeValue(style_.border, leading[axis], &PilatesValueZero)->value,
      0.0f);
}

float PilatesNode::getTrailingBorder(const PilatesFlexDirection flexDirection) {
  if (PilatesFlexDirectionIsRow(flexDirection) &&
      style_.border[PilatesEdgeEnd].unit != PilatesUnitUndefined &&
      style_.border[PilatesEdgeEnd].value >= 0.0f) {
    return style_.border[PilatesEdgeEnd].value;
  }

  return fmaxf(
      PilatesComputedEdgeValue(style_.border, trailing[flexDirection], &PilatesValueZero)
          ->value,
      0.0f);
}

float PilatesNode::getLeadingPadding(
    const PilatesFlexDirection axis,
    const float widthSize) {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.padding[PilatesEdgeStart].unit != PilatesUnitUndefined &&
      PilatesResolveValue(style_.padding[PilatesEdgeStart], widthSize) >= 0.0f) {
    return PilatesResolveValue(style_.padding[PilatesEdgeStart], widthSize);
  }
  return fmaxf(
      PilatesResolveValue(
          *PilatesComputedEdgeValue(style_.padding, leading[axis], &PilatesValueZero),
          widthSize),
      0.0f);
}

float PilatesNode::getTrailingPadding(
    const PilatesFlexDirection axis,
    const float widthSize) {
  if (PilatesFlexDirectionIsRow(axis) &&
      style_.padding[PilatesEdgeEnd].unit != PilatesUnitUndefined &&
      PilatesResolveValue(style_.padding[PilatesEdgeEnd], widthSize) >= 0.0f) {
    return PilatesResolveValue(style_.padding[PilatesEdgeEnd], widthSize);
  }
  return fmaxf(
      PilatesResolveValue(
          *PilatesComputedEdgeValue(style_.padding, trailing[axis], &PilatesValueZero),
          widthSize),
      0.0f);
}

float PilatesNode::getLeadingPaddingAndBorder(
    const PilatesFlexDirection axis,
    const float widthSize) {
  return getLeadingPadding(axis, widthSize) + getLeadingBorder(axis);
}

float PilatesNode::getTrailingPaddingAndBorder(
    const PilatesFlexDirection axis,
    const float widthSize) {
  return getTrailingPadding(axis, widthSize) + getTrailingBorder(axis);
}

bool PilatesNode::didUseLegacyFlag() {
  bool didUseLegacyFlag = layout_.didUseLegacyFlag;
  if (didUseLegacyFlag) {
    return true;
  }
  for (const auto& child : children_) {
    if (child->layout_.didUseLegacyFlag) {
      didUseLegacyFlag = true;
      break;
    }
  }
  return didUseLegacyFlag;
}

void PilatesNode::setAndPropogateUseLegacyFlag(bool useLegacyFlag) {
  config_->useLegacyStretchBehaviour = useLegacyFlag;
  for_each(children_.begin(), children_.end(), [=](PilatesNodeRef childNode) {
    childNode->getConfig()->useLegacyStretchBehaviour = useLegacyFlag;
  });
}

void PilatesNode::setLayoutDoesLegacyFlagAffectsLayout(
    bool doesLegacyFlagAffectsLayout) {
  layout_.doesLegacyStretchFlagAffectsLayout = doesLegacyFlagAffectsLayout;
}

void PilatesNode::setLayoutDidUseLegacyFlag(bool didUseLegacyFlag) {
  layout_.didUseLegacyFlag = didUseLegacyFlag;
}

bool PilatesNode::isLayoutTreeEqualToNode(const PilatesNode& node) const {
  if (children_.size() != node.children_.size()) {
    return false;
  }
  if (layout_ != node.layout_) {
    return false;
  }
  if (children_.size() == 0) {
    return true;
  }

  bool isLayoutTreeEqual = true;
  PilatesNodeRef otherNodeChildren = nullptr;
  for (std::vector<PilatesNodeRef>::size_type i = 0; i < children_.size(); ++i) {
    otherNodeChildren = node.children_[i];
    isLayoutTreeEqual =
        children_[i]->isLayoutTreeEqualToNode(*otherNodeChildren);
    if (!isLayoutTreeEqual) {
      return false;
    }
  }
  return isLayoutTreeEqual;
}
