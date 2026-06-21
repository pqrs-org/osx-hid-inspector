#pragma once

// pqrs::cf::set v1.1.0

// (C) Copyright Takayama Fumihiko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See https://www.boost.org/LICENSE_1_0.txt)

#include <pqrs/cf/cf_ptr.hpp>

namespace pqrs::cf {
[[nodiscard]] inline cf_ptr<CFSetRef> make_empty_cf_set() noexcept {
  return adopt_cf_ptr(CFSetCreate(kCFAllocatorDefault,
                                  nullptr,
                                  0,
                                  &kCFTypeSetCallBacks));
}

[[nodiscard]] inline cf_ptr<CFMutableSetRef> make_cf_mutable_set(CFIndex capacity = 0) noexcept {
  return adopt_cf_ptr(CFSetCreateMutable(kCFAllocatorDefault,
                                         capacity,
                                         &kCFTypeSetCallBacks));
}
} // namespace pqrs::cf
