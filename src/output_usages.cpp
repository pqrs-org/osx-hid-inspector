#include <iomanip>
#include <map>
#include <pqrs/cf/json.hpp>
#include <pqrs/osx/iokit_registry_entry.hpp>
#include <set>
#include <sstream>

namespace {
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

void output(pqrs::osx::iokit_registry_entry registry_entry) {
  if (registry_entry.get().conforms_to("IOHIDDevice")) {
    std::cout << "========================================" << std::endl;

    if (auto id = registry_entry.find_registry_entry_id()) {
      std::cout << "registry_entry_id: " << *id << std::endl;
    }

    if (auto properties = registry_entry.find_properties()) {
      auto properties_json = pqrs::cf::json::strip_cf_type_json(pqrs::cf::json::to_json(*properties));

      std::map<int, std::set<int>> usage_pages;

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
        std::cout << "    ------------------------------" << std::endl;
        std::cout << "    usage_page: " << usage_page << std::endl;
        std::cout << "        usages: [" << std::endl;

        // Join sequences

        std::vector<std::string> joined_usages;
        {
          std::optional<int> joined_first_usage;
          std::optional<int> last_usage;
          for (const auto& usage : usages) {
            if (joined_first_usage &&
                last_usage &&
                usage == *last_usage + 1 &&
                !joined_usages.empty()) {
              joined_usages.pop_back();

              if (usage - *joined_first_usage < 8) {
                std::stringstream ss;
                for (int i = *joined_first_usage; i <= usage; ++i) {
                  ss << std::to_string(i);
                  if (i != usage) {
                    ss << ", ";
                  }
                }
                joined_usages.push_back(ss.str());
              } else {
                joined_usages.push_back(std::to_string(*joined_first_usage) + " ... " + std::to_string(usage));
              }
            } else {
              joined_first_usage = usage;
              joined_usages.push_back(std::to_string(usage));
            }

            last_usage = usage;
          }
        }

        for (const auto& usage : joined_usages) {
          std::cout << "            " << usage << "," << std::endl;
        }

        std::cout << "        ]" << std::endl;
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
