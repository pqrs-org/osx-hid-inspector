#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <pqrs/cf/json.hpp>
#include <pqrs/osx/iokit_object_ptr.hpp>
#include <pqrs/osx/iokit_registry_entry.hpp>
#include <set>

namespace {
#if 1
void collect_usage(std::map<int, std::set<int>>& result, const nlohmann::json& json) {
  if (json.is_object()) {
    auto key = pqrs::json::find<std::string>(json, "key");
    if (key == "Elements") {
      if (auto value = pqrs::json::find_array(json, "value")) {
        for (const auto& dictionary : value->value()) {
          std::optional<int> usage_page;
          std::optional<int> usage;

          for (const auto& j : dictionary) {
            auto k = pqrs::json::find<std::string>(j, "key");
            auto v = pqrs::json::find<int>(j, "value");

            if (k == "UsagePage") {
              usage_page = v;
            }
            if (k == "Usage") {
              usage = v;
            }

            collect_usage(result, j);
          }

          if (usage_page && usage) {
            result[*usage_page].insert(*usage);
          }
        }
      }
    }
  } else if (json.is_array()) {
    for (const auto& j : json) {
      collect_usage(result, j);
    }
  }
}
#endif

void inspect(pqrs::osx::iokit_registry_entry registry_entry) {
  if (registry_entry.get().conforms_to("IOHIDDevice")) {
    if (auto id = registry_entry.find_registry_entry_id()) {
      std::cout << "registry_entry_id: " << *id << std::endl;
    }

    if (auto properties = registry_entry.find_properties()) {
      auto properties_json = pqrs::cf::json::strip_cf_type_json(pqrs::cf::json::to_json(*properties));
      // auto properties_json = pqrs::cf::json::to_json(*properties);
      std::map<int, std::set<int>> usage_pages;

#if 0
      std::cout << std::setw(4) << properties_json << std::endl;
#else
      for (const auto& p : properties_json) {
        auto key = p["key"];
        auto value = p["value"];
        if (key == "Manufacturer" ||
            key == "Product") {
          std::cout << "    " << key << ": " << value << std::endl;
        }

        collect_usage(usage_pages, p);
      }

      for (const auto& [usage_page, usages] : usage_pages) {
        std::cout << "    "
                  << "usage_page:" << usage_page << " usages:[";

        int index = 0;
        for (const auto& usage : usages) {
          if (index > 0) {
            std::cout << ",";
          }
          ++index;

          std::cout << usage;
        }

        std::cout << "]" << std::endl;
      }
#endif
    }

    std::cout << std::endl;
  }

  auto child_iterator = registry_entry.get_child_iterator(kIOServicePlane);
  while (true) {
    auto next = child_iterator.next();
    if (!next) {
      break;
    }

    inspect(pqrs::osx::iokit_registry_entry(next));
  }
}
} // namespace

int main(void) {
  auto registry_entry = pqrs::osx::iokit_registry_entry::get_root_entry();
  inspect(registry_entry);

  return 0;
}
