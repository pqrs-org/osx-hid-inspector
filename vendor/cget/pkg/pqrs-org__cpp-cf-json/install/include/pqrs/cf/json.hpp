#pragma once

// pqrs::cf::json v1.0

// (C) Copyright Takayama Fumihiko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <pqrs/cf/array.hpp>
#include <pqrs/cf/dictionary.hpp>
#include <pqrs/cf/number.hpp>
#include <pqrs/cf/set.hpp>
#include <pqrs/cf/string.hpp>
#include <pqrs/json.hpp>

namespace pqrs {
namespace cf {
inline nlohmann::json cf_type_to_json(CFTypeRef object) {
  nlohmann::json result;

  if (object) {
    CFTypeID type = CFGetTypeID(object);

    if (type == CFArrayGetTypeID()) {
      auto value = nlohmann::json::array();
      auto array = reinterpret_cast<CFArrayRef>(object);
      CFArrayApplyFunction(array,
                           {0, CFArrayGetCount(array)},
                           [](const void* o, void* context) {
                             auto j = reinterpret_cast<nlohmann::json*>(context);
                             j->push_back(cf_type_to_json(reinterpret_cast<CFTypeRef>(o)));
                           },
                           &value);
      result = nlohmann::json::object({
          {"type", "array"},
          {"value", value},
      });

    } else if (type == CFBooleanGetTypeID()) {
      result = nlohmann::json::object({
          {"type", "boolean"},
          {"value", static_cast<bool>(CFBooleanGetValue(reinterpret_cast<CFBooleanRef>(object)))},
      });

    } else if (type == CFDataGetTypeID()) {
      auto value = nlohmann::json::array();
      auto data = reinterpret_cast<CFDataRef>(object);
      auto length = CFDataGetLength(data);
      auto bytes = CFDataGetBytePtr(data);
      for (CFIndex i = 0; i < length; ++i) {
        value.push_back(bytes[i]);
      }
      result = nlohmann::json::object({
          {"type", "data"},
          {"value", value},
      });

    } else if (type == CFDictionaryGetTypeID()) {
      auto value = nlohmann::json::array();
      CFDictionaryApplyFunction(reinterpret_cast<CFDictionaryRef>(object),
                                [](const void* k, const void* v, void* context) {
                                  auto j = reinterpret_cast<nlohmann::json*>(context);
                                  j->push_back(nlohmann::json::object({
                                      {"key", cf_type_to_json(k)},
                                      {"value", cf_type_to_json(v)},
                                  }));
                                },
                                &value);
      result = nlohmann::json::object({
          {"type", "dictionary"},
          {"value", value},
      });

    } else if (type == CFNumberGetTypeID()) {
      result = nlohmann::json::object();

      auto number = reinterpret_cast<CFNumberRef>(object);
      if (CFNumberIsFloatType(number)) {
        if (auto v = make_number<double>(number)) {
          result["type"] = "number_float";
          result["value"] = *v;
        } else {
          throw json::marshal_error("failed to marshal CFNumber");
        }
      } else {
        if (auto v = make_number<int64_t>(number)) {
          result["type"] = "number_integer";
          result["value"] = *v;
        } else {
          throw json::marshal_error("failed to marshal CFNumber");
        }
      }

    } else if (type == CFSetGetTypeID()) {
      auto value = nlohmann::json::array();
      CFSetApplyFunction(reinterpret_cast<CFSetRef>(object),
                         [](const void* o, void* context) {
                           auto j = reinterpret_cast<nlohmann::json*>(context);
                           j->push_back(cf_type_to_json(reinterpret_cast<CFTypeRef>(o)));
                         },
                         &value);
      result = nlohmann::json::object({
          {"type", "set"},
          {"value", value},
      });

    } else if (type == CFStringGetTypeID()) {
      if (auto v = make_string(object)) {
        result = nlohmann::json::object({
            {"type", "string"},
            {"value", *v},
        });
      } else {
        throw json::marshal_error("failed to marshal CFString");
      }

    } else {
      throw json::marshal_error("unknown CFTypeRef");
    }
  }

  return result;
}

inline cf_ptr<CFTypeRef> json_to_cf_type(const nlohmann::json& json) {
  using namespace std::string_literals;

  cf_ptr<CFTypeRef> result;

  if (!json.is_object()) {
    throw json::unmarshal_error("json must be object, but is `"s + json.dump() + "`"s);
  }

  //
  // Type
  //

  std::string type;
  {
    auto it = json.find("type");
    if (it == std::end(json)) {
      throw json::unmarshal_error("`type` is not found in `"s + json.dump() + "`"s);
    }
    auto type_json = it.value();
    if (!type_json.is_string()) {
      throw json::unmarshal_error("`type` must be string, but is `"s + type_json.dump() + "`"s);
    }
    type = type_json.get<std::string>();
  }

  //
  // Value
  //

  nlohmann::json value_json;
  {
    auto it = json.find("value");
    if (it == std::end(json)) {
      throw json::unmarshal_error("`value` is not found in `"s + json.dump() + "`"s);
    }
    value_json = it.value();
  }

  if (type == "array") {
    if (!value_json.is_array()) {
      throw json::unmarshal_error("`value` must be array, but is `"s + value_json.dump() + "`"s);
    }

    auto array = make_cf_mutable_array();
    for (const auto& j : value_json) {
      if (auto v = json_to_cf_type(j)) {
        CFArrayAppendValue(*array, *v);
      }
    }

    result = cf_ptr<CFTypeRef>(*array);

  } else if (type == "boolean") {
    if (!value_json.is_boolean()) {
      throw json::unmarshal_error("`value` must be boolean, but is `"s + value_json.dump() + "`"s);
    }

    if (value_json.get<bool>()) {
      result = cf_ptr<CFTypeRef>(kCFBooleanTrue);
    } else {
      result = cf_ptr<CFTypeRef>(kCFBooleanFalse);
    }

  } else if (type == "data") {
    if (!value_json.is_array()) {
      throw json::unmarshal_error("`value` must be array, but is `"s + value_json.dump() + "`"s);
    }

    std::vector<uint8_t> bytes;
    for (const auto& j : value_json) {
      if (!j.is_number()) {
        throw json::unmarshal_error("`value` must be array of numbers, but is `"s + value_json.dump() + "`"s);
      }

      bytes.push_back(j.get<uint8_t>());
    }

    auto data = CFDataCreate(kCFAllocatorDefault,
                             &(bytes[0]),
                             bytes.size());
    result = cf_ptr<CFTypeRef>(data);
    CFRelease(data);

  } else if (type == "dictionary") {
    if (!value_json.is_array()) {
      throw json::unmarshal_error("`value` must be array, but is `"s + value_json.dump() + "`"s);
    }

    auto dictionary = make_cf_mutable_dictionary();

    for (const auto& j : value_json) {
      if (!j.is_object()) {
        throw json::unmarshal_error("`value` must be array of objects, but is `"s + value_json.dump() + "`"s);
      }

      nlohmann::json k_json;
      {
        auto it = j.find("key");
        if (it == std::end(j)) {
          throw json::unmarshal_error("`key` is not found in `"s + j.dump() + "`"s);
        }
        k_json = it.value();
      }

      nlohmann::json v_json;
      {
        auto it = j.find("value");
        if (it == std::end(j)) {
          throw json::unmarshal_error("`value` is not found in `"s + j.dump() + "`"s);
        }
        v_json = it.value();
      }

      if (auto k = json_to_cf_type(k_json)) {
        if (auto v = json_to_cf_type(v_json)) {
          CFDictionaryAddValue(*dictionary, *k, *v);
        }
      }
    }

    result = cf_ptr<CFTypeRef>(*dictionary);

  } else if (type == "number_float") {
    if (!value_json.is_number()) {
      throw json::unmarshal_error("`value` must be number, but is `"s + value_json.dump() + "`"s);
    }

    if (auto number = make_cf_number(value_json.get<double>())) {
      result = cf_ptr<CFTypeRef>(*number);
    }

  } else if (type == "number_integer") {
    if (!value_json.is_number()) {
      throw json::unmarshal_error("`value` must be number, but is `"s + value_json.dump() + "`"s);
    }

    if (auto number = make_cf_number(value_json.get<int64_t>())) {
      result = cf_ptr<CFTypeRef>(*number);
    }

  } else if (type == "set") {
    if (!value_json.is_array()) {
      throw json::unmarshal_error("`value` must be array, but is `"s + value_json.dump() + "`"s);
    }

    auto set = make_cf_mutable_set();
    for (const auto& j : value_json) {
      if (auto v = json_to_cf_type(j)) {
        CFSetAddValue(*set, *v);
      }
    }

    result = cf_ptr<CFTypeRef>(*set);

  } else if (type == "string") {
    if (!value_json.is_string()) {
      throw json::unmarshal_error("`value` must be string, but is `"s + value_json.dump() + "`"s);
    }

    if (auto string = make_cf_string(value_json.get<std::string>())) {
      result = cf_ptr<CFTypeRef>(*string);
    }

  } else {
    throw json::unmarshal_error("unknown type: `"s + type + "`"s);
  }

  return result;
}
} // namespace cf
} // namespace pqrs
