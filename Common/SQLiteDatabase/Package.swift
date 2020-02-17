// swift-tools-version:4.0
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

#if os(macOS) || os(iOS) || os(tvOS) || os(watchOS)
let package = Package(
    name: "SQLiteDatabase",
    products: [
        .library(name: "SQLiteDatabase", targets: ["SQLiteDatabase"]),
    ],
    dependencies: [
    ],
    targets: [
        .target(name: "SQLiteDatabase"),
    ]
)
#else
let package = Package(
    name: "SQLiteDatabase",
    products: [
        .library(name: "SQLiteDatabase", targets: ["SQLiteDatabase"]),
        ],
    dependencies: [
    ],
    targets: [
        .target(name: "SQLiteDatabase", dependencies: ["SQLite3"]),
        .target(name: "SQLite3"),
        ]
)
#endif

