/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import CoreGraphics
import FlexLayoutPilates

postfix operator %

extension Int {
    public static postfix func % (value: Int) -> PilatesValue {
        return PilatesValue(value: Float(value), unit: .percent)
    }
}

extension Float {
    public static postfix func % (value: Float) -> PilatesValue {
        return PilatesValue(value: value, unit: .percent)
    }
}

extension CGFloat {
    public static postfix func % (value: CGFloat) -> PilatesValue {
        return PilatesValue(value: Float(value), unit: .percent)
    }
}

extension PilatesValue: ExpressibleByIntegerLiteral, ExpressibleByFloatLiteral {
    public init(integerLiteral value: Int) {
        self = PilatesValue(value: Float(value), unit: .point)
    }
    
    public init(floatLiteral value: Float) {
        self = PilatesValue(value: value, unit: .point)
    }
  
    public init(_ value: Float) {
        self = PilatesValue(value: value, unit: .point)
    }
  
    public init(_ value: CGFloat) {
        self = PilatesValue(value: Float(value), unit: .point)
    }
}
