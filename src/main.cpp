#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <pqrs/cf/json.hpp>
#include <pqrs/osx/iokit_object_ptr.hpp>
#include <pqrs/osx/iokit_registry_entry.hpp>
#include <set>

namespace {
void collect_usage(std::map<int, std::set<int>>& result, const nlohmann::json& json) {
  auto type = json["type"];
  auto value = json["value"];

  if (type == "dictionary") {
    std::optional<int> usage_page;
    std::optional<int> usage;

    for (const auto& j : value) {
      auto k = j["key"];
      auto v = j["value"];

      if (k["type"] == "string") {
        if (k["value"] == "UsagePage") {
          usage_page = v["value"].get<int>();
        }
        if (k["value"] == "Usage") {
          usage = v["value"].get<int>();
        }
      }

      collect_usage(result, v);
    }

    if (usage_page && usage) {
      result[*usage_page].insert(*usage);
    }
  } else if (type == "array" ||
             type == "dictionary" ||
             type == "set") {
    for (const auto& j : value) {
      collect_usage(result, j);
    }
  }
}

void inspect(pqrs::osx::iokit_registry_entry registry_entry) {
  if (registry_entry.get().conforms_to("IOHIDDevice")) {
    if (auto id = registry_entry.find_registry_entry_id()) {
      std::cout << "registry_entry_id: 0x" << std::hex << *id << std::dec << std::endl;
    }

    if (auto properties = registry_entry.find_properties()) {
      auto properties_json = pqrs::cf::cf_type_to_json(*properties);
      std::map<int, std::set<int>> usage_pages;

#if 0
      std::cout << std::setw(4) << properties_json << std::endl;
#else
      for (const auto& p : properties_json["value"]) {
        auto key = p["key"];
        auto value = p["value"];
        if (key["type"] == "string") {
          if (key["value"] == "Manufacturer" ||
              key["value"] == "Product") {
            std::cout << "    " << key["value"] << ": " << p["value"]["value"] << std::endl;
          }
        }

        collect_usage(usage_pages, p["value"]);
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
