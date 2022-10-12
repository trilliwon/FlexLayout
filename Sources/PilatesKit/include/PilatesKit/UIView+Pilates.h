/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "PilatesLayout.h"
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^PilatesLayoutConfigurationBlock)(PilatesLayout *layout);

@interface UIView (Pilates)

/**
 The PilatesLayout that is attached to this view. It is lazily created.
 */
@property (nonatomic, readonly, strong) PilatesLayout *pilates;
/**
 Indicates whether or not Pilates is enabled
 */
@property (nonatomic, readonly, assign) BOOL isPilatesEnabled;

/**
 In ObjC land, every time you access `view.pilates.*` you are adding another `objc_msgSend`
 to your code. If you plan on making multiple changes to PilatesLayout, it's more performant
 to use this method, which uses a single objc_msgSend call.
 */
- (void)configureLayoutWithBlock:(PilatesLayoutConfigurationBlock)block
    NS_SWIFT_NAME(configureLayout(block:));

@end

NS_ASSUME_NONNULL_END
