#pragma once

// pqrs::cf::json v2.2

// (C) Copyright Takayama Fumihiko 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include "json/impl/impl.hpp"
#include "json/strip_option.hpp"
#include <pqrs/cf/array.hpp>
#include <pqrs/cf/dictionary.hpp>
#include <pqrs/cf/number.hpp>
#include <pqrs/cf/set.hpp>
#include <pqrs/cf/string.hpp>
#include <pqrs/json.hpp>
#include <vector>

namespace pqrs {
namespace cf {
namespace json {
inline nlohmann::json to_json(CFTypeRef object) {
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
                             j->push_back(to_json(reinterpret_cast<CFTypeRef>(o)));
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
                                      {"key", to_json(k)},
                                      {"value", to_json(v)},
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
          throw pqrs::json::marshal_error("failed to marshal CFNumber");
        }
      } else {
        if (auto v = make_number<int64_t>(number)) {
          result["type"] = "number_integer";
          result["value"] = *v;
        } else {
          throw pqrs::json::marshal_error("failed to marshal CFNumber");
        }
      }

    } else if (type == CFSetGetTypeID()) {
      auto value = nlohmann::json::array();
      CFSetApplyFunction(reinterpret_cast<CFSetRef>(object),
                         [](const void* o, void* context) {
                           auto j = reinterpret_cast<nlohmann::json*>(context);
                           j->push_back(to_json(reinterpret_cast<CFTypeRef>(o)));
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
        throw pqrs::json::marshal_error("failed to marshal CFString");
      }

    } else {
      throw pqrs::json::marshal_error("unknown CFTypeRef");
    }
  }

  return result;
}

inline cf_ptr<CFTypeRef> to_cf_type(const nlohmann::json& json) {
  using namespace std::string_literals;

  cf_ptr<CFTypeRef> result;

  auto type = impl::get_type(json);
  auto value_json = impl::get_value_json(json);

  if (type == "array") {
    impl::validate_array_value(value_json);

    auto array = make_cf_mutable_array();
    for (const auto& j : value_json) {
      if (auto v = to_cf_type(j)) {
        CFArrayAppendValue(*array, *v);
      }
    }

    result = cf_ptr<CFTypeRef>(*array);

  } else if (type == "boolean") {
    impl::validate_boolean_value(value_json);

    if (value_json.get<bool>()) {
      result = cf_ptr<CFTypeRef>(kCFBooleanTrue);
    } else {
      result = cf_ptr<CFTypeRef>(kCFBooleanFalse);
    }

  } else if (type == "data") {
    impl::validate_data_value(value_json);

    std::vector<uint8_t> bytes;
    for (const auto& j : value_json) {
      pqrs::json::requires_number(j, "entry of `value`");

      bytes.push_back(j.get<uint8_t>());
    }

    auto data = CFDataCreate(kCFAllocatorDefault,
                             &(bytes[0]),
                             bytes.size());
    result = cf_ptr<CFTypeRef>(data);
    CFRelease(data);

  } else if (type == "dictionary") {
    impl::validate_dictionary_value(value_json);

    auto dictionary = make_cf_mutable_dictionary();

    for (const auto& j : value_json) {
      pqrs::json::requires_object(j, "entry of `value`");

      nlohmann::json k_json = impl::get_dictionary_key(j);
      nlohmann::json v_json = impl::get_dictionary_value(j);

      if (auto k = to_cf_type(k_json)) {
        if (auto v = to_cf_type(v_json)) {
          CFDictionaryAddValue(*dictionary, *k, *v);
        }
      }
    }

    result = cf_ptr<CFTypeRef>(*dictionary);

  } else if (type == "number_float") {
    impl::validate_number_value(value_json);

    if (auto number = make_cf_number(value_json.get<double>())) {
      result = cf_ptr<CFTypeRef>(*number);
    }

  } else if (type == "number_integer") {
    impl::validate_number_value(value_json);

    if (auto number = make_cf_number(value_json.get<int64_t>())) {
      result = cf_ptr<CFTypeRef>(*number);
    }

  } else if (type == "set") {
    impl::validate_set_value(value_json);

    auto set = make_cf_mutable_set();
    for (const auto& j : value_json) {
      if (auto v = to_cf_type(j)) {
        CFSetAddValue(*set, *v);
      }
    }

    result = cf_ptr<CFTypeRef>(*set);

  } else if (type == "string") {
    impl::validate_string_value(value_json);

    if (auto string = make_cf_string(value_json.get<std::string>())) {
      result = cf_ptr<CFTypeRef>(*string);
    }

  } else {
    throw pqrs::json::unmarshal_error("unknown type: `"s + type + "`"s);
  }

  return result;
}

inline nlohmann::json strip_cf_type_json(const nlohmann::json& json,
                                         type_safe::flag_set<strip_option> options = strip_option::none) {
  using namespace std::string_literals;

  nlohmann::json result;

  auto type = impl::get_type(json);
  auto value_json = impl::get_value_json(json);

  if (type == "array") {
    impl::validate_array_value(value_json);

    result = nlohmann::json::array();
    for (const auto& j : value_json) {
      result.push_back(strip_cf_type_json(j, options));
    }

  } else if (type == "boolean") {
    impl::validate_boolean_value(value_json);

    result = value_json;

  } else if (type == "data") {
    impl::validate_data_value(value_json);

    result = value_json;

  } else if (type == "dictionary") {
    impl::validate_dictionary_value(value_json);

    if (options & strip_option::collapse_dictionary) {
      result = nlohmann::json::object();

      for (const auto& j : value_json) {
        nlohmann::json k_json = impl::get_dictionary_key(j);
        nlohmann::json v_json = impl::get_dictionary_value(j);

        std::string k_string;
        auto k = strip_cf_type_json(k_json, options);
        if (k.is_string()) {
          k_string = k.get<std::string>();
        } else {
          k_string = k.dump();
        }

        result[k_string] = strip_cf_type_json(v_json, options);
      }

    } else {
      result = nlohmann::json::array();

      for (const auto& j : value_json) {
        nlohmann::json k_json = impl::get_dictionary_key(j);
        nlohmann::json v_json = impl::get_dictionary_value(j);

        result.push_back(nlohmann::json::object({
            {"key", strip_cf_type_json(k_json, options)},
            {"value", strip_cf_type_json(v_json, options)},
        }));
      }
    }

  } else if (type == "number_float") {
    impl::validate_number_value(value_json);

    result = value_json;

  } else if (type == "number_integer") {
    impl::validate_number_value(value_json);

    result = value_json;

  } else if (type == "set") {
    impl::validate_set_value(value_json);

    result = nlohmann::json::array();
    for (const auto& j : value_json) {
      result.push_back(strip_cf_type_json(j, options));
    }

  } else if (type == "string") {
    impl::validate_string_value(value_json);

    result = value_json;
  }

  return result;
}
} // namespace json
} // namespace cf
} // namespace pqrs
