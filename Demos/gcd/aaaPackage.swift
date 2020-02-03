import PackageDescription

let package = Package(
    name: "G",
    targets: [
        Target(name: "G", dependencies: ["G"]),
    ]
)

// Ensure that the dynamic library is created for the performance test harness.
products.append(
    Product(name: "G", type: . Library(.Dynamic), modules: "SwiftProtobuf")
)
