/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "UIView+Pilates.h"
#import "PilatesLayout+Private.h"
#import <objc/runtime.h>

static const void *kYGPilatesAssociatedKey = &kYGPilatesAssociatedKey;

@implementation UIView (PilatesKit)

- (YGLayout *)pilates
{
  YGLayout *pilates = objc_getAssociatedObject(self, kYGPilatesAssociatedKey);
  if (!pilates) {
    pilates = [[YGLayout alloc] initWithView:self];
    objc_setAssociatedObject(self, kYGPilatesAssociatedKey, pilates, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }

  return pilates;
}

- (BOOL)isPilatesEnabled
{
  return objc_getAssociatedObject(self, kYGPilatesAssociatedKey) != nil;
}

- (void)configureLayoutWithBlock:(YGLayoutConfigurationBlock)block
{
  if (block != nil) {
    block(self.pilates);
  }
}

@end
