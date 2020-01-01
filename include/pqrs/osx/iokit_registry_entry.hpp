#pragma once

// pqrs::osx::iokit_service v0.0

// (C) Copyright Takayama Fumihiko 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <optional>
#include <pqrs/osx/iokit_iterator.hpp>
#include <pqrs/osx/iokit_object_ptr.hpp>
#include <pqrs/osx/iokit_return.hpp>
#include <pqrs/osx/iokit_types.hpp>

namespace pqrs {
namespace osx {
class iokit_registry_entry final {
public:
  //
  // Constructors
  //

  iokit_registry_entry(void) : iokit_registry_entry(IO_OBJECT_NULL) {
  }

  explicit iokit_registry_entry(io_registry_entry_t registry_entry) : registry_entry_(registry_entry) {
  }

  explicit iokit_registry_entry(iokit_object_ptr registry_entry) : registry_entry_(registry_entry) {
  }

  static iokit_registry_entry get_root_entry(mach_port_t master_port = kIOMasterPortDefault) {
    return iokit_registry_entry(IORegistryGetRootEntry(master_port));
  }

  //
  // Methods
  //

  const iokit_object_ptr& get(void) const {
    return registry_entry_;
  }

  iokit_iterator get_child_iterator(const io_name_t plane) const {
    iokit_iterator result;

    if (registry_entry_) {
      io_iterator_t it = IO_OBJECT_NULL;
      iokit_return r = IORegistryEntryGetChildIterator(*registry_entry_, plane, &it);
      if (r) {
        result = iokit_iterator(it);
        IOObjectRelease(it);
      }
    }

    return result;
  }

  std::optional<iokit_registry_entry_id> find_registry_entry_id(void) const {
    if (registry_entry_) {
      uint64_t id;
      iokit_return r = IORegistryEntryGetRegistryEntryID(*registry_entry_, &id);
      if (r) {
        return iokit_registry_entry_id(id);
      }
    }
    return std::nullopt;
  }

  operator bool(void) const {
    return registry_entry_;
  }

private:
  iokit_object_ptr registry_entry_;
};
} // namespace osx
} // namespace pqrs
