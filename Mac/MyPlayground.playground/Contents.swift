import Cocoa

var calendar = Calendar.init(identifier: Calendar.Identifier.gregorian)
let timeZone = TimeZone.init(identifier: "Asia/Shanghai")!
calendar.timeZone = timeZone

let date = Date()

let option: Set<Calendar.Component> = [.year, .month, .day, .hour, .minute]

let dateComponents: DateComponents = calendar.dateComponents(option, from: date)

print(dateComponents)

let year = dateComponents.year!
let month = dateComponents.month!
let day = dateComponents.day!
let hour = dateComponents.hour!
let minute = dateComponents.minute!

var time: Int64 = Int64(year)
print(dateComponents.day)
print(dateComponents.minute)


struct Record {
    enum Status: UInt8 {
        case join = 1
        case leave = 2
        case heart = 3
    }
    
    let time: Int64
    let status: Status
    
    
}
