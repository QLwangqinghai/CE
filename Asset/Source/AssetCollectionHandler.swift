//
//  AssetCollectionHandler.swift
//  Asset
//
//  Created by vector on 2019/10/21.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

open class AssetCollectionCell: UICollectionViewCell {
    public override init(frame: CGRect) {
        super.init(frame: frame)
        
        
    }
    required public init?(coder: NSCoder) {
        super.init(coder: coder)
    }
}

//UITableViewDelegate, UITableViewDataSource
open class AssetCollectionHandler: NSObject {
    public let collectionView: UICollectionView = UICollectionView()
    
    private var items: [PHAsset] = []
    private let observerKey: String = UUID().uuidString

    public private(set) var dataProvider: AssetList? {
        willSet {
            if let dataProvider = self.dataProvider {
                let key = String(format: "%p", self)
                dataProvider.group.removeDataObserver(forKey: key)
            }
        }
        didSet {
            if let dataProvider = self.dataProvider {
                let key = self.observerKey
                let observer = AssetGroup.Observer.init(didReload: {[weak self] (provider) in
                    guard let `self` = self else {
                        return
                    }
                    guard self.dataProvider == provider else {
                        return
                    }
                    
                    
                }, didChange: {[weak self] (provider, changes) in
                    guard let `self` = self else {
                        return
                    }
                    guard self.dataProvider == provider else {
                        return
                    }
                    
                    
                }) {[weak self] (provider, groups) in
                    guard let `self` = self else {
                        return
                    }
                    guard self.dataProvider == provider else {
                        return
                    }
                    
                    
                }
//                let observer = Observer(didReload: {[weak self] (provider) in
//                    guard let `self` = self else {
//                        return
//                    }
//                    guard self.dataProvider == provider else {
//                        return
//                    }
//                    self.items = dataProvider.groups
//
//                    self.tableView.reloadData()
//                }) {[weak self] (provider, changes) in
//                    guard let `self` = self else {
//                        return
//                    }
//                    guard self.dataProvider == provider else {
//                        return
//                    }
//                    self.items = dataProvider.groups
//
//                    self.tableView.beginUpdates()
//                    for change in changes {
//                        let indexPaths = change.groups.map { (item) -> IndexPath in
//                            return IndexPath(row: item.0, section: 0)
//                        }
//                        switch change.type {
//                        case .insert:
//                            self.tableView.insertRows(at: indexPaths, with: .automatic)
//                            break
//                        case .remove:
//                            self.tableView.deleteRows(at: indexPaths, with: .automatic)
//                            break
//                        case .update:
//                            self.tableView.reloadRows(at: indexPaths, with: .automatic)
//                            break
//                        }
//                    }
//                    self.tableView.endUpdates()
//                }
//                dataProvider.addDataObserver(observer, forKey: key)
//                self.items = dataProvider.groups
//                self.tableView.reloadData()
            } else {
                self.items = []
                self.collectionView.reloadData()
            }
        }
    }
    
//    public override init() {
//        let tableView = UITableView(frame: UIScreen.main.bounds, style: .plain)
//        tableView.separatorStyle = .singleLine
//        tableView.estimatedRowHeight = 0
//        tableView.estimatedSectionHeaderHeight = 0
//        tableView.estimatedSectionFooterHeight = 0
//        tableView.register(GroupTableCell.classForCoder(), forCellReuseIdentifier: "GroupTableCell")
//        self.tableView = tableView
//        super.init()
//        tableView.delegate = self
//        tableView.dataSource = self
//    }
    
//    public func update(dataProvider: AssetGroupDataProvider?) {
//        if self.dataProvider != dataProvider {
//            self.dataProvider = dataProvider
//            self.tableView.reloadData()
//        }
//    }
//
//    public func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
//        print("numberOfRowsInSection \(self.items.count)")
//        return self.items.count
//    }
//
//    public func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
//        print("cellForRowAt \(indexPath)")
//        let assetGroup = self.items[indexPath.row]
//        guard let cell: GroupTableCell = tableView.dequeueReusableCell(withIdentifier: "GroupTableCell") as? GroupTableCell else {
//            return UITableViewCell()
//        }
//        cell.textLabel?.text = assetGroup.title
//        return cell
//    }

}
