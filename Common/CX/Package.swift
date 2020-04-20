// swift-tools-version:5.1

import PackageDescription

let package = Package(
	name: "CX",
    products: [
        .library(
            name: "CX",
            targets: ["CX"]
        )
    ],
    dependencies: [],
    targets: [
        .target(
            name: "CX",
            dependencies: [],
            path: "Sources/CX"
        )
    ],
    cLanguageStandard: .c11
)
