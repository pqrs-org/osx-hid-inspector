#include <iomanip>
#include <map>
#include <pqrs/cf/json.hpp>
#include <pqrs/osx/iokit_hid_element.hpp>
#include <pqrs/osx/iokit_registry_entry.hpp>
#include <set>
#include <sstream>

namespace {
typedef std::map<pqrs::osx::iokit_hid_element_type,
                 std::map<pqrs::hid::usage_page::value_t,
                          std::set<pqrs::hid::usage::value_t>>>
    collected_result_t;

void collect_usage(collected_result_t& result, const nlohmann::json& json) {
  if (json.is_object()) {
    if (auto elements = pqrs::json::find_json(json, "Elements")) {
      for (const auto& dictionary : elements->value()) {
        if (auto element_type = pqrs::json::find<int>(dictionary, "Type")) {
          if (auto usage_page = pqrs::json::find<int>(dictionary, "UsagePage")) {
            if (auto usage = pqrs::json::find<int>(dictionary, "Usage")) {
              auto et = pqrs::osx::iokit_hid_element_type(*element_type);
              auto up = pqrs::hid::usage_page::value_t(*usage_page);
              auto u = pqrs::hid::usage::value_t(*usage);
              result[et][up].insert(u);
            }
          }
        }
      }
    }
    for (const auto& [k, v] : json.items()) {
      collect_usage(result, v);
    }
  } else if (json.is_array()) {
    for (const auto& j : json) {
      collect_usage(result, j);
    }
  }
}

void output(pqrs::osx::iokit_registry_entry registry_entry) {
  if (!registry_entry) {
    return;
  }

  if (registry_entry.get().conforms_to("IOHIDDevice")) {
    std::cout << "========================================" << std::endl;

    if (auto id = registry_entry.find_registry_entry_id()) {
      std::cout << "registry_entry_id: " << *id << std::endl;
    }

    if (auto properties = registry_entry.find_properties()) {
      auto properties_json = pqrs::cf::json::strip_cf_type_json(pqrs::cf::json::to_json(*properties),
                                                                pqrs::cf::json::strip_option::collapse_dictionary);

      for (const auto& key : {
               "Manufacturer",
               "Product",
               "Transport",
               "VendorID",
               "ProductID",
               "DeviceUsagePairs",
               "PrimaryUsagePage",
               "PrimaryUsage",
           }) {
        if (auto it = pqrs::json::find_json(properties_json, key)) {
          std::cout << "    " << key << ": " << it->value() << std::endl;
        }
      }

      collected_result_t collected_result;
      collect_usage(collected_result, properties_json);

      for (const auto& [element_type, usage_pages] : collected_result) {
        std::cout << "    ==============================" << std::endl;
        std::cout << "    element_type: " << pqrs::osx::get_iokit_hid_element_type_name(element_type) << std::endl;
        for (const auto& [usage_page, usages] : usage_pages) {
          std::cout << "        --------------------------" << std::endl;
          std::cout << "        usage_page: " << usage_page << std::hex << " (0x" << usage_page << ")" << std::dec << std::endl;
          std::cout << "            usages: [" << std::endl;

          // Join sequences

          std::vector<std::string> joined_usages;
          {
            std::optional<pqrs::hid::usage::value_t> joined_first_usage;
            std::optional<pqrs::hid::usage::value_t> last_usage;
            for (const auto& usage : usages) {
              std::stringstream ss;

              if (joined_first_usage &&
                  last_usage &&
                  type_safe::get(usage) == type_safe::get(*last_usage) + 1 &&
                  !joined_usages.empty()) {
                joined_usages.pop_back();

                if (type_safe::get(usage) - type_safe::get(*joined_first_usage) < 8) {
                  for (int i = type_safe::get(*joined_first_usage); i <= type_safe::get(usage); ++i) {
                    ss << i << std::hex << " (0x" << i << ")" << std::dec;
                    if (i != type_safe::get(usage)) {
                      ss << ", ";
                    }
                  }
                } else {
                  ss << *joined_first_usage << " ... " << usage;
                }
              } else {
                joined_first_usage = usage;
                ss << usage << std::hex << " (0x" << usage << ")" << std::dec;
              }

              joined_usages.push_back(ss.str());

              last_usage = usage;
            }
          }

          for (const auto& usage : joined_usages) {
            std::cout << "                " << usage << "," << std::endl;
          }

          std::cout << "            ] (" << usages.size() << " entries)" << std::endl;
        }
      }
    }

    std::cout << std::endl;
  }

  auto child_iterator = registry_entry.get_child_iterator(kIOServicePlane);
  while (true) {
    auto next = child_iterator.next();
    if (!next) {
      break;
    }

    output(pqrs::osx::iokit_registry_entry(next));
  }
}
} // namespace

void output_usages(void) {
  output(pqrs::osx::iokit_registry_entry::get_root_entry());
}
