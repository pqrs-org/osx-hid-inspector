#include <ios>
#include <iostream>
#include <pqrs/osx/iokit_object_ptr.hpp>
#include <pqrs/osx/iokit_registry_entry.hpp>

namespace {
void inspect(pqrs::osx::iokit_registry_entry registry_entry) {
  if (registry_entry.get().conforms_to("IOHIDDevice")) {
    if (auto id = registry_entry.find_registry_entry_id()) {
      std::cout << "registry_entry_id: 0x" << std::hex << *id << std::dec << std::endl;
    }
    if (auto name = registry_entry.find_name_in_plane(kIOServicePlane)) {
      std::cout << "name_in_plane: " << *name << std::endl;
    }
    if (auto location = registry_entry.find_location_in_plane(kIOServicePlane)) {
      std::cout << "location_in_plane: " << *location << std::endl;
    }
    if (auto class_name = registry_entry.get().class_name()) {
      std::cout << "class_name: " << *class_name << std::endl;
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
