// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "FlexLayout",
  products: [
    .library(name: "FlexLayout", targets: ["FlexLayout"]),
    .library(name: "FlexLayoutPilates", targets: ["FlexLayoutPilates"]),
    .library(name: "FlexLayoutPilatesKit", targets: ["FlexLayoutPilatesKit"])
  ],
  dependencies: [
  ],
  targets: [
    .target(
      name: "FlexLayout",
      dependencies: ["FlexLayoutPilates", "FlexLayoutPilatesKit"],
      path: "Sources/Swift",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/pilates/"),
        .headerSearchPath("./")
      ],
      swiftSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ]
    ),
    .target(
      name: "FlexLayoutPilates",
      dependencies: [],
      path: "Sources/pilates",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/pilates/"),
        .headerSearchPath("./")
      ]
    ),
    .target(
      name: "FlexLayoutPilatesKit",
      dependencies: ["FlexLayoutPilates"],
      path: "Sources/PilatesKit",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/PilatesKit/"),
        .headerSearchPath("./")
      ]
    )
  ],
  cLanguageStandard: .gnu99,
  cxxLanguageStandard: .gnucxx11
)
