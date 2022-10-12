/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "PilatesLayout+Private.h"
#import "UIView+Pilates.h"

#define Pilates_PROPERTY(type, lowercased_name, capitalized_name)    \
- (type)lowercased_name                                         \
{                                                               \
  return PilatesNodeStyleGet##capitalized_name(self.node);           \
}                                                               \
                                                                \
- (void)set##capitalized_name:(type)lowercased_name             \
{                                                               \
  PilatesNodeStyleSet##capitalized_name(self.node, lowercased_name); \
}

#define Pilates_VALUE_PROPERTY(lowercased_name, capitalized_name)                       \
- (PilatesValue)lowercased_name                                                         \
{                                                                                  \
  return PilatesNodeStyleGet##capitalized_name(self.node);                              \
}                                                                                  \
                                                                                   \
- (void)set##capitalized_name:(PilatesValue)lowercased_name                             \
{                                                                                  \
  switch (lowercased_name.unit) {                                                  \
     case PilatesUnitUndefined:                                                         \
      PilatesNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case PilatesUnitPoint:                                                              \
      PilatesNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case PilatesUnitPercent:                                                            \
      PilatesNodeStyleSet##capitalized_name##Percent(self.node, lowercased_name.value); \
      break;                                                                       \
    default:                                                                       \
      NSAssert(NO, @"Not implemented");                                            \
  }                                                                                \
}

#define Pilates_AUTO_VALUE_PROPERTY(lowercased_name, capitalized_name)                  \
- (PilatesValue)lowercased_name                                                         \
{                                                                                  \
  return PilatesNodeStyleGet##capitalized_name(self.node);                              \
}                                                                                  \
                                                                                   \
- (void)set##capitalized_name:(PilatesValue)lowercased_name                             \
{                                                                                  \
  switch (lowercased_name.unit) {                                                  \
    case PilatesUnitPoint:                                                              \
      PilatesNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case PilatesUnitPercent:                                                            \
      PilatesNodeStyleSet##capitalized_name##Percent(self.node, lowercased_name.value); \
      break;                                                                       \
    case PilatesUnitAuto:                                                               \
      PilatesNodeStyleSet##capitalized_name##Auto(self.node);                           \
      break;                                                                       \
    default:                                                                       \
      NSAssert(NO, @"Not implemented");                                            \
  }                                                                                \
}

#define Pilates_EDGE_PROPERTY_GETTER(type, lowercased_name, capitalized_name, property, edge) \
- (type)lowercased_name                                                                  \
{                                                                                        \
  return PilatesNodeStyleGet##property(self.node, edge);                                      \
}

#define Pilates_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge) \
- (void)set##capitalized_name:(CGFloat)lowercased_name                             \
{                                                                                  \
  PilatesNodeStyleSet##property(self.node, edge, lowercased_name);                      \
}

#define Pilates_EDGE_PROPERTY(lowercased_name, capitalized_name, property, edge)         \
Pilates_EDGE_PROPERTY_GETTER(CGFloat, lowercased_name, capitalized_name, property, edge) \
Pilates_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge)

#define Pilates_VALUE_EDGE_PROPERTY_SETTER(objc_lowercased_name, objc_capitalized_name, c_name, edge) \
- (void)set##objc_capitalized_name:(PilatesValue)objc_lowercased_name                                 \
{                                                                                                \
  switch (objc_lowercased_name.unit) {                                                           \
    case PilatesUnitUndefined:                                                                        \
      PilatesNodeStyleSet##c_name(self.node, edge, objc_lowercased_name.value);                       \
      break;                                                                                     \
    case PilatesUnitPoint:                                                                            \
      PilatesNodeStyleSet##c_name(self.node, edge, objc_lowercased_name.value);                       \
      break;                                                                                     \
    case PilatesUnitPercent:                                                                          \
      PilatesNodeStyleSet##c_name##Percent(self.node, edge, objc_lowercased_name.value);              \
      break;                                                                                     \
    default:                                                                                     \
      NSAssert(NO, @"Not implemented");                                                          \
  }                                                                                              \
}

#define Pilates_VALUE_EDGE_PROPERTY(lowercased_name, capitalized_name, property, edge)   \
Pilates_EDGE_PROPERTY_GETTER(PilatesValue, lowercased_name, capitalized_name, property, edge) \
Pilates_VALUE_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge)

#define Pilates_VALUE_EDGES_PROPERTIES(lowercased_name, capitalized_name)                                                  \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Left, capitalized_name##Left, capitalized_name, PilatesEdgeLeft)                   \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Top, capitalized_name##Top, capitalized_name, PilatesEdgeTop)                      \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Right, capitalized_name##Right, capitalized_name, PilatesEdgeRight)                \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Bottom, capitalized_name##Bottom, capitalized_name, PilatesEdgeBottom)             \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Start, capitalized_name##Start, capitalized_name, PilatesEdgeStart)                \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##End, capitalized_name##End, capitalized_name, PilatesEdgeEnd)                      \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Horizontal, capitalized_name##Horizontal, capitalized_name, PilatesEdgeHorizontal) \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name##Vertical, capitalized_name##Vertical, capitalized_name, PilatesEdgeVertical)       \
Pilates_VALUE_EDGE_PROPERTY(lowercased_name, capitalized_name, capitalized_name, PilatesEdgeAll)

PilatesValue PilatesPointValue(CGFloat value)
{
  return (PilatesValue) { .value = (float) value, .unit = (PilatesUnit) PilatesUnitPoint };
}

PilatesValue PilatesPercentValue(CGFloat value)
{
  return (PilatesValue) { .value = (float) value, .unit = PilatesUnitPercent };
}

static PilatesConfigRef globalConfig;

@interface PilatesLayout ()

@property (nonatomic, weak, readonly) UIView *view;

@end

@implementation PilatesLayout

@synthesize isEnabled=_isEnabled;
@synthesize isIncludedInLayout=_isIncludedInLayout;
@synthesize node=_node;

+ (void)initialize
{
  globalConfig = PilatesConfigNew();
  PilatesConfigSetExperimentalFeatureEnabled(globalConfig, PilatesExperimentalFeatureWebFlexBasis, true);
  PilatesConfigSetPointScaleFactor(globalConfig, [UIScreen mainScreen].scale);
}

- (instancetype)initWithView:(UIView*)view
{
  if (self = [super init]) {
    _view = view;
    _node = PilatesNodeNewWithConfig(globalConfig);
    PilatesNodeSetContext(_node, (__bridge void *) view);
    _isEnabled = NO;
    _isIncludedInLayout = YES;

    if ([view isKindOfClass:[UILabel class]]) {
      if (PilatesNodeGetBaselineFunc(_node) == NULL) {
        PilatesNodeSetBaselineFunc(_node, PilatesMeasureBaselineLabel);
      }
    }

    if ([view isKindOfClass:[UITextView class]]) {
      if (PilatesNodeGetBaselineFunc(_node) == NULL) {
        PilatesNodeSetBaselineFunc(_node, PilatesMeasureBaselineTextView);
      }
    }

    if ([view isKindOfClass:[UITextField class]]) {
      if (PilatesNodeGetBaselineFunc(_node) == NULL) {
        PilatesNodeSetBaselineFunc(_node, PilatesMeasureBaselineTextField);
      }
    }
  }

  return self;
}

- (void)dealloc
{
  PilatesNodeFree(self.node);
}

- (BOOL)isDirty
{
  return PilatesNodeIsDirty(self.node);
}

- (void)markDirty
{
  if (self.isDirty || !self.isLeaf) {
    return;
  }

  // Pilates is not happy if we try to mark a node as "dirty" before we have set
  // the measure function. Since we already know that this is a leaf,
  // this *should* be fine. Forgive me Hack Gods.
  const PilatesNodeRef node = self.node;
  if (PilatesNodeGetMeasureFunc(node) == NULL) {
    PilatesNodeSetMeasureFunc(node, PilatesMeasureView);
  }

  PilatesNodeMarkDirty(node);
}

- (NSUInteger)numberOfChildren
{
  return PilatesNodeGetChildCount(self.node);
}

- (BOOL)isLeaf
{
  NSAssert([NSThread isMainThread], @"This method must be called on the main thread.");
  if (self.isEnabled) {
    for (UIView *subview in self.view.subviews) {
      PilatesLayout *const pilates = subview.pilates;
      if (pilates.isEnabled && pilates.isIncludedInLayout) {
        return NO;
      }
    }
  }

  return YES;
}

#pragma mark - Style

- (PilatesPositionType)position
{
  return PilatesNodeStyleGetPositionType(self.node);
}

- (void)setPosition:(PilatesPositionType)position
{
  PilatesNodeStyleSetPositionType(self.node, position);
}

Pilates_PROPERTY(PilatesDirection, direction, Direction)
Pilates_PROPERTY(PilatesFlexDirection, flexDirection, FlexDirection)
Pilates_PROPERTY(PilatesJustify, justifyContent, JustifyContent)
Pilates_PROPERTY(PilatesAlign, alignContent, AlignContent)
Pilates_PROPERTY(PilatesAlign, alignItems, AlignItems)
Pilates_PROPERTY(PilatesAlign, alignSelf, AlignSelf)
Pilates_PROPERTY(PilatesWrap, flexWrap, FlexWrap)
Pilates_PROPERTY(PilatesOverflow, overflow, Overflow)
Pilates_PROPERTY(PilatesDisplay, display, Display)

Pilates_PROPERTY(CGFloat, flexGrow, FlexGrow)
Pilates_PROPERTY(CGFloat, flexShrink, FlexShrink)
Pilates_AUTO_VALUE_PROPERTY(flexBasis, FlexBasis)

Pilates_VALUE_EDGE_PROPERTY(left, Left, Position, PilatesEdgeLeft)
Pilates_VALUE_EDGE_PROPERTY(top, Top, Position, PilatesEdgeTop)
Pilates_VALUE_EDGE_PROPERTY(right, Right, Position, PilatesEdgeRight)
Pilates_VALUE_EDGE_PROPERTY(bottom, Bottom, Position, PilatesEdgeBottom)
Pilates_VALUE_EDGE_PROPERTY(start, Start, Position, PilatesEdgeStart)
Pilates_VALUE_EDGE_PROPERTY(end, End, Position, PilatesEdgeEnd)
Pilates_VALUE_EDGES_PROPERTIES(margin, Margin)
Pilates_VALUE_EDGES_PROPERTIES(padding, Padding)

Pilates_EDGE_PROPERTY(borderLeftWidth, BorderLeftWidth, Border, PilatesEdgeLeft)
Pilates_EDGE_PROPERTY(borderTopWidth, BorderTopWidth, Border, PilatesEdgeTop)
Pilates_EDGE_PROPERTY(borderRightWidth, BorderRightWidth, Border, PilatesEdgeRight)
Pilates_EDGE_PROPERTY(borderBottomWidth, BorderBottomWidth, Border, PilatesEdgeBottom)
Pilates_EDGE_PROPERTY(borderStartWidth, BorderStartWidth, Border, PilatesEdgeStart)
Pilates_EDGE_PROPERTY(borderEndWidth, BorderEndWidth, Border, PilatesEdgeEnd)
Pilates_EDGE_PROPERTY(borderWidth, BorderWidth, Border, PilatesEdgeAll)

Pilates_AUTO_VALUE_PROPERTY(width, Width)
Pilates_AUTO_VALUE_PROPERTY(height, Height)
Pilates_VALUE_PROPERTY(minWidth, MinWidth)
Pilates_VALUE_PROPERTY(minHeight, MinHeight)
Pilates_VALUE_PROPERTY(maxWidth, MaxWidth)
Pilates_VALUE_PROPERTY(maxHeight, MaxHeight)
Pilates_PROPERTY(CGFloat, aspectRatio, AspectRatio)

#pragma mark - Layout and Sizing

- (PilatesDirection)resolvedDirection
{
  return PilatesNodeLayoutGetDirection(self.node);
}

- (void)applyLayout
{
  [self calculateLayoutWithSize:self.view.bounds.size];
  PilatesApplyLayoutToViewHierarchy(self.view, NO);
}

- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
{
  [self calculateLayoutWithSize:self.view.bounds.size];
  PilatesApplyLayoutToViewHierarchy(self.view, preserveOrigin);
}

- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin dimensionFlexibility:(PilatesDimensionFlexibility)dimensionFlexibility
{
  CGSize size = self.view.bounds.size;
  if (dimensionFlexibility & PilatesDimensionFlexibilityFlexibleWidth) {
    size.width = PilatesUndefined;
  }
  if (dimensionFlexibility & PilatesDimensionFlexibilityFlexibleHeight) {
    size.height = PilatesUndefined;
  }
  [self calculateLayoutWithSize:size];
  PilatesApplyLayoutToViewHierarchy(self.view, preserveOrigin);
}


- (CGSize)intrinsicSize
{
  const CGSize constrainedSize = {
    .width = PilatesUndefined,
    .height = PilatesUndefined,
  };
  return [self calculateLayoutWithSize:constrainedSize];
}

- (CGSize)calculateLayoutWithSize:(CGSize)size
{
  NSAssert([NSThread isMainThread], @"Pilates calculation must be done on main.");
  NSAssert(self.isEnabled, @"Pilates is not enabled for this view.");

  PilatesAttachNodesFromViewHierachy(self.view);

  const PilatesNodeRef node = self.node;
  PilatesNodeCalculateLayout(
    node,
    size.width,
    size.height,
    PilatesNodeStyleGetDirection(node));

  return (CGSize) {
    .width = PilatesNodeLayoutGetWidth(node),
    .height = PilatesNodeLayoutGetHeight(node),
  };
}

#pragma mark - Private

static float PilatesMeasureBaselineLabel(
  PilatesNodeRef node,
  const float width,
  const float height) {

  UILabel* view = (__bridge UILabel*) PilatesNodeGetContext(node);
  return view.font.ascender; // height + view.font.ascender for lastBaseline
}

static float PilatesMeasureBaselineTextView(
  PilatesNodeRef node,
  const float width,
  const float height) {

  UITextView* view = (__bridge UITextView*) PilatesNodeGetContext(node);
  return view.font.ascender + view.contentInset.top + view.textContainerInset.top;
}

static float PilatesMeasureBaselineTextField(
  PilatesNodeRef node,
  const float width,
  const float height) {

  UITextField* view = (__bridge UITextField*) PilatesNodeGetContext(node);

  switch (view.borderStyle) {
  case UITextBorderStyleNone:
    return view.font.ascender;
  case UITextBorderStyleLine:
    return view.font.ascender + 4;
  case UITextBorderStyleBezel:
  case UITextBorderStyleRoundedRect:
    return view.font.ascender + 7;
  }
}

static PilatesSize PilatesMeasureView(
  PilatesNodeRef node,
  float width,
  PilatesMeasureMode widthMode,
  float height,
  PilatesMeasureMode heightMode)
{
  const CGFloat constrainedWidth = (widthMode == PilatesMeasureModeUndefined) ? CGFLOAT_MAX : width;
  const CGFloat constrainedHeight = (heightMode == PilatesMeasureModeUndefined) ? CGFLOAT_MAX: height;

  UIView *view = (__bridge UIView*) PilatesNodeGetContext(node);
  const CGSize sizeThatFits = [view sizeThatFits:(CGSize) {
    .width = constrainedWidth,
    .height = constrainedHeight,
  }];

  return (PilatesSize) {
    .width = (float) PilatesSanitizeMeasurement(constrainedWidth, sizeThatFits.width, widthMode),
    .height = (float) PilatesSanitizeMeasurement(constrainedHeight, sizeThatFits.height, heightMode),
  };
}

static CGFloat PilatesSanitizeMeasurement(
  CGFloat constrainedSize,
  CGFloat measuredSize,
  PilatesMeasureMode measureMode)
{
  CGFloat result;
  if (measureMode == PilatesMeasureModeExactly) {
    result = constrainedSize;
  } else if (measureMode == PilatesMeasureModeAtMost) {
    result = MIN(constrainedSize, measuredSize);
  } else {
    result = measuredSize;
  }

  return result;
}

static BOOL PilatesNodeHasExactSameChildren(const PilatesNodeRef node, NSArray<UIView *> *subviews)
{
  if (PilatesNodeGetChildCount(node) != subviews.count) {
    return NO;
  }

  for (int i=0; i<subviews.count; i++) {
    if (PilatesNodeGetChild(node, i) != subviews[i].pilates.node) {
      return NO;
    }
  }

  return YES;
}

static void PilatesAttachNodesFromViewHierachy(UIView *const view)
{
  PilatesLayout *const pilates = view.pilates;
  const PilatesNodeRef node = pilates.node;

  // Only leaf nodes should have a measure function
  if (pilates.isLeaf) {
    PilatesRemoveAllChildren(node);
    PilatesNodeSetMeasureFunc(node, PilatesMeasureView);
  } else {
    PilatesNodeSetMeasureFunc(node, NULL);

    NSMutableArray<UIView *> *subviewsToInclude = [[NSMutableArray alloc] initWithCapacity:view.subviews.count];
    for (UIView *subview in view.subviews) {
      if (subview.pilates.isIncludedInLayout) {
        [subviewsToInclude addObject:subview];
      }
    }

    if (!PilatesNodeHasExactSameChildren(node, subviewsToInclude)) {
      PilatesRemoveAllChildren(node);
      for (int i=0; i<subviewsToInclude.count; i++) {
        PilatesNodeRef child = subviewsToInclude[i].pilates.node;
        PilatesNodeRef parent = PilatesNodeGetParent(child);
        if (parent != NULL) {
          PilatesNodeRemoveChild(parent, child);
        }
        PilatesNodeInsertChild(node, child, i);
      }
    }

    for (UIView *const subview in subviewsToInclude) {
      PilatesAttachNodesFromViewHierachy(subview);
    }
  }
}

static void PilatesRemoveAllChildren(const PilatesNodeRef node)
{
  if (node == NULL) {
    return;
  }

  while (PilatesNodeGetChildCount(node) > 0) {
    PilatesNodeRemoveChild(node, PilatesNodeGetChild(node, PilatesNodeGetChildCount(node) - 1));
  }
}

static CGFloat PilatesRoundPixelValue(CGFloat value)
{
  static CGFloat scale;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^(){
    scale = [UIScreen mainScreen].scale;
  });

  return roundf(value * scale) / scale;
}

static void PilatesApplyLayoutToViewHierarchy(UIView *view, BOOL preserveOrigin)
{
  NSCAssert([NSThread isMainThread], @"Framesetting should only be done on the main thread.");

  const PilatesLayout *pilates = view.pilates;

  if (!pilates.isIncludedInLayout) {
     return;
  }

  PilatesNodeRef node = pilates.node;
  const CGPoint topLeft = {
    PilatesNodeLayoutGetLeft(node),
    PilatesNodeLayoutGetTop(node),
  };

  const CGPoint bottomRight = {
    topLeft.x + PilatesNodeLayoutGetWidth(node),
    topLeft.y + PilatesNodeLayoutGetHeight(node),
  };

  const CGPoint origin = preserveOrigin ? view.frame.origin : CGPointZero;
  view.frame = (CGRect) {
    .origin = {
      .x = PilatesRoundPixelValue(topLeft.x + origin.x),
      .y = PilatesRoundPixelValue(topLeft.y + origin.y),
    },
    .size = {
      .width = PilatesRoundPixelValue(bottomRight.x) - PilatesRoundPixelValue(topLeft.x),
      .height = PilatesRoundPixelValue(bottomRight.y) - PilatesRoundPixelValue(topLeft.y),
    },
  };

  if (!pilates.isLeaf) {
    for (NSUInteger i=0; i<view.subviews.count; i++) {
      PilatesApplyLayoutToViewHierarchy(view.subviews[i], NO);
    }
  }
}

@end
