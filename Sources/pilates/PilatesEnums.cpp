/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "PilatesEnums.h"

const char *PilatesAlignToString(const PilatesAlign value){
  switch(value){
    case PilatesAlignAuto:
      return "auto";
    case PilatesAlignFlexStart:
      return "flex-start";
    case PilatesAlignCenter:
      return "center";
    case PilatesAlignFlexEnd:
      return "flex-end";
    case PilatesAlignStretch:
      return "stretch";
    case PilatesAlignBaseline:
      return "baseline";
    case PilatesAlignSpaceBetween:
      return "space-between";
    case PilatesAlignSpaceAround:
      return "space-around";
  }
  return "unknown";
}

const char *PilatesDimensionToString(const PilatesDimension value){
  switch(value){
    case PilatesDimensionWidth:
      return "width";
    case PilatesDimensionHeight:
      return "height";
  }
  return "unknown";
}

const char *PilatesDirectionToString(const PilatesDirection value){
  switch(value){
    case PilatesDirectionInherit:
      return "inherit";
    case PilatesDirectionLTR:
      return "ltr";
    case PilatesDirectionRTL:
      return "rtl";
  }
  return "unknown";
}

const char *PilatesDisplayToString(const PilatesDisplay value){
  switch(value){
    case PilatesDisplayFlex:
      return "flex";
    case PilatesDisplayNone:
      return "none";
  }
  return "unknown";
}

const char *PilatesEdgeToString(const PilatesEdge value){
  switch(value){
    case PilatesEdgeLeft:
      return "left";
    case PilatesEdgeTop:
      return "top";
    case PilatesEdgeRight:
      return "right";
    case PilatesEdgeBottom:
      return "bottom";
    case PilatesEdgeStart:
      return "start";
    case PilatesEdgeEnd:
      return "end";
    case PilatesEdgeHorizontal:
      return "horizontal";
    case PilatesEdgeVertical:
      return "vertical";
    case PilatesEdgeAll:
      return "all";
  }
  return "unknown";
}

const char *PilatesExperimentalFeatureToString(const PilatesExperimentalFeature value){
  switch(value){
    case PilatesExperimentalFeatureWebFlexBasis:
      return "web-flex-basis";
  }
  return "unknown";
}

const char *PilatesFlexDirectionToString(const PilatesFlexDirection value){
  switch(value){
    case PilatesFlexDirectionColumn:
      return "column";
    case PilatesFlexDirectionColumnReverse:
      return "column-reverse";
    case PilatesFlexDirectionRow:
      return "row";
    case PilatesFlexDirectionRowReverse:
      return "row-reverse";
  }
  return "unknown";
}

const char *PilatesJustifyToString(const PilatesJustify value){
  switch(value){
    case PilatesJustifyFlexStart:
      return "flex-start";
    case PilatesJustifyCenter:
      return "center";
    case PilatesJustifyFlexEnd:
      return "flex-end";
    case PilatesJustifySpaceBetween:
      return "space-between";
    case PilatesJustifySpaceAround:
      return "space-around";
    case PilatesJustifySpaceEvenly:
      return "space-evenly";
  }
  return "unknown";
}

const char *PilatesLogLevelToString(const PilatesLogLevel value){
  switch(value){
    case PilatesLogLevelError:
      return "error";
    case PilatesLogLevelWarn:
      return "warn";
    case PilatesLogLevelInfo:
      return "info";
    case PilatesLogLevelDebug:
      return "debug";
    case PilatesLogLevelVerbose:
      return "verbose";
    case PilatesLogLevelFatal:
      return "fatal";
  }
  return "unknown";
}

const char *PilatesMeasureModeToString(const PilatesMeasureMode value){
  switch(value){
    case PilatesMeasureModeUndefined:
      return "undefined";
    case PilatesMeasureModeExactly:
      return "exactly";
    case PilatesMeasureModeAtMost:
      return "at-most";
  }
  return "unknown";
}

const char *PilatesNodeTypeToString(const PilatesNodeType value){
  switch(value){
    case PilatesNodeTypeDefault:
      return "default";
    case PilatesNodeTypeText:
      return "text";
  }
  return "unknown";
}

const char *PilatesOverflowToString(const PilatesOverflow value){
  switch(value){
    case PilatesOverflowVisible:
      return "visible";
    case PilatesOverflowHidden:
      return "hidden";
    case PilatesOverflowScroll:
      return "scroll";
  }
  return "unknown";
}

const char *PilatesPositionTypeToString(const PilatesPositionType value){
  switch(value){
    case PilatesPositionTypeRelative:
      return "relative";
    case PilatesPositionTypeAbsolute:
      return "absolute";
  }
  return "unknown";
}

const char *PilatesPrintOptionsToString(const PilatesPrintOptions value){
  switch(value){
    case PilatesPrintOptionsLayout:
      return "layout";
    case PilatesPrintOptionsStyle:
      return "style";
    case PilatesPrintOptionsChildren:
      return "children";
  }
  return "unknown";
}

const char *PilatesUnitToString(const PilatesUnit value){
  switch(value){
    case PilatesUnitUndefined:
      return "undefined";
    case PilatesUnitPoint:
      return "point";
    case PilatesUnitPercent:
      return "percent";
    case PilatesUnitAuto:
      return "auto";
  }
  return "unknown";
}

const char *PilatesWrapToString(const PilatesWrap value){
  switch(value){
    case PilatesWrapNoWrap:
      return "no-wrap";
    case PilatesWrapWrap:
      return "wrap";
    case PilatesWrapWrapReverse:
      return "wrap-reverse";
  }
  return "unknown";
}
