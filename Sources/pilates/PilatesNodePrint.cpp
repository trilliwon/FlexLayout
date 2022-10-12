/*
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "PilatesNodePrint.h"
#include <stdarg.h>
#include "PilatesEnums.h"
#include "PilatesNode.h"
#include "Pilates-internal.h"

namespace facebook {
namespace pilates {
typedef std::string string;

static void indent(string* base, uint32_t level) {
  for (uint32_t i = 0; i < level; ++i) {
    base->append("  ");
  }
}

static bool areFourValuesEqual(const std::array<PilatesValue, PilatesEdgeCount>& four) {
  return PilatesValueEqual(four[0], four[1]) && PilatesValueEqual(four[0], four[2]) &&
      PilatesValueEqual(four[0], four[3]);
}

static void appendFormatedString(string* str, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  va_list argsCopy;
  va_copy(argsCopy, args);
  std::vector<char> buf(1 + vsnprintf(NULL, 0, fmt, args));
  va_end(args);
  vsnprintf(buf.data(), buf.size(), fmt, argsCopy);
  va_end(argsCopy);
  string result = string(buf.begin(), buf.end() - 1);
  str->append(result);
}

static void
appendFloatIfNotUndefined(string* base, const string key, const float num) {
  if (!PilatesFloatIsUndefined(num)) {
    appendFormatedString(base, "%s: %g; ", key.c_str(), num);
  }
}

static void appendNumberIfNotUndefined(
    string* base,
    const string key,
    const PilatesValue number) {
  if (number.unit != PilatesUnitUndefined) {
    if (number.unit == PilatesUnitAuto) {
      base->append(key + ": auto; ");
    } else {
      string unit = number.unit == PilatesUnitPoint ? "px" : "%%";
      appendFormatedString(
          base, "%s: %g%s; ", key.c_str(), number.value, unit.c_str());
    }
  }
}

static void
appendNumberIfNotAuto(string* base, const string& key, const PilatesValue number) {
  if (number.unit != PilatesUnitAuto) {
    appendNumberIfNotUndefined(base, key, number);
  }
}

static void
appendNumberIfNotZero(string* base, const string& str, const PilatesValue number) {
  if (!PilatesFloatsEqual(number.value, 0)) {
    appendNumberIfNotUndefined(base, str, number);
  }
}

static void appendEdges(
    string* base,
    const string& key,
    const std::array<PilatesValue, PilatesEdgeCount>& edges) {
  if (areFourValuesEqual(edges)) {
    appendNumberIfNotZero(base, key, edges[PilatesEdgeLeft]);
  } else {
    for (int edge = PilatesEdgeLeft; edge != PilatesEdgeAll; ++edge) {
      string str = key + "-" + PilatesEdgeToString(static_cast<PilatesEdge>(edge));
      appendNumberIfNotZero(base, str, edges[edge]);
    }
  }
}

static void appendEdgeIfNotUndefined(
    string* base,
    const string& str,
    const std::array<PilatesValue, PilatesEdgeCount>& edges,
    const PilatesEdge edge) {
  appendNumberIfNotUndefined(
      base, str, *PilatesComputedEdgeValue(edges, edge, &PilatesValueUndefined));
}

void PilatesNodeToString(
    std::string* str,
    PilatesNodeRef node,
    PilatesPrintOptions options,
    uint32_t level) {
  indent(str, level);
  appendFormatedString(str, "<div ");
  if (node->getPrintFunc() != nullptr) {
    node->getPrintFunc()(node);
  }

  if (options & PilatesPrintOptionsLayout) {
    appendFormatedString(str, "layout=\"");
    appendFormatedString(
        str, "width: %g; ", node->getLayout().dimensions[PilatesDimensionWidth]);
    appendFormatedString(
        str, "height: %g; ", node->getLayout().dimensions[PilatesDimensionHeight]);
    appendFormatedString(
        str, "top: %g; ", node->getLayout().position[PilatesEdgeTop]);
    appendFormatedString(
        str, "left: %g;", node->getLayout().position[PilatesEdgeLeft]);
    appendFormatedString(str, "\" ");
  }

  if (options & PilatesPrintOptionsStyle) {
    appendFormatedString(str, "style=\"");
    if (node->getStyle().flexDirection != PilatesNode().getStyle().flexDirection) {
      appendFormatedString(
          str,
          "flex-direction: %s; ",
          PilatesFlexDirectionToString(node->getStyle().flexDirection));
    }
    if (node->getStyle().justifyContent != PilatesNode().getStyle().justifyContent) {
      appendFormatedString(
          str,
          "justify-content: %s; ",
          PilatesJustifyToString(node->getStyle().justifyContent));
    }
    if (node->getStyle().alignItems != PilatesNode().getStyle().alignItems) {
      appendFormatedString(
          str,
          "align-items: %s; ",
          PilatesAlignToString(node->getStyle().alignItems));
    }
    if (node->getStyle().alignContent != PilatesNode().getStyle().alignContent) {
      appendFormatedString(
          str,
          "align-content: %s; ",
          PilatesAlignToString(node->getStyle().alignContent));
    }
    if (node->getStyle().alignSelf != PilatesNode().getStyle().alignSelf) {
      appendFormatedString(
          str, "align-self: %s; ", PilatesAlignToString(node->getStyle().alignSelf));
    }
    appendFloatIfNotUndefined(str, "flex-grow", node->getStyle().flexGrow);
    appendFloatIfNotUndefined(str, "flex-shrink", node->getStyle().flexShrink);
    appendNumberIfNotAuto(str, "flex-basis", node->getStyle().flexBasis);
    appendFloatIfNotUndefined(str, "flex", node->getStyle().flex);

    if (node->getStyle().flexWrap != PilatesNode().getStyle().flexWrap) {
      appendFormatedString(
          str, "flexWrap: %s; ", PilatesWrapToString(node->getStyle().flexWrap));
    }

    if (node->getStyle().overflow != PilatesNode().getStyle().overflow) {
      appendFormatedString(
          str, "overflow: %s; ", PilatesOverflowToString(node->getStyle().overflow));
    }

    if (node->getStyle().display != PilatesNode().getStyle().display) {
      appendFormatedString(
          str, "display: %s; ", PilatesDisplayToString(node->getStyle().display));
    }
    appendEdges(str, "margin", node->getStyle().margin);
    appendEdges(str, "padding", node->getStyle().padding);
    appendEdges(str, "border", node->getStyle().border);

    appendNumberIfNotAuto(
        str, "width", node->getStyle().dimensions[PilatesDimensionWidth]);
    appendNumberIfNotAuto(
        str, "height", node->getStyle().dimensions[PilatesDimensionHeight]);
    appendNumberIfNotAuto(
        str, "max-width", node->getStyle().maxDimensions[PilatesDimensionWidth]);
    appendNumberIfNotAuto(
        str, "max-height", node->getStyle().maxDimensions[PilatesDimensionHeight]);
    appendNumberIfNotAuto(
        str, "min-width", node->getStyle().minDimensions[PilatesDimensionWidth]);
    appendNumberIfNotAuto(
        str, "min-height", node->getStyle().minDimensions[PilatesDimensionHeight]);

    if (node->getStyle().positionType != PilatesNode().getStyle().positionType) {
      appendFormatedString(
          str,
          "position: %s; ",
          PilatesPositionTypeToString(node->getStyle().positionType));
    }

    appendEdgeIfNotUndefined(
        str, "left", node->getStyle().position, PilatesEdgeLeft);
    appendEdgeIfNotUndefined(
        str, "right", node->getStyle().position, PilatesEdgeRight);
    appendEdgeIfNotUndefined(str, "top", node->getStyle().position, PilatesEdgeTop);
    appendEdgeIfNotUndefined(
        str, "bottom", node->getStyle().position, PilatesEdgeBottom);
    appendFormatedString(str, "\" ");

    if (node->getMeasure() != nullptr) {
      appendFormatedString(str, "has-custom-measure=\"true\"");
    }
  }
  appendFormatedString(str, ">");

  const uint32_t childCount = static_cast<uint32_t>(node->getChildren().size());
  if (options & PilatesPrintOptionsChildren && childCount > 0) {
    for (uint32_t i = 0; i < childCount; i++) {
      appendFormatedString(str, "\n");
      PilatesNodeToString(str, PilatesNodeGetChild(node, i), options, level + 1);
    }
    appendFormatedString(str, "\n");
    indent(str, level);
  }
  appendFormatedString(str, "</div>");
}
} // namespace pilates
} // namespace facebook
