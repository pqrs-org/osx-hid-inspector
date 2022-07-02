#include <iomanip>
#include <pqrs/cf/json.hpp>
#include <pqrs/osx/iokit_registry_entry.hpp>

namespace {
void make_json(const pqrs::osx::iokit_registry_entry& registry_entry, nlohmann::json& output) {
  if (!registry_entry) {
    return;
  }

  if (registry_entry.get().conforms_to("IOHIDDevice")) {
    auto json = nlohmann::json::object();

    if (auto class_name = registry_entry.get().class_name()) {
      json["class_name"] = *class_name;
    }
    if (auto name_in_plane = registry_entry.find_name_in_plane(kIOServicePlane)) {
      json["name_in_plane"] = *name_in_plane;
    }
    if (auto location_in_plane = registry_entry.find_location_in_plane(kIOServicePlane)) {
      json["location_in_plane"] = *location_in_plane;
    }
    if (auto path = registry_entry.find_path(kIOServicePlane)) {
      json["path"] = *path;
    }
    if (auto properties = registry_entry.find_properties()) {
      json["properties"] = pqrs::cf::json::strip_cf_type_json(pqrs::cf::json::to_json(*properties),
                                                              pqrs::cf::json::strip_option::collapse_dictionary);
    }
    if (auto id = registry_entry.find_registry_entry_id()) {
      json["registry_entry_id"] = type_safe::get(*id);
    }

    output.push_back(json);
  }

  auto child_iterator = registry_entry.get_child_iterator(kIOServicePlane);
  while (true) {
    auto next = child_iterator.next();
    if (!next) {
      break;
    }

    make_json(pqrs::osx::iokit_registry_entry(next), output);
  }
}
} // namespace

void output_all_properties_json(void) {
  auto json = nlohmann::json::array();
  make_json(pqrs::osx::iokit_registry_entry::get_root_entry(), json);

  std::cout << std::setw(4) << json << std::endl;
}
