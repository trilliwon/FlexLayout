/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "PilatesLayout.h"
#if FLEXLAYOUT_SWIFT_PACKAGE
#import <pilates/Pilates.h>
#else
#import "Pilates.h"
#endif

@interface PilatesLayout ()

@property (nonatomic, assign, readonly) PilatesNodeRef node;

- (instancetype)initWithView:(UIView *)view;

@end
