/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <stdio.h>
#include "PilatesLayout.h"
#include "PilatesStyle.h"
#include "Pilates-internal.h"

struct PilatesNode {
 private:
  void* context_;
  PilatesPrintFunc print_;
  bool hasNewLayout_;
  PilatesNodeType nodeType_;
  PilatesMeasureFunc measure_;
  PilatesBaselineFunc baseline_;
  PilatesDirtiedFunc dirtied_;
  PilatesStyle style_;
  PilatesLayout layout_;
  uint32_t lineIndex_;
  PilatesNodeRef parent_;
  PilatesVector children_;
  PilatesNodeRef nextChild_;
  PilatesConfigRef config_;
  bool isDirty_;
  std::array<PilatesValue, 2> resolvedDimensions_;

  float relativePosition(const PilatesFlexDirection axis, const float axisSize);

 public:
  PilatesNode();
  ~PilatesNode();
  explicit PilatesNode(const PilatesConfigRef newConfig);
  PilatesNode(const PilatesNode& node);
  PilatesNode& operator=(const PilatesNode& node);
  PilatesNode(
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
      std::array<PilatesValue, 2> resolvedDimensions);

  // Getters
  void* getContext() const;
  PilatesPrintFunc getPrintFunc() const;
  bool getHasNewLayout() const;
  PilatesNodeType getNodeType() const;
  PilatesMeasureFunc getMeasure() const;
  PilatesBaselineFunc getBaseline() const;
  PilatesDirtiedFunc getDirtied() const;
  // For Performance reasons passing as reference.
  PilatesStyle& getStyle();
  // For Performance reasons passing as reference.
  PilatesLayout& getLayout();
  uint32_t getLineIndex() const;
  PilatesNodeRef getParent() const;
  PilatesVector getChildren() const;
  uint32_t getChildrenCount() const;
  PilatesNodeRef getChild(uint32_t index) const;
  PilatesNodeRef getNextChild() const;
  PilatesConfigRef getConfig() const;
  bool isDirty() const;
  std::array<PilatesValue, 2> getResolvedDimensions() const;
  PilatesValue getResolvedDimension(int index);

  // Methods related to positions, margin, padding and border
  float getLeadingPosition(const PilatesFlexDirection axis, const float axisSize);
  bool isLeadingPositionDefined(const PilatesFlexDirection axis);
  bool isTrailingPosDefined(const PilatesFlexDirection axis);
  float getTrailingPosition(const PilatesFlexDirection axis, const float axisSize);
  float getLeadingMargin(const PilatesFlexDirection axis, const float widthSize);
  float getTrailingMargin(const PilatesFlexDirection axis, const float widthSize);
  float getLeadingBorder(const PilatesFlexDirection flexDirection);
  float getTrailingBorder(const PilatesFlexDirection flexDirection);
  float getLeadingPadding(const PilatesFlexDirection axis, const float widthSize);
  float getTrailingPadding(const PilatesFlexDirection axis, const float widthSize);
  float getLeadingPaddingAndBorder(
      const PilatesFlexDirection axis,
      const float widthSize);
  float getTrailingPaddingAndBorder(
      const PilatesFlexDirection axis,
      const float widthSize);
  float getMarginForAxis(const PilatesFlexDirection axis, const float widthSize);
  // Setters

  void setContext(void* context);
  void setPrintFunc(PilatesPrintFunc printFunc);
  void setHasNewLayout(bool hasNewLayout);
  void setNodeType(PilatesNodeType nodeTye);
  void setMeasureFunc(PilatesMeasureFunc measureFunc);
  void setBaseLineFunc(PilatesBaselineFunc baseLineFunc);
  void setDirtiedFunc(PilatesDirtiedFunc dirtiedFunc);
  void setStyle(PilatesStyle style);
  void setStyleFlexDirection(PilatesFlexDirection direction);
  void setStyleAlignContent(PilatesAlign alignContent);
  void setLayout(PilatesLayout layout);
  void setLineIndex(uint32_t lineIndex);
  void setParent(PilatesNodeRef parent);
  void setChildren(PilatesVector children);
  void setNextChild(PilatesNodeRef nextChild);
  void setConfig(PilatesConfigRef config);
  void setDirty(bool isDirty);
  void setLayoutLastParentDirection(PilatesDirection direction);
  void setLayoutComputedFlexBasis(float computedFlexBasis);
  void setLayoutComputedFlexBasisGeneration(
      uint32_t computedFlexBasisGeneration);
  void setLayoutMeasuredDimension(float measuredDimension, int index);
  void setLayoutHadOverflow(bool hadOverflow);
  void setLayoutDimension(float dimension, int index);
  void setLayoutDirection(PilatesDirection direction);
  void setLayoutMargin(float margin, int index);
  void setLayoutBorder(float border, int index);
  void setLayoutPadding(float padding, int index);
  void setLayoutPosition(float position, int index);
  void setPosition(
      const PilatesDirection direction,
      const float mainSize,
      const float crossSize,
      const float parentWidth);
  void setAndPropogateUseLegacyFlag(bool useLegacyFlag);
  void setLayoutDoesLegacyFlagAffectsLayout(bool doesLegacyFlagAffectsLayout);
  void setLayoutDidUseLegacyFlag(bool didUseLegacyFlag);
  void markDirtyAndPropogateDownwards();

  // Other methods
  PilatesValue marginLeadingValue(const PilatesFlexDirection axis) const;
  PilatesValue marginTrailingValue(const PilatesFlexDirection axis) const;
  PilatesValue resolveFlexBasisPtr() const;
  void resolveDimension();
  PilatesDirection resolveDirection(const PilatesDirection parentDirection);
  void clearChildren();
  /// Replaces the occurrences of oldChild with newChild
  void replaceChild(PilatesNodeRef oldChild, PilatesNodeRef newChild);
  void replaceChild(PilatesNodeRef child, uint32_t index);
  void insertChild(PilatesNodeRef child, uint32_t index);
  /// Removes the first occurrence of child
  bool removeChild(PilatesNodeRef child);
  void removeChild(uint32_t index);

  void cloneChildrenIfNeeded();
  void markDirtyAndPropogate();
  float resolveFlexGrow();
  float resolveFlexShrink();
  bool isNodeFlexible();
  bool didUseLegacyFlag();
  bool isLayoutTreeEqualToNode(const PilatesNode& node) const;
};
