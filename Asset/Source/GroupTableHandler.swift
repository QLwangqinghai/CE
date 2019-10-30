//
//  GroupTableHandler.swift
//  Asset
//
//  Created by vector on 2019/10/19.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit
import Photos

public class ThumbnailImageView: UIImageView {
    public var assetIdentifier: ThumbnailIdentifier?
    
}

open class GroupTableCell: UITableViewCell {
    public struct Default {
        public static let thumbnailImageSize: CGSize = CGSize(width: 56, height: 56)

        public static var titleColor: UIColor = {
            if #available(iOS 13, *) {
                return UIColor(dynamicProvider: { (collection) -> UIColor in
                    if collection.userInterfaceStyle == .dark {
                        return UIColor(white: 0.6, alpha: 1.0)
                    } else {
                        return UIColor(white: 0.4, alpha: 1.0)
                    }
                })
            } else {
                return UIColor(white: 0.4, alpha: 1.0)
            }
        }()
        public static var titleFont: UIFont = UIFont.boldSystemFont(ofSize: 16)
    }

    public let thumbnailImageView: ThumbnailImageView
    public let titleLabel: UILabel

    public override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        self.thumbnailImageView = ThumbnailImageView()
        self.titleLabel = UILabel()
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        self._compareBase()
    }
    required public init?(coder: NSCoder) {
        self.thumbnailImageView = ThumbnailImageView()
        self.titleLabel = UILabel()
        super.init(coder: coder)
        self._compareBase()
    }
    
    private func _compareBase() {
        self.contentView.addSubview(self.thumbnailImageView)
        self.thumbnailImageView.translatesAutoresizingMaskIntoConstraints = false
        self.thumbnailImageView.widthAnchor.constraint(equalToConstant: Default.thumbnailImageSize.width).isActive = true
        self.thumbnailImageView.heightAnchor.constraint(equalToConstant: Default.thumbnailImageSize.height).isActive = true
        self.thumbnailImageView.leftAnchor.constraint(equalTo: self.contentView.leftAnchor, constant: 16).isActive = true
        self.thumbnailImageView.centerYAnchor.constraint(equalTo: self.contentView.centerYAnchor).isActive = true
        
        self.titleLabel.numberOfLines = 1
        self.titleLabel.lineBreakMode = .byTruncatingMiddle
        self.titleLabel.font = Default.titleFont
        self.titleLabel.textColor = Default.titleColor

        self.contentView.addSubview(self.titleLabel)
        self.titleLabel.translatesAutoresizingMaskIntoConstraints = false
        self.titleLabel.leftAnchor.constraint(equalTo: self.contentView.leftAnchor, constant: 92).isActive = true
        self.titleLabel.rightAnchor.constraint(lessThanOrEqualTo: self.contentView.rightAnchor, constant: -16).isActive = true
        self.titleLabel.centerYAnchor.constraint(equalTo: self.contentView.centerYAnchor).isActive = true
        
    }
    public static var cellHeight: CGFloat = 60
}

open class GroupTableHandler: NSObject, UITableViewDelegate, UITableViewDataSource {
    
    public static let thumbnailOptions: ThumbnailOptions = ThumbnailOptions(size: GroupTableCell.Default.thumbnailImageSize, contentMode: .aspectFill)

    let cachingImageManager: CachingImageManager = CachingImageManager()
    
    public let tableView: UITableView
    
    public private(set) var items: [Group] = []
    private let observerKey: String = UUID().uuidString
    public private(set) var dataProvider: GroupProvider? {
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
        tableView.rowHeight = GroupTableCell.cellHeight
        tableView.estimatedSectionHeaderHeight = 0
        tableView.estimatedSectionFooterHeight = 0
        tableView.register(GroupTableCell.classForCoder(), forCellReuseIdentifier: "GroupTableCell")
        self.tableView = tableView
        super.init()
        tableView.delegate = self
        tableView.dataSource = self
    }
    
    public func update(dataProvider: GroupProvider?) {
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
        let group = self.items[indexPath.row]
        guard let cell: GroupTableCell = tableView.dequeueReusableCell(withIdentifier: "GroupTableCell") as? GroupTableCell else {
            return UITableViewCell()
        }
        cell.titleLabel.text = "\(group.title ?? "") (\(group.count))"
        let placeholder: UIImage? = nil
        if let asset = group.lastAsset {
            let option = GroupTableHandler.thumbnailOptions
            var changed = false
            if let old = cell.thumbnailImageView.assetIdentifier {
                if old.identifier != asset.localIdentifier || old.options != option {
                    changed = true
                }
            } else {
                changed = true
            }
            if changed {
                let identifier = ThumbnailIdentifier(identifier: asset.localIdentifier, options: option)
                cell.thumbnailImageView.assetIdentifier = identifier

                if let image = self.cachingImageManager.image(forKey: identifier) {
                    cell.thumbnailImageView.image = image
                } else {
                    cell.thumbnailImageView.image = placeholder
                    
                    self.cachingImageManager.requestThumbnailImage(asset: asset, options: option) { (_, image, error) in
                        if let e = error {
                            print(e)
                        }
                        if cell.thumbnailImageView.assetIdentifier == identifier {
                            if let v = image {
                                cell.thumbnailImageView.image = v
                            }
                        }
                    }
                    
                }
            }
        } else {
            cell.thumbnailImageView.assetIdentifier = nil
            cell.thumbnailImageView.image = placeholder
        }
        
        
        return cell
    }
}








