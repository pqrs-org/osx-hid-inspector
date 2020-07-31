[![Build Status](https://github.com/pqrs-org/osx-hid-inspector/workflows/CI/badge.svg)](https://github.com/pqrs-org/osx-hid-inspector/actions)
[![License](https://img.shields.io/badge/license-Public%20Domain-blue.svg)](https://github.com/pqrs-org/osx-hid-inspector/blob/master/LICENSE.md)

# osx-hid-inspector

osx-hid-inspector is a command line tool for macOS for inspecting human input devices (HID).

## Supported systems

macOS Sierra (10.12) or later.

---

## Using a pre-built binary

1.  Download `dist/hid-inspector.dmg`.
2.  Open `hid-inspector.dmg`.
3.  Use `/Volumes/hid-inspector/bin/hid-inspector`.

## Building from source code

### Requirements

-   [XcodeGen](https://github.com/yonaskolb/XcodeGen)

### Steps

Execute make command on the terminal.

```shell
make
```

`src/build_xcode/build/Release/hid-inspector` is a built file.

---

## Usage

-   Show help

    ```shell
    hid-inspector --help
    ```

    -   Result:

        ```text
        Show details of human input devices.
        Usage:
        hid-inspector [OPTION...]

            --help  Print help.

        Output format options:
            --version                 Output version.
            --output-all-properties-json
                                        Output all properties in json format.
            --output-usages           Output usage pages and usages.

        Examples:
        hid-inspector --version
        hid-inspector --output-all-properties-json
        hid-inspector --output-usages
        ```

-   Show usage pages, usages of all human input devices

    ```shell
    hid-inspector --output-usages
    ```

    -   Result:

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
        ...
        ```

-   Show human input device properties in json

    ```shell
    hid-inspector --output-all-properties-json
    ```

    Note: This command outputs too much data, thus takes about 10 seconds.

    -   Result:

        ```text
        [
            {
                "class_name": "AppleUserHIDDevice",
                "name_in_plane": "AppleUserUSBHostHIDDevice",
                "path": "IOService:/AppleACPIPlatformExpert/...",
                "properties": {
                    "BootProtocol": 1,
                    "CFBundleIdentifier": "com.apple.AppleUserHIDDrivers",
                    "CFBundleIdentifierKernel": "com.apple.iokit.IOHIDFamily",
                    "CountryCode": 0,
                    "DebugState": {
                        "InputReportCount": 730767,
                        "InputReportTime": 4165427
                    },
                    "DeviceOpenedByEventSystem": true,
                    "DeviceUsagePairs": [
                        {
                            "DeviceUsage": 6,
                            "DeviceUsagePage": 1
                        }
                    ],
                    "Elements": [
                        ...
                    ],
                    ...
                }
            }
            ...
        ]
        ```
