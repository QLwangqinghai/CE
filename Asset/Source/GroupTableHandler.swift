//
//  GroupTableHandler.swift
//  Asset
//
//  Created by vector on 2019/10/19.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

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
    private let observerKey: String = UUID().uuidString
    public private(set) var dataProvider: AssetGroupDataProvider? {
        willSet {
            if let dataProvider = self.dataProvider {
                dataProvider.groupList.unobserveList(forKey: self.observerKey)
            }
        }
        didSet {
            if let dataProvider = self.dataProvider {
                dataProvider.groupList.observeList(didChange: {[weak self]  (groupList, changes) in
                    guard changes.count > 0 else {
                        return
                    }
                    guard let `self` = self else {
                          return
                    }
                    guard let dataProvider = self.dataProvider else {
                        return
                    }
                    guard dataProvider.groupList === groupList else {
                        return
                    }
                    self.items = groupList.list
                    self.tableView.beginUpdates()
                    for change in changes {
                        switch change {
                        case .insert(let items):
                            guard items.count > 0 else {
                                break
                            }
                            let indexPaths = items.map { (item) -> IndexPath in
                                return IndexPath(row: item.0, section: 0)
                            }
                            self.tableView.insertRows(at: indexPaths, with: .automatic)
                        case .remove(let items):
                            guard items.count > 0 else {
                                break
                            }
                            let indexPaths = items.map { (item) -> IndexPath in
                                return IndexPath(row: item.0, section: 0)
                            }
                            self.tableView.deleteRows(at: indexPaths, with: .automatic)
                        }
                    }
                    self.tableView.endUpdates()
                }, didReload: {[weak self]  (groupList, changes) in
                    guard changes.count > 0 else {
                        return
                    }
                    guard let `self` = self else {
                          return
                    }
                    guard let dataProvider = self.dataProvider else {
                        return
                    }
                    guard dataProvider.groupList === groupList else {
                        return
                    }
                    self.items = groupList.list
                    let indexPaths = changes.map { (change) -> IndexPath in
                        return IndexPath(row: change.0, section: 0)
                    }
                    self.tableView.beginUpdates()
                    self.tableView.reloadRows(at: indexPaths, with: .automatic)
                    self.tableView.endUpdates()
                }, didReloadAll: {[weak self]  (groupList) in
                    guard let `self` = self else {
                          return
                    }
                    guard let dataProvider = self.dataProvider else {
                        return
                    }
                    guard dataProvider.groupList === groupList else {
                        return
                    }
                    self.items = groupList.list

                    self.tableView.reloadData()
                }, forKey: self.observerKey)
                self.items = dataProvider.groupList.list
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
    deinit {
        self.dataProvider?.groupList.unobserveList(forKey: self.observerKey)
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








