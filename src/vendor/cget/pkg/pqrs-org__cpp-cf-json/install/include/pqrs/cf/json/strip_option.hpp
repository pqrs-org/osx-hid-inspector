#pragma once

// (C) Copyright Takayama Fumihiko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <type_safe/flag_set.hpp>

namespace pqrs {
namespace cf {
namespace json {
enum class strip_option {
  none,

  // strip_option::collapse_dictionary changes the result of "dictionry".
  //
  // If CFDictionary is @[ @"key1": @"value1", @123 : @"value2" ], then
  // strip_cf_type_json(to_json(dict)) is:
  //
  // default:
  // [
  //     {
  //         "key": "key1",
  //         "value": "value1",
  //     },
  //     {
  //         "key": 123,
  //         "value": "value2",
  //     }
  // ]
  //
  // with strip_option::collapse_dictionary
  // {
  //     "key1": "value1",
  //     "123": "value2"
  // }
  //
  // Note:
  // The key is converted string since json object key must be string.
  // (e.g., 123 -> "123".)

  collapse_dictionary,

  end_,
};
} // namespace json
} // namespace cf
} // namespace pqrs

namespace type_safe {
template <>
struct flag_set_traits<pqrs::cf::json::strip_option> : std::true_type {
  static constexpr std::size_t size() {
    return static_cast<std::size_t>(pqrs::cf::json::strip_option::end_) + 1;
  }
};
} // namespace type_safe
