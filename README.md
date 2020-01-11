[![Build Status](https://github.com/pqrs-org/osx-hid-inspector/workflows/CI/badge.svg)](https://github.com/pqrs-org/osx-hid-inspector/actions)
[![License](https://img.shields.io/badge/license-Public%20Domain-blue.svg)](https://github.com/pqrs-org/osx-hid-inspector/blob/master/LICENSE.md)

# osx-hid-inspector

osx-hid-inspector is a command line tool for macOS which shows human input devices (HID) status.

## Using a pre-built binary

1.  Download `dist/hid-inspector.dmg`.
2.  Open `hid-inspector.dmg`.
3.  Use `/Volumes/hid-inspector/bin/hid-inspector`.

## Usages

### Output usage pages, usages of all human input devices

```shell
hid-inspector --output-usages
```

#### Result

```text
registry_entry_id: 4294985109
    Manufacturer: "Apple Inc."
    Product: "Apple Internal Keyboard / Trackpad"
    DeviceUsagePairs: [{"DeviceUsage":11,"DeviceUsagePage":65280}]
    PrimaryUsagePage: 65280
    PrimaryUsage: 11
    ------------------------------
    usage_page: 0
        usages: [
            -1, 0,
        ] (2 entries)
    ------------------------------
    usage_page: 65280
        usages: [
            -1,
            11,
        ] (2 entries)

========================================
registry_entry_id: 4294985114
    Manufacturer: "Apple Inc."
    Product: "Apple Internal Keyboard / Trackpad"
    DeviceUsagePairs: [{"DeviceUsage":6,"DeviceUsagePage":1}]
    PrimaryUsagePage: 1
    PrimaryUsage: 6
    ------------------------------
    usage_page: 0
        usages: [
            -1, 0,
        ] (2 entries)
    ------------------------------
    usage_page: 1
        usages: [
            6,
        ] (1 entries)
    ------------------------------
    usage_page: 7
        usages: [
            -1 ... 255,
        ] (257 entries)
    ------------------------------
    usage_page: 8
        usages: [
            1, 2, 3, 4, 5,
        ] (5 entries)
    ------------------------------
    usage_page: 12
        usages: [
            184,
        ] (1 entries)
    ------------------------------
    usage_page: 255
        usages: [
            3,
        ] (1 entries)

...
```
