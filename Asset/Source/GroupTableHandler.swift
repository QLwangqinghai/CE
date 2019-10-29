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
//    public let imageView: UIImageView
    public override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
//        self.imageView = UIImageView()
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
    public private(set) var dataProvider: AssetDataProvider? {
        didSet {
            if let dataProvider = self.dataProvider {
                self.items = dataProvider.groupArray
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
        tableView.rowHeight = 62
        tableView.estimatedSectionHeaderHeight = 0
        tableView.estimatedSectionFooterHeight = 0
        tableView.register(GroupTableCell.classForCoder(), forCellReuseIdentifier: "GroupTableCell")
        self.tableView = tableView
        super.init()
        tableView.delegate = self
        tableView.dataSource = self
    }
    
    public func update(dataProvider: AssetDataProvider?) {
        if self.dataProvider != dataProvider {
            self.dataProvider = dataProvider
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
        cell.textLabel?.text = "\(assetGroup.title) \(assetGroup.count)"
        return cell
    }
}








