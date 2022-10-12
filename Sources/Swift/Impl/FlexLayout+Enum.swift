// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Created by Luc Dion on 2017-07-17.

import UIKit

import FlexLayoutPilates

extension PilatesFlexDirection {
    static let column = PilatesFlexDirectionColumn
    static let columnReverse = PilatesFlexDirectionColumnReverse
    static let row = PilatesFlexDirectionRow
    static let rowReverse = PilatesFlexDirectionRowReverse
}

extension PilatesJustify {
    static let flexStart = PilatesJustifyFlexStart
    static let center = PilatesJustifyCenter
    static let flexEnd = PilatesJustifyFlexEnd
    static let spaceBetween = PilatesJustifySpaceBetween
    static let spaceAround = PilatesJustifySpaceAround
    static let spaceEvenly = PilatesJustifySpaceEvenly
}

extension PilatesAlign {
    static let auto = PilatesAlignAuto
    static let baseline = PilatesAlignBaseline
    static let stretch = PilatesAlignStretch
    static let flexStart = PilatesAlignFlexStart
    static let center = PilatesAlignCenter
    static let flexEnd = PilatesAlignFlexEnd
    static let spaceBetween = PilatesAlignSpaceBetween
    static let spaceAround = PilatesAlignSpaceAround
}

extension PilatesWrap {
    static let noWrap = PilatesWrapNoWrap
    static let wrap = PilatesWrapWrap
    static let wrapReverse = PilatesWrapWrapReverse
}

extension PilatesPositionType {
    static let relative = PilatesPositionTypeRelative
    static let absolute = PilatesPositionTypeAbsolute
}

extension PilatesDirection {
    static let LTR = PilatesDirectionLTR
    static let RTL = PilatesDirectionRTL
    static let inherit = PilatesDirectionInherit
}

extension PilatesDisplay {
    static let flex = PilatesDisplayFlex
    static let none = PilatesDisplayNone
}

extension PilatesUnit {
    static let percent = PilatesUnitPercent
    static let point = PilatesUnitPoint
}

extension Flex.Direction {
    var pilatesValue: PilatesFlexDirection {
        switch self {
        case .column:        return PilatesFlexDirection.column
        case .columnReverse: return PilatesFlexDirection.columnReverse
        case .row:           return PilatesFlexDirection.row
        case .rowReverse:    return PilatesFlexDirection.rowReverse
        }
    }
}

extension Flex.JustifyContent {
    var pilatesValue: PilatesJustify {
        switch self {
        case .start:        return PilatesJustify.flexStart
        case .center:       return PilatesJustify.center
        case .end:          return PilatesJustify.flexEnd
        case .spaceBetween: return PilatesJustify.spaceBetween
        case .spaceAround:  return PilatesJustify.spaceAround
        case .spaceEvenly:  return PilatesJustify.spaceEvenly
        }
    }
}

extension Flex.AlignContent {
    var pilatesValue: PilatesAlign {
        switch self {
        case .stretch:      return PilatesAlign.stretch
        case .start:        return PilatesAlign.flexStart
        case .center:       return PilatesAlign.center
        case .end:          return PilatesAlign.flexEnd
        case .spaceBetween: return PilatesAlign.spaceBetween
        case .spaceAround:  return PilatesAlign.spaceAround
        }
    }
}

extension Flex.AlignItems {
    var pilatesValue: PilatesAlign {
        switch self {
        case .stretch:      return PilatesAlign.stretch
        case .start:        return PilatesAlign.flexStart
        case .center:       return PilatesAlign.center
        case .end:          return PilatesAlign.flexEnd
        case .baseline:     return PilatesAlign.baseline
        }
    }
}

extension Flex.AlignSelf {
    var pilatesValue: PilatesAlign {
        switch self {
        case .auto:         return PilatesAlign.auto
        case .stretch:      return PilatesAlign.stretch
        case .start:        return PilatesAlign.flexStart
        case .center:       return PilatesAlign.center
        case .end:          return PilatesAlign.flexEnd
        case .baseline:     return PilatesAlign.baseline
        }
    }
}

extension Flex.Wrap {
    var pilatesValue: PilatesWrap {
        switch self {
        case .noWrap:      return PilatesWrap.noWrap
        case .wrap:        return PilatesWrap.wrap
        case .wrapReverse: return PilatesWrap.wrapReverse
        }
    }
}

extension Flex.Position {
    var pilatesValue: PilatesPositionType {
        switch self {
        case .relative: return PilatesPositionType.relative
        case .absolute: return PilatesPositionType.absolute
        }
    }
}

extension Flex.LayoutDirection {
    var pilatesValue: PilatesDirection {
        switch self {
        case .ltr: return PilatesDirection.LTR
        case .rtl: return PilatesDirection.RTL
        default:   return PilatesDirection.inherit
        }
    }
}

extension Flex.Display {
    var pilatesValue: PilatesDisplay {
        switch self {
        case .flex: return PilatesDisplay.flex
        case .none: return PilatesDisplay.none
        }
    }
}

/*extension Flex.Overflow {
    var pilatesValue: PilatesOverflow {
        switch self {
        case .visible: return PilatesOverflow.visible
        case .hidden:  return PilatesOverflow.hidden
        case .scroll:  return PilatesOverflow.scroll
        }
    }
}*/
