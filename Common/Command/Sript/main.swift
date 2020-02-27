
import Foundation

class Execution {
    class func execute(path: String, arguments: [String]? = nil) -> (Int, Data) {
        let task = Process()
        let outpipe = Pipe()
        
        task.standardOutput = outpipe
        
        
        task.launchPath = path
        if let argu = arguments {
            task.arguments = argu
        }
        task.launch()
        task.waitUntilExit()
        
        let reader = outpipe.fileHandleForReading
        
        
        let data = reader.readDataToEndOfFile()
        return (Int(task.terminationStatus), data)

    }
    
    class func executeShell(cmd: String) -> Int {
        let task = Process()
        task.launchPath = "/bin/bash"
        
        task.arguments = ["-c", cmd]

        task.launch()
        task.waitUntilExit()
        
        
        return Int(task.terminationStatus)
    }
}



let currentTime = Date()
let projectName = "BichanWallet"

let fileManager: FileManager = FileManager.default

let currentDirectoryPath = fileManager.currentDirectoryPath
let projectInfoFilePath = currentDirectoryPath.appending("/BichanWallet.xcodeproj/project.pbxproj")
let infoPlistFilePath = currentDirectoryPath.appending("/BichanWallet/Info.plist")
let podfilePath = currentDirectoryPath.appending("/Podfile")
let paths = currentDirectoryPath.components(separatedBy: "/")

var tmpPaths = paths
tmpPaths.removeLast()

let homeDirectoryPath = tmpPaths.joined(separator: "/")
let cersDirectoryPath = homeDirectoryPath.appending("/cers")

tmpPaths.removeLast()
tmpPaths.append("BuildInfo-iOS")
let buildInfoDirectoryPath = tmpPaths.joined(separator: "/")
try? fileManager.createDirectory(at: URL(fileURLWithPath: buildInfoDirectoryPath), withIntermediateDirectories: true, attributes: nil)

let arguments = ProcessInfo().arguments
var subModules:[String:String] = [:]



var paramsBegin: Bool = false

let supportTypes: NSSet = NSSet(array: ["enterprise", "development"])
let supportConfigurations: NSSet = NSSet(array: ["Debug", "Release"])

class Params {
    var type: String = ""
    var configuration: String = ""
    var buildVersion: String = ""
    var version: String = ""
    var description: String = ""
    
    var bundleId: String = ""
    var developTeam: String = ""
    var teamId: String = ""
    var codeSignIdentity: String = ""
    var profileUUID: String = ""
    var profileSpecifier: String = ""
}

class MobileProvisionInfo {
    let uuid: String
    
    init(path: String) throws {
        let result = Execution.execute(path: "/usr/bin/security", arguments: ["cms" ,"-D", "-i", path])
        guard result.0 == 0 else {
            throw NSError(domain: "", code: -2, userInfo: nil)
        }
        
        let tmpDict = try PropertyListSerialization.propertyList(from: result.1, options: .mutableContainers, format: nil)
        
        guard let dict: NSDictionary = tmpDict as? NSDictionary else {
            throw NSError(domain: "", code: -3, userInfo: nil)
        }
        
        guard let uuid: String = dict["UUID"] as? String else {
            throw NSError(domain: "", code: -4, userInfo: nil)
        }
        self.uuid = uuid
    }
    
}


let params = Params()

for item in arguments {
    if paramsBegin {
        let argus = item.components(separatedBy: ":")
        
        guard argus.count == 2 else {
            exit(-1)
        }
        let key = argus[0]
        let value = argus[1]

        switch key {
        case "type":
            if !supportTypes.contains(value) {
                print("不支持的type: \(value)")
                exit(-1)
            }
            params.type = value
            break
        case "configuration":
            if !supportConfigurations.contains(value) {
                print("不支持的module: \(value)")
                exit(-1)
            }
            params.configuration = value
            break
        case "buildVersion":
            params.buildVersion = value
            break
        case "version":
            params.version = value
            break
        case "description":
            params.description = value
            break
        case "BCPersistenceSDK":
            fallthrough
        case "BCNetworkSDK":
            fallthrough
        case "BCUserCenterSDK":
            fallthrough
        case "BCWalletSDK":
            fallthrough
        case "BCCloudWalletSDK":
            fallthrough
        case "BichanWalletSDK":
            subModules[key] = value
        default:
            break
        }
    } else {
        if item == "--params" {
            paramsBegin = true
        }
    }
}
guard params.type != "" else {
    print("type 无效")
    exit(-1)
}
guard params.configuration != "" else {
    print("configuration 无效")
    exit(-1)
}
guard params.version != "" else {
    print("version 无效")
    exit(-1)
}

let projectInfoUrl = URL(fileURLWithPath: projectInfoFilePath)
guard let projectInfo = try? String(contentsOf: projectInfoUrl) else {
    exit(-1)
}

var projectInfoLines = projectInfo.components(separatedBy: "\n")

var mobileprovisionFilePath = cersDirectoryPath.appending("/")
switch params.type {
case "enterprise":
    params.bundleId = "com.wangqinghai.any"
    params.profileSpecifier = "any_Development"
    params.codeSignIdentity = "iPhone Developer: qinghai wang (8W5NBK7WA9)"
    params.developTeam = "J97FN9Y7WR"
    mobileprovisionFilePath += "Development_"
    break
case "development":
    params.bundleId = "com.bichan.bcwallet"
    params.profileSpecifier = "bcwallet_development"
    params.codeSignIdentity = "iPhone Developer: qinghai wang (9LK543CJ7M)"
    params.developTeam = "93H87X99T8"
    mobileprovisionFilePath += "Development_"
    break
default:
    print("不支持的type: \(params.type)")
    exit(-1)
    break
}

mobileprovisionFilePath += params.bundleId
mobileprovisionFilePath += ".mobileprovision"

let mobileProvisionInfo: MobileProvisionInfo = try! MobileProvisionInfo(path:mobileprovisionFilePath)
params.profileUUID = mobileProvisionInfo.uuid

if params.buildVersion == "" || params.buildVersion == "default" {
    params.buildVersion = ""
    let dateFormater: DateFormatter = DateFormatter()
    dateFormater.dateFormat = "yyMMdd"
    let time = dateFormater.string(from: currentTime)
    
    
    let buildVersionFilePath = buildInfoDirectoryPath.appending("/\(time)")
    let url = URL(fileURLWithPath: buildVersionFilePath)
    var version: Int = 1
    do {
        let buildVersionString = try String(contentsOf: url)
        
        if let versionValue: Int = Int(buildVersionString) {
            version = versionValue + 1
        }
    } catch {
        version += 1
    }
    params.buildVersion += "\(time).\(version)"
    
    let newData = "\(version)".data(using: String.Encoding.utf8)!
    try? newData.write(to: url)
}

if params.description == "" || params.description == "default" {

    var description: String = ""
    
    let dateFormater: DateFormatter = DateFormatter()
    dateFormater.dateFormat = "yyMMdd_hh_mm"
    description += dateFormater.string(from: currentTime)
    description += ","
    description += params.type
    description += ","
    description += params.configuration
    description += ","
    description += params.buildVersion
    params.description = description
}




let profileFileSavePath = fileManager.homeDirectoryForCurrentUser.path.appending("/Library/MobileDevice/Provisioning Profiles/\(mobileProvisionInfo.uuid).mobileprovision")

let pData: Data = try! Data(contentsOf: URL(fileURLWithPath: mobileprovisionFilePath))
try! pData.write(to: URL(fileURLWithPath: profileFileSavePath))


var podfileContent = """
platform :ios, '8.0'
source 'http://192.168.1.18/bichan_coms/pod_specs’
inhibit_all_warnings!

target 'BichanWallet' do
# Uncomment the next line if you're using Swift or would like to use dynamic frameworks
# use_frameworks!

# Pods for BichanWallet


### 币产子模块 - master版本控制

"""


let _ = subModules.map { (item) -> Void in
    podfileContent += " pod '\(item.0)', '\(item.1)'\n"
}

podfileContent += """

end
"""

guard let data = podfileContent.data(using: String.Encoding.utf8) else {
    exit(-1)
}

do {
    try data.write(to: URL(fileURLWithPath: podfilePath))
} catch let error {
    print("write podfile with error: \(error)")
    exit(-1)
}

func rewritePROVISIONING_PROFILE(lines: [String], uuid: String) -> [String] {
    return lines.map({ (line) -> String in
        
        if let range = line.range(of: "PROVISIONING_PROFILE = ") {
            var str = line.prefix(upTo: range.upperBound)
            str += "\"\(uuid)\";"
            return String(str)
        } else {
            return line
        }
    })
}

func rewritePROVISIONING_PROFILE_SPECIFIER(lines: [String], name: String) -> [String] {
    return lines.map({ (line) -> String in
        
        if let range = line.range(of: "PROVISIONING_PROFILE_SPECIFIER = ") {
            var str = line.prefix(upTo: range.upperBound)
            str += "\(name);"
            return String(str)
        } else {
            return line
        }
    })
}

func rewriteDEVELOPMENT_TEAM(lines: [String], team: String) -> [String] {
    return lines.map({ (line) -> String in
        
        if let range = line.range(of: "DEVELOPMENT_TEAM = ") {
            var str = line.prefix(upTo: range.upperBound)
            str += "\(team);"
            return String(str)
        } else {
            return line
        }
    })
}

func rewriteCODE_SIGN_IDENTITY(lines: [String], signIdentify: String) -> [String] {
    return lines.map({ (line) -> String in
        
        if let range = line.range(of: "CODE_SIGN_IDENTITY = ") {
            var str = line.prefix(upTo: range.upperBound)
            str += "\"\(signIdentify)\";"
            return String(str)
        } else {
            return line
        }
    })
}
func rewritePRODUCT_BUNDLE_IDENTIFIER(lines: [String], bundleId: String) -> [String] {
    return lines.map({ (line) -> String in
        
        if let range = line.range(of: "PRODUCT_BUNDLE_IDENTIFIER = ") {
            var str = line.prefix(upTo: range.upperBound)
            str += "\(bundleId);"
            return String(str)
        } else {
            return line
        }
    })
}

func updateInfoPlist(path: String, items: [String:String?]) throws {
    guard let infos: NSMutableDictionary = NSMutableDictionary(contentsOfFile: path) else {
        throw NSError(domain: "", code: -101, userInfo: nil)
    }
    _ = items.map { (item) -> Void in
        if let value = item.value {
            infos[item.key] = value
        } else {
            infos.removeObject(forKey: item.key)
        }
    }
    infos.write(toFile: path, atomically: true)
}



projectInfoLines = rewritePROVISIONING_PROFILE(lines: projectInfoLines, uuid: params.profileUUID)
projectInfoLines = rewritePROVISIONING_PROFILE_SPECIFIER(lines: projectInfoLines, name: params.profileSpecifier)

projectInfoLines = rewriteDEVELOPMENT_TEAM(lines: projectInfoLines, team: params.developTeam)
projectInfoLines = rewriteCODE_SIGN_IDENTITY(lines: projectInfoLines, signIdentify: params.codeSignIdentity)
projectInfoLines = rewritePRODUCT_BUNDLE_IDENTIFIER(lines: projectInfoLines, bundleId: params.bundleId)


let fianlProjectInfo = projectInfoLines.joined(separator: "\n")

try! fianlProjectInfo.write(to: projectInfoUrl, atomically: true, encoding: String.Encoding.utf8)

var updateItems: [String:String?] = [:]
updateItems["CFBundleVersion"] = params.buildVersion
updateItems["CFBundleShortVersionString"] = params.version

try! updateInfoPlist(path: infoPlistFilePath, items: updateItems)

let buildUploadResult = Execution.executeShell(cmd: "./build_upload.sh --fastlane_params \"type:\(params.type) configuration:\(params.configuration) ipaname:\(projectName)_\(params.configuration)_\(params.buildVersion) bundleId:\(params.bundleId) provisionName:\(params.profileSpecifier) description:\(params.description)\"")
guard buildUploadResult == 0 else {
    exit(-1)
}
print("Success")


String(format: "%04lld", 12)
