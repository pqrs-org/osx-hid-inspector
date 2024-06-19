[![Build Status](https://github.com/pqrs-org/osx-hid-inspector/workflows/CI/badge.svg)](https://github.com/pqrs-org/osx-hid-inspector/actions)
[![License](https://img.shields.io/badge/license-Public%20Domain-blue.svg)](https://github.com/pqrs-org/osx-hid-inspector/blob/main/LICENSE.md)

# osx-hid-inspector

osx-hid-inspector is a command line tool for macOS for inspecting human input devices (HID).

## Supported systems

macOS 13 Ventura or later.

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

`src/build/Release/hid-inspector` is a built file.

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
        registry_entry_id: 4294985173
            Manufacturer: "Contour Design"
            Product: "RollerMouse Mobile"
            Transport: "USB"
            VendorID: 2867
            ProductID: 12288
            DeviceUsagePairs: [{"DeviceUsage":2,"DeviceUsagePage":1},{"DeviceUsage":1,"DeviceUsagePage":1},{"DeviceUsage":6,"DeviceUsagePage":1},{"DeviceUsage":1,"DeviceUsagePage":12},{"DeviceUsage":48,"DeviceUsagePage":65280}]
            PrimaryUsagePage: 1
            PrimaryUsage: 2
            ==============================
            element_type: input_misc
                report_id: 1
                    --------------------------
                    usage_page: 1 (0x1)
                        usages: [
                            48 (0x30), 49 (0x31),
                            56 (0x38),
                        ] (3 entries)
                    --------------------------
                    usage_page: 12 (0xc)
                        usages: [
                            568 (0x238),
                        ] (1 entries)
                    --------------------------
                    usage_page: 65280 (0xff00)
                        usages: [
                            1 (0x1),
                        ] (1 entries)
                report_id: 2
                    --------------------------
                    usage_page: 65280 (0xff00)
                        usages: [
                            2 (0x2),
                        ] (1 entries)
                report_id: 3
                    --------------------------
                    usage_page: 65280 (0xff00)
                        usages: [
                            3 (0x3),
                        ] (1 entries)
                report_id: 5
                    --------------------------
                    usage_page: 65280 (0xff00)
                        usages: [
                            33 (0x21),
                        ] (1 entries)
            ==============================
            element_type: input_button
                report_id: 1
                    --------------------------
                    usage_page: 9 (0x9)
                        usages: [
                            1 (0x1), 2 (0x2), 3 (0x3), 4 (0x4), 5 (0x5), 6 (0x6), 7 (0x7), 8 (0x8),
                        ] (8 entries)
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
