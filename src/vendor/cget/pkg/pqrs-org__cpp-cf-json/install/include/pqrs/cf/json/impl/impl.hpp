#pragma once

// (C) Copyright Takayama Fumihiko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <pqrs/json.hpp>

namespace pqrs {
namespace cf {
namespace json {
namespace impl {
inline std::string get_type(const nlohmann::json& json) {
  using namespace std::string_literals;

  pqrs::json::requires_object(json, "json");

  auto it = json.find("type");
  if (it == std::end(json)) {
    throw pqrs::json::unmarshal_error("`type` is not found in `"s + json.dump() + "`"s);
  }

  auto type_json = it.value();
  pqrs::json::requires_string(type_json, "`type`");

  return type_json.get<std::string>();
}

inline nlohmann::json get_value_json(const nlohmann::json& json) {
  using namespace std::string_literals;

  pqrs::json::requires_object(json, "json");

  auto it = json.find("value");
  if (it == std::end(json)) {
    throw pqrs::json::unmarshal_error("`value` is not found in `"s + json.dump() + "`"s);
  }
  return it.value();
}

inline void validate_array_value(const nlohmann::json& json) {
  pqrs::json::requires_array(json, "`value`");
}

inline void validate_boolean_value(const nlohmann::json& json) {
  pqrs::json::requires_boolean(json, "`value`");
}

inline void validate_data_value(const nlohmann::json& json) {
  pqrs::json::requires_array(json, "`value`");
}

inline void validate_dictionary_value(const nlohmann::json& json) {
  pqrs::json::requires_array(json, "`value`");
}

inline void validate_number_value(const nlohmann::json& json) {
  pqrs::json::requires_number(json, "`value`");
}

inline void validate_set_value(const nlohmann::json& json) {
  pqrs::json::requires_array(json, "`value`");
}

inline void validate_string_value(const nlohmann::json& json) {
  pqrs::json::requires_string(json, "`value`");
}

inline nlohmann::json get_dictionary_key(const nlohmann::json& json) {
  using namespace std::string_literals;

  auto it = json.find("key");
  if (it == std::end(json)) {
    throw pqrs::json::unmarshal_error("`key` is not found in `"s + json.dump() + "`"s);
  }

  return it.value();
}

inline nlohmann::json get_dictionary_value(const nlohmann::json& json) {
  using namespace std::string_literals;

  auto it = json.find("value");
  if (it == std::end(json)) {
    throw pqrs::json::unmarshal_error("`value` is not found in `"s + json.dump() + "`"s);
  }

  return it.value();
}
} // namespace impl
} // namespace json
} // namespace cf
} // namespace pqrs
