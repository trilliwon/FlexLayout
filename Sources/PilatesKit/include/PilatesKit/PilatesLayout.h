/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import <UIKit/UIKit.h>

#if FLEXLAYOUT_SWIFT_PACKAGE
#import <pilates/PilatesEnums.h>
#import <pilates/Pilates.h>
#import <pilates/PilatesMacros.h>
#else
#import "PilatesEnums.h"
#import "Pilates.h"
#import "PilatesMacros.h"
#endif

Pilates_EXTERN_C_BEGIN

extern PilatesValue PilatesPointValue(CGFloat value)
    NS_SWIFT_UNAVAILABLE("Use the swift Int and FloatingPoint extensions instead");
extern PilatesValue PilatesPercentValue(CGFloat value)
    NS_SWIFT_UNAVAILABLE("Use the swift Int and FloatingPoint extensions instead");

Pilates_EXTERN_C_END

typedef NS_OPTIONS(NSInteger, PilatesDimensionFlexibility) {
  PilatesDimensionFlexibilityFlexibleWidth = 1 << 0,
  PilatesDimensionFlexibilityFlexibleHeight = 1 << 1,
};

@interface PilatesLayout : NSObject

/**
 Make default init unavailable, as it will not initialise PilatesNode which is
 required for the setters and getters of PilatesLayout's properties to work properly.
*/
- (instancetype)init
    __attribute__((unavailable("you are not meant to initialise PilatesLayout")));

/**
  The property that decides if we should include this view when calculating
  layout. Defaults to YES.
 */
@property (nonatomic, readwrite, assign, setter=setIncludedInLayout:) BOOL isIncludedInLayout;

/**
 The property that decides during layout/sizing whether or not styling properties should be applied.
 Defaults to NO.
 */
@property (nonatomic, readwrite, assign, setter=setEnabled:) BOOL isEnabled;

@property (nonatomic, readwrite, assign) PilatesDirection direction;
@property (nonatomic, readwrite, assign) PilatesFlexDirection flexDirection;
@property (nonatomic, readwrite, assign) PilatesJustify justifyContent;
@property (nonatomic, readwrite, assign) PilatesAlign alignContent;
@property (nonatomic, readwrite, assign) PilatesAlign alignItems;
@property (nonatomic, readwrite, assign) PilatesAlign alignSelf;
@property (nonatomic, readwrite, assign) PilatesPositionType position;
@property (nonatomic, readwrite, assign) PilatesWrap flexWrap;
@property (nonatomic, readwrite, assign) PilatesOverflow overflow;
@property (nonatomic, readwrite, assign) PilatesDisplay display;

@property (nonatomic, readwrite, assign) CGFloat flexGrow;
@property (nonatomic, readwrite, assign) CGFloat flexShrink;
@property (nonatomic, readwrite, assign) PilatesValue flexBasis;

@property (nonatomic, readwrite, assign) PilatesValue left;
@property (nonatomic, readwrite, assign) PilatesValue top;
@property (nonatomic, readwrite, assign) PilatesValue right;
@property (nonatomic, readwrite, assign) PilatesValue bottom;
@property (nonatomic, readwrite, assign) PilatesValue start;
@property (nonatomic, readwrite, assign) PilatesValue end;

@property (nonatomic, readwrite, assign) PilatesValue marginLeft;
@property (nonatomic, readwrite, assign) PilatesValue marginTop;
@property (nonatomic, readwrite, assign) PilatesValue marginRight;
@property (nonatomic, readwrite, assign) PilatesValue marginBottom;
@property (nonatomic, readwrite, assign) PilatesValue marginStart;
@property (nonatomic, readwrite, assign) PilatesValue marginEnd;
@property (nonatomic, readwrite, assign) PilatesValue marginHorizontal;
@property (nonatomic, readwrite, assign) PilatesValue marginVertical;
@property (nonatomic, readwrite, assign) PilatesValue margin;

@property (nonatomic, readwrite, assign) PilatesValue paddingLeft;
@property (nonatomic, readwrite, assign) PilatesValue paddingTop;
@property (nonatomic, readwrite, assign) PilatesValue paddingRight;
@property (nonatomic, readwrite, assign) PilatesValue paddingBottom;
@property (nonatomic, readwrite, assign) PilatesValue paddingStart;
@property (nonatomic, readwrite, assign) PilatesValue paddingEnd;
@property (nonatomic, readwrite, assign) PilatesValue paddingHorizontal;
@property (nonatomic, readwrite, assign) PilatesValue paddingVertical;
@property (nonatomic, readwrite, assign) PilatesValue padding;

@property (nonatomic, readwrite, assign) CGFloat borderLeftWidth;
@property (nonatomic, readwrite, assign) CGFloat borderTopWidth;
@property (nonatomic, readwrite, assign) CGFloat borderRightWidth;
@property (nonatomic, readwrite, assign) CGFloat borderBottomWidth;
@property (nonatomic, readwrite, assign) CGFloat borderStartWidth;
@property (nonatomic, readwrite, assign) CGFloat borderEndWidth;
@property (nonatomic, readwrite, assign) CGFloat borderWidth;

@property (nonatomic, readwrite, assign) PilatesValue width;
@property (nonatomic, readwrite, assign) PilatesValue height;
@property (nonatomic, readwrite, assign) PilatesValue minWidth;
@property (nonatomic, readwrite, assign) PilatesValue minHeight;
@property (nonatomic, readwrite, assign) PilatesValue maxWidth;
@property (nonatomic, readwrite, assign) PilatesValue maxHeight;

// Pilates specific properties, not compatible with flexbox specification
@property (nonatomic, readwrite, assign) CGFloat aspectRatio;

/**
 Get the resolved direction of this node. This won't be PilatesDirectionInherit
 */
@property (nonatomic, readonly, assign) PilatesDirection resolvedDirection;

/**
 Perform a layout calculation and update the frames of the views in the hierarchy with the results.
 If the origin is not preserved, the root view's layout results will applied from {0,0}.
 */
- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
    NS_SWIFT_NAME(applyLayout(preservingOrigin:));

/**
 Perform a layout calculation and update the frames of the views in the hierarchy with the results.
 If the origin is not preserved, the root view's layout results will applied from {0,0}.
 */
- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
               dimensionFlexibility:(PilatesDimensionFlexibility)dimensionFlexibility
    NS_SWIFT_NAME(applyLayout(preservingOrigin:dimensionFlexibility:));

/**
 Returns the size of the view if no constraints were given. This could equivalent to calling [self
 sizeThatFits:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX)];
 */
@property (nonatomic, readonly, assign) CGSize intrinsicSize;

/**
  Returns the size of the view based on provided constraints. Pass NaN for an unconstrained dimension.
 */
- (CGSize)calculateLayoutWithSize:(CGSize)size
    NS_SWIFT_NAME(calculateLayout(with:));

/**
 Returns the number of children that are using Flexbox.
 */
@property (nonatomic, readonly, assign) NSUInteger numberOfChildren;

/**
 Return a BOOL indiciating whether or not we this node contains any subviews that are included in
 Pilates's layout.
 */
@property (nonatomic, readonly, assign) BOOL isLeaf;

/**
 Return's a BOOL indicating if a view is dirty. When a node is dirty
 it usually indicates that it will be remeasured on the next layout pass.
 */
@property (nonatomic, readonly, assign) BOOL isDirty;

/**
 Mark that a view's layout needs to be recalculated. Only works for leaf views.
 */
- (void)markDirty;

@end
