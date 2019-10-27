//
//  ViewController.swift
//  Asset
//
//  Created by vector on 2019/10/16.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

class ViewController: UIViewController {
    var dataSource: AssetGroupDataProvider?



    var test2: OrderedListHandler? = nil

    
    let handler: GroupTableHandler = GroupTableHandler()
    
    var array: [Any] = []
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.view.backgroundColor = .red
        
//        dataSource = AssetGroupDataProvider()
        
        
        let label = UILabel()
        self.view.addSubview(label)
        
        let size0 = label.intrinsicContentSize
        label.text = "123141\n123"
        let size1 = label.intrinsicContentSize
        
        label.sizeToFit()
        
        let size2 = label.intrinsicContentSize

        print("\(size0)  \(size1)  \(size2)")
        
        
        let imageView: UIImageView = UIImageView()
        var image = UIImage(named: "edit_button_delete")
        
        print("\(imageView.intrinsicContentSize)")
        imageView.image = image
        print("\(imageView.intrinsicContentSize)")

        image = nil
        image = UIImage(named: "edit_button_delete.png")
        imageView.image = image
        print("\(imageView.intrinsicContentSize)")

        
        let button = UIButton(type: UIButton.ButtonType.custom)
        button.backgroundColor = .red
        self.view.addSubview(button)
        button.setTitle("ajklfjsa", for: .normal)
        
        button.frame = CGRect.init(x: 100, y: 100, width: 40, height: 10)
        
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 2) {
            PHPhotoLibrary.requestAuthorization { (status) in
                if status == .authorized {
                    self.dataSource = AssetGroupDataProvider()
                }
            }
        }
        self.view.addSubview(self.handler.tableView)
        self.handler.tableView.frame = self.view.bounds
        self.handler.update(dataProvider: AssetGroupDataProvider())

  
//        let test = MyTableHandler()
//        self.array.append(test)
//        self.view.addSubview(test.tableView)
//        test.tableView.frame = self.view.bounds
        
        let test2 = OrderedListHandler()
        self.test2 = test2
        self.view.addSubview(test2.tableView)
        test2.tableView.frame = self.view.bounds

        
    }
    
//    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        PHPhotoLibrary.requestAuthorization { (status) in
//            if status == .authorized {
//                AssetGroupDataProvider.test()
//            }
//        }
//    }
    
    
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
            
//            print("begin-changes")
            self.tableView.performBatchUpdates({
//                print("beginUpdates")
                
                for change in changes {
                    switch change {
                    case .insert(let items):
//                        print("observeList callback insert \(items)")
                        guard items.count > 0 else {
                            break
                        }
                        let indexPaths = items.map { (item) -> IndexPath in
                            return IndexPath(row: item.0, section: 0)
                        }
                        self.tableView.insertRows(at: indexPaths, with: .automatic)
                    case .remove(let items):
//                        print("observeList callback remove \(items)")
                        guard items.count > 0 else {
                            break
                        }
                        let indexPaths = items.map { (item) -> IndexPath in
                            return IndexPath(row: item.0, section: 0)
                        }
                        self.tableView.deleteRows(at: indexPaths, with: .automatic)
                    }
                }

//                print("endUpdates")
            }) { (result) in
//                print("performBatchUpdates result: \(result)")
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
                
                updater.replace(items.map({ (section) -> UniqueOrderedList<Int, Section>.Storage.Element in
                    return UniqueOrderedList<Int, Section>.Storage.Element(order: Int(section.date.timeIntervalSince1970 * 10000), value: section)
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
                
                updater.filter({ item -> Bool in
                    return !removed.contains(item.value.identifier)
                })

                updater.replace(items.map({ (section) -> UniqueOrderedList<Int, Section>.Storage.Element in
                    return UniqueOrderedList<Int, Section>.Storage.Element(order: Int(section.date.timeIntervalSince1970 * 10000), value: section)
                }))
                print("aa - replace \(items)")
            }
        }
        
        func bb() {
            self.orderedList.update { (updater) in
                var removed: Set<String> = []
                var items: [Section] = []

                for _ in 0 ..< 500 {
                    removed.insert("\(arc4random() % 3000)")
                }
                
                for _ in 0 ..< 1000 {
                    let section = self.section(of: "\(arc4random() % 3000)", date: Date(timeInterval: TimeInterval(arc4random() % 10000) * -1 / 10000.0, since: Date()))
                    items.append(section)
                }
                
                updater.filter({ item -> Bool in
                    return !removed.contains(item.value.identifier)
                })
                updater.replace(items.map({ (section) -> UniqueOrderedList<Int, Section>.Storage.Element in
                    return UniqueOrderedList<Int, Section>.Storage.Element(order: Int(section.date.timeIntervalSince1970 * 10000), value: section)
                }))
            }
        }
        
        for index in 0 ..< 20 {
            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + Double(3 * (index + 1))) {
                bb()
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
        return self.items.count
    }
    
    public func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let section = self.items[indexPath.row]
        guard let cell: UITableViewCell = tableView.dequeueReusableCell(withIdentifier: "UITableViewCell") else {
            return UITableViewCell()
        }
        cell.textLabel?.text = "\(section.identifier) - \(self.dateFormatter.string(from: section.date))"
        return cell
    }
}
