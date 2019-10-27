//
//  ViewController.swift
//  TTable
//
//  Created by vector on 2019/10/24.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit



open class MyTableHandler: NSObject, UITableViewDelegate, UITableViewDataSource {
    public let tableView: UITableView
    private var items: [String] = ["0", "1", "2", "3"]
        
    func update(_ body: () -> Void) {
        self.tableView.performBatchUpdates({
            print("beginUpdates")
            body()
            print("endUpdates")
        }) { (result) in
            print("result\(result)")
        }
    }
    
    public override init() {
        let tableView = UITableView(frame: UIScreen.main.bounds, style: .plain)
        tableView.separatorStyle = .singleLine
        tableView.estimatedRowHeight = 0
        tableView.estimatedSectionHeaderHeight = 0
        tableView.estimatedSectionFooterHeight = 0
        tableView.register(UITableViewCell.classForCoder(), forCellReuseIdentifier: "UITableViewCell")
        self.tableView = tableView
        super.init()
        tableView.delegate = self
        tableView.dataSource = self
        self.tableView.reloadData()

        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 2) {
            do {
                self.items = ["1", "2"]
                
                let ids: [Int] = [0, 3]
                let indexPaths = ids.map { (idx) -> IndexPath in
                    return IndexPath(row: idx, section: 0)
                }
                self.update {
                    self.tableView.deleteRows(at: indexPaths, with: .automatic)
                }
            }
            
            do {
                self.items = ["a", "1", "2", "b", "c", "d"]

                let ids: [Int] = [0, 3, 4, 5]
                let indexPaths = ids.map { (idx) -> IndexPath in
                    return IndexPath(row: idx, section: 0)
                }
                self.update {
                    self.tableView.insertRows(at: indexPaths, with: .automatic)
                }
            }
        }
        
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 4) {
            self.items = ["a", "1", "2", "b", "c", "d"]
            
            let ids: [Int] = [0, 3, 4, 5]
            let indexPaths = ids.map { (idx) -> IndexPath in
                return IndexPath(row: idx, section: 0)
            }
            self.update {
                self.tableView.insertRows(at: indexPaths, with: .automatic)
            }
        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 6) {
            self.items = ["a", "b", "c", "d"]
            
            let ids: [Int] = [1, 2]
            let indexPaths = ids.map { (idx) -> IndexPath in
                return IndexPath(row: idx, section: 0)
            }
            
            self.items = ["a", "11", "22", "33", "b", "44", "c", "d", "55", "66"]
            let ids2: [Int] = [1, 2, 3, 5, 8, 9]
            let indexPaths2 = ids2.map { (idx) -> IndexPath in
                return IndexPath(row: idx, section: 0)
            }

            self.update {
                self.tableView.deleteRows(at: indexPaths, with: .automatic)
                self.tableView.insertRows(at: indexPaths2, with: .automatic)
            }
        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 8) {
            self.items = ["22", "33", "b", "44", "c", "d", "55", "66"]
            self.items = ["x", "y", "22", "33", "b", "44", "c", "d", "55", "66"]
            self.items = ["x", "y", "22", "33", "b", "44", "c", "d", "55", "66", "cc", "dd"]
            self.items = ["x", "y", "22", "33", "b", "44", "c", "d", "cc", "dd"]

            self.update {
                for idx in 0 ..< 2 {
                    self.tableView.deleteRows(at: [IndexPath(row: idx, section: 0)], with: .automatic)
                }
                for idx in 0 ..< 2 {
                    self.tableView.insertRows(at: [IndexPath(row: idx, section: 0)], with: .automatic)
                }
                self.tableView.insertRows(at: [IndexPath(row: 10, section: 0)], with: .automatic)
                self.tableView.insertRows(at: [IndexPath(row: 11, section: 0)], with: .automatic)

                self.tableView.deleteRows(at: [IndexPath(row: 8, section: 0)], with: .automatic)
                self.tableView.deleteRows(at: [IndexPath(row: 9, section: 0)], with: .automatic)
            }
        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 10) {
            self.items = ["1", "2", "3", "4"]

            self.update {
                for idx in 0 ..< 10 {
                    self.tableView.deleteRows(at: [IndexPath(row: idx, section: 0)], with: .automatic)
                }
                for idx in 0 ..< 4 {
                    self.tableView.insertRows(at: [IndexPath(row: idx, section: 0)], with: .automatic)
                }
                
                self.tableView.deleteRows(at: [IndexPath(row: 3, section: 0)], with: .automatic)
                self.tableView.insertRows(at: [IndexPath(row: 3, section: 0)], with: .automatic)
            }
        }
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 12) {
            self.items = ["a", "b", "c", "1", "2", "3", "4", "5"]
            self.update {
                self.tableView.insertRows(at: [IndexPath(row: 5, section: 0)], with: .automatic)
                for idx in 0 ..< 3 {
                    self.tableView.insertRows(at: [IndexPath(row: idx, section: 0)], with: .automatic)
                }
            }
        }
        
    }
    
    public func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        print("numberOfRowsInSection \(self.items.count)")
        return self.items.count
    }
    
    public func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        print("cellForRowAt \(indexPath)")
        let title = self.items[indexPath.row]
        guard let cell: UITableViewCell = tableView.dequeueReusableCell(withIdentifier: "UITableViewCell") else {
            return UITableViewCell()
        }
        cell.textLabel?.text = title
        return cell
    }
}

extension NSString: UniqueElement {
    public typealias UniqueIdentifier = String

    public var uniqueIdentifier: UniqueIdentifier {
        return self as String
    }

    public func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String) {}
    public func unobserveContent(forKey:String) {}
}

public class Section: UniqueElement, CustomDebugStringConvertible {
    public typealias UniqueIdentifier = String

    public var uniqueIdentifier: UniqueIdentifier {
        return self.identifier
    }

    public func observeContent(onChanged:(_ key: UniqueIdentifier) -> Void, forKey:String) {}
    public func unobserveContent(forKey:String) {}
    
    public let identifier: String
    public var date: Date
    public init(identifier: String, date: Date) {
        self.identifier = identifier
        self.date = date
    }
    
    public var debugDescription: String {
        return "(\(self.identifier), \(self.date))"
    }
    
}


open class OrderedListHandler: NSObject, UITableViewDelegate, UITableViewDataSource {
    public let tableView: UITableView
    private let orderedList: UniqueOrderedList<Int, Section> = UniqueOrderedList<Int, Section>(order: .descending)
    private var items: [Section] = []
    private var sectionMap: [String: Section] = [:]
    private let observerKey: String = UUID().uuidString

    private let dateFormatter: DateFormatter = {
        let result = DateFormatter()
        result.dateFormat = "HH:mm:ss SSS"
        return result
    }()
    
    func section(of: String, date: Date) -> Section {
        if let s = self.sectionMap[of] {
            s.date = date
            return s
        } else {
            let s = Section(identifier: of, date: date)
            self.sectionMap[of] = s
            return s
        }
    }
    
    public override init() {
        let tableView = UITableView(frame: UIScreen.main.bounds, style: .plain)
        tableView.separatorStyle = .singleLine
        tableView.estimatedRowHeight = 0
        tableView.estimatedSectionHeaderHeight = 0
        tableView.estimatedSectionFooterHeight = 0
        tableView.register(UITableViewCell.classForCoder(), forCellReuseIdentifier: "UITableViewCell")
        self.tableView = tableView
        super.init()
        tableView.delegate = self
        tableView.dataSource = self
        self.tableView.reloadData()

        self.orderedList.observeList(didChange: {[weak self] (list, changes) in
            guard changes.count > 0 else {
                return
            }
            guard let `self` = self else {
                  return
            }
            guard list === self.orderedList else {
                return
            }
            self.items = list.storage.load()
            
            print("begin-changes")
            self.tableView.performBatchUpdates({
                print("beginUpdates")
                
                for change in changes {
                    switch change {
                    case .insert(let items):
                        let log = items.map { (item) -> String in
                            return "(\(item.0): \(item.1.1))"
                        }
                        print("observeList callback insert \(log)")
                        guard items.count > 0 else {
                            break
                        }
                        let indexPaths = items.map { (item) -> IndexPath in
                            return IndexPath(row: item.0, section: 0)
                        }
                        self.tableView.insertRows(at: indexPaths, with: .automatic)
                    case .remove(let items):
                        let log = items.map { (item) -> String in
                            return "(\(item.0): \(item.1.1))"
                        }
                        print("observeList callback remove \(log)")
                        guard items.count > 0 else {
                            break
                        }
                        let indexPaths = items.map { (item) -> IndexPath in
                            return IndexPath(row: item.0, section: 0)
                        }
                        self.tableView.deleteRows(at: indexPaths, with: .automatic)
                    }
                }

                print("endUpdates")
            }) { (result) in
                print("performBatchUpdates result: \(result)")
            }

            print("end-changes")
        }, forKey: self.observerKey)
        
        
        var operations: [()->()] = []
        
        operations.append {
            self.orderedList.update { (updater) in
                var items: [Section] = []
                for _ in 0 ..< 30 {
                    
                    let section = self.section(of: "\(arc4random() % 50)", date: Date(timeInterval: TimeInterval(arc4random() % 200) * -1, since: Date()))
                    items.append(section)
                }
                updater.replace(items.map({ (section) -> (Int, Section) in
                    return (Int(section.date.timeIntervalSince1970 * 10000), section)
                }))
                print("1 - replace \(items)")
            }
        }

//        operations.append {
//            self.orderedList.update { (updater) in
//                var items: [Section] = []
//                for _ in 0 ..< 30 {
//                    let section = self.section(of: "\(arc4random() % 50)", date: Date(timeInterval: TimeInterval(arc4random() % 200) * -1, since: Date()))
//                    items.append(section)
//                }
//                updater.replace(items.map({ (section) -> (Int, Section) in
//                    return (Int(section.date.timeIntervalSince1970 * 1000), section)
//                }))
//                print("1 - replace \(items)")
//            }
//        }
        
        func aa() {
            self.orderedList.update { (updater) in
                var removed: Set<String> = []
                var items: [Section] = []

                let count = Int(sqrt(Double(arc4random() % 50)) - 1)
                if count > 0 {
                    for _ in 0 ..< count {
                        removed.insert("\(arc4random() % 50)")
                    }
                }
                
                
                let mcount = Int(sqrt(Double(arc4random() % 50)) - 1)
                if mcount > 0 {
                    for _ in 0 ..< mcount {
                        let section = self.section(of: "\(arc4random() % 50)", date: Date(timeInterval: TimeInterval(arc4random() % 10000) * -1 / 10000.0, since: Date()))
                        items.append(section)
                    }
                }
                
                updater.filter({ (_, section) -> Bool in
                    return !removed.contains(section.identifier)
                })
                updater.replace(items.map({ (section) -> (Int, Section) in
                    return (Int(section.date.timeIntervalSince1970 * 10000), section)
                }))
                print("aa - replace \(items)")
            }
        }
        
        for index in 0 ..< 20 {
            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + Double(3 * (index + 1))) {
                aa()
            }
        }
        
//
//        operations.append {
//            self.orderedList.update { (updater) in
//                updater.filter { (_, string) -> Bool in
//                    return string.integerValue % 3 != 0
//                }
//                print("3 delete all where order % 3 == 0")
//                var items: [NSString] = []
//                for _ in 0 ..< 50 {
//                    items.append(NSString(format: "%lu", arc4random() % 1000))
//                }
//                print("3 inserts \(items)")
//                updater.replace(items.map({ (string) -> (Int, NSString) in
//                    return (string.integerValue, string)
//                }))
//            }
//        }
        
        
        for (index, body) in operations.enumerated() {
            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + Double(2 * (index + 1))) {
                body()
            }
        }
    }
    
    public func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        print("numberOfRowsInSection \(self.items.count)")
        return self.items.count
    }
    
    public func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        print("cellForRowAt \(indexPath)")
        let section = self.items[indexPath.row]
        guard let cell: UITableViewCell = tableView.dequeueReusableCell(withIdentifier: "UITableViewCell") else {
            return UITableViewCell()
        }
        cell.textLabel?.text = "\(section.identifier) - \(self.dateFormatter.string(from: section.date))"
        return cell
    }
}



class ViewController: UIViewController {
    var test: MyTableHandler? = nil
    var test2: OrderedListHandler? = nil

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        
//        let test = MyTableHandler()
//        self.test = test
//        self.view.addSubview(test.tableView)
//        test.tableView.frame = self.view.bounds
        
        let test2 = OrderedListHandler()
        self.test2 = test2
        self.view.addSubview(test2.tableView)
        test2.tableView.frame = self.view.bounds
    }


}

