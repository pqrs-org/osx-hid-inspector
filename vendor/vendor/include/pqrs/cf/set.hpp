#pragma once

// pqrs::cf::set v1.0

// (C) Copyright Takayama Fumihiko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <pqrs/cf/cf_ptr.hpp>

namespace pqrs {
namespace cf {
inline cf_ptr<CFSetRef> make_empty_cf_set(void) {
  cf_ptr<CFSetRef> result;

  if (auto cf_set = CFSetCreate(kCFAllocatorDefault,
                                nullptr,
                                0,
                                &kCFTypeSetCallBacks)) {
    result = cf_set;
    CFRelease(cf_set);
  }

  return result;
}

inline cf_ptr<CFMutableSetRef> make_cf_mutable_set(CFIndex capacity = 0) {
  cf_ptr<CFMutableSetRef> result;

  if (auto cf_mutable_set = CFSetCreateMutable(kCFAllocatorDefault,
                                               capacity,
                                               &kCFTypeSetCallBacks)) {
    result = cf_mutable_set;
    CFRelease(cf_mutable_set);
  }

  return result;
}
} // namespace cf
} // namespace pqrs
