//
//  GroupTableHandler.swift
//  Asset
//
//  Created by vector on 2019/10/19.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

open class GroupTableCell: UITableViewCell {
    public override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
    }
    required public init?(coder: NSCoder) {
        super.init(coder: coder)
    }
    
    
    
    
}

open class GroupTableHandler: NSObject, UITableViewDelegate, UITableViewDataSource {
    public let tableView: UITableView
    
    private var items: [AssetGroup] = []
    
    public private(set) var dataProvider: AssetGroupDataProvider? {
        willSet {
            if let dataProvider = self.dataProvider {
                let key = String(format: "%p", self)
                dataProvider.removeDataObserver(forKey: key)
            }
        }
        didSet {
            if let dataProvider = self.dataProvider {
                let key = String(format: "%p", self)
                let observer = AssetGroupDataProviderObserver(groupsDidReload: {[weak self] (provider) in
                    guard let `self` = self else {
                        return
                    }
                    guard self.dataProvider == provider else {
                        return
                    }
                    self.items = dataProvider.groups

                    self.tableView.reloadData()
                }) {[weak self] (provider, changes) in
                    guard let `self` = self else {
                        return
                    }
                    guard self.dataProvider == provider else {
                        return
                    }
                    self.items = dataProvider.groups

                    self.tableView.beginUpdates()
                    for change in changes {
                        let indexPaths = change.groups.map { (item) -> IndexPath in
                            return IndexPath(row: item.0, section: 0)
                        }
                        switch change.type {
                        case .insert:
                            self.tableView.insertRows(at: indexPaths, with: .automatic)
                            break
                        case .remove:
                            self.tableView.deleteRows(at: indexPaths, with: .automatic)
                            break
                        case .update:
                            self.tableView.reloadRows(at: indexPaths, with: .automatic)
                            break
                        }
                    }
                    self.tableView.endUpdates()
                }
                dataProvider.addDataObserver(observer, forKey: key)
                self.items = dataProvider.groups
                self.tableView.reloadData()
            } else {
                self.items = []
                self.tableView.reloadData()
            }
        }
    }
    
    public override init() {
        let tableView = UITableView(frame: UIScreen.main.bounds, style: .plain)
        tableView.separatorStyle = .singleLine
        tableView.estimatedRowHeight = 0
        tableView.estimatedSectionHeaderHeight = 0
        tableView.estimatedSectionFooterHeight = 0
        tableView.register(GroupTableCell.classForCoder(), forCellReuseIdentifier: "GroupTableCell")
        self.tableView = tableView
        super.init()
        tableView.delegate = self
        tableView.dataSource = self
    }
    
    public func update(dataProvider: AssetGroupDataProvider?) {
        if self.dataProvider != dataProvider {
            self.dataProvider = dataProvider
            self.tableView.reloadData()
        }
    }
    
    public func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        print("numberOfRowsInSection \(self.items.count)")
        return self.items.count
    }
    
    public func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        print("cellForRowAt \(indexPath)")
        let assetGroup = self.items[indexPath.row]
        guard let cell: GroupTableCell = tableView.dequeueReusableCell(withIdentifier: "GroupTableCell") as? GroupTableCell else {
            return UITableViewCell()
        }
        cell.textLabel?.text = assetGroup.title
        return cell
    }
    
}


open class MyTableHandler: NSObject, UITableViewDelegate, UITableViewDataSource {
    public let tableView: UITableView
    
    private var items: [String] = ["0", "1", "2", "3"]
        
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
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 2) {
            self.items = ["1", "2"]
            
            var ids: [Int] = [0, 3]
            let indexPaths = ids.map { (idx) -> IndexPath in
                return IndexPath(row: idx, section: 0)
            }
            self.tableView.beginUpdates()
            self.tableView.deleteRows(at: indexPaths, with: .automatic)
//            self.tableView.insertRows(at: indexPaths, with: .automatic)
            self.tableView.endUpdates()
        }
        self.tableView.reloadData()
        
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 4) {
            self.items = ["a", "1", "2", "b", "c", "d"]
            
            var ids: [Int] = [0, 3, 4, 5]
            let indexPaths = ids.map { (idx) -> IndexPath in
                return IndexPath(row: idx, section: 0)
            }
            self.tableView.beginUpdates()
//            self.tableView.deleteRows(at: indexPaths, with: .automatic)
            self.tableView.insertRows(at: indexPaths, with: .automatic)
            self.tableView.endUpdates()
        }
        self.tableView.reloadData()
        
        
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
