//
//  FlexLayout+Private.swift
//  FlexLayout
//
//  Created by DION, Luc (MTL) on 2017-11-23.
//  Copyright © 2017 Mirego. All rights reserved.
//

import UIKit
import FlexLayoutPilates

extension Flex {
    func valueOrUndefined(_ value: CGFloat?) -> PilatesValue {
        if let value = value {
            return PilatesValue(value)
        } else {
            return PilatesValueUndefined
        }
    }
    
    func valueOrAuto(_ value: CGFloat?) -> PilatesValue {
        if let value = value {
            return PilatesValue(value)
        } else {
            return PilatesValueAuto
        }
    }
}
