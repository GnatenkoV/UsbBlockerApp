struct BundleVersionData: Decodable {
    var CFBundleShortVersionString: String
    var CFBundleVersion: String
}

struct StagedBundleURLData: Decodable {
    var relative: String
}

struct ContainerData: Decodable {
    var bundlePath: String
}

struct ExtensionData: Decodable {
    var state: String
    var categories: [String]
    var originPath: String
    var bundleVersion: BundleVersionData?
    var identifier: String
    var stagedBundleURL: StagedBundleURLData?
    var container: ContainerData?
    var uniqueID: String
    var teamID: String
}

struct ExtensionsPropertyList: Decodable {
    var version: Int?
    var extensions: Array<ExtensionData>
    var developerMode: Bool?
    var extensionsPolicy: [String]?
    var bootUUID: String?
}
