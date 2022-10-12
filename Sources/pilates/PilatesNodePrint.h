/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#pragma once
#include <string>

#include "Pilates.h"

namespace facebook {
namespace pilates {

void PilatesNodeToString(
    std::string* str,
    PilatesNodeRef node,
    PilatesPrintOptions options,
    uint32_t level);

} // namespace pilates
} // namespace facebook
