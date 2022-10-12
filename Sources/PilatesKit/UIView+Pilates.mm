/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "UIView+Pilates.h"
#import "PilatesLayout+Private.h"
#import <objc/runtime.h>

static const void *kPilatesPilatesAssociatedKey = &kPilatesPilatesAssociatedKey;

@implementation UIView (PilatesKit)

- (PilatesLayout *)pilates
{
  PilatesLayout *pilates = objc_getAssociatedObject(self, kPilatesPilatesAssociatedKey);
  if (!pilates) {
    pilates = [[PilatesLayout alloc] initWithView:self];
    objc_setAssociatedObject(self, kPilatesPilatesAssociatedKey, pilates, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }

  return pilates;
}

- (BOOL)isPilatesEnabled
{
  return objc_getAssociatedObject(self, kPilatesPilatesAssociatedKey) != nil;
}

- (void)configureLayoutWithBlock:(PilatesLayoutConfigurationBlock)block
{
  if (block != nil) {
    block(self.pilates);
  }
}

@end
