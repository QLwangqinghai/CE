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


open class GroupCell: UICollectionViewCell {
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

    public override init(frame: CGRect) {
        self.thumbnailImageView = ThumbnailImageView()
        self.titleLabel = UILabel()
        super.init(frame: frame)
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

open class GroupTableHandler: NSObject, UICollectionViewDelegateFlowLayout, UICollectionViewDataSource {
    public static let thumbnailOptions: ThumbnailOptions = ThumbnailOptions(size: GroupCell.Default.thumbnailImageSize, contentMode: .aspectFill)

    let cachingImageManager: CachingImageManager = CachingImageManager()
    
    public let collectionView: UICollectionView
    
    public private(set) var items: [Group] = []
    private let observerKey: String = UUID().uuidString
    public private(set) var dataProvider: GroupProvider? {
        didSet {
            if let dataProvider = self.dataProvider {
                self.items = dataProvider.groupArray
                self.collectionView.reloadData()
            } else {
                self.items = []
                self.collectionView.reloadData()
            }
        }
    }
    
    public override init() {
        let flowLayout = UICollectionViewFlowLayout()
        flowLayout.itemSize = CGSize(width: CGFloat.greatestFiniteMagnitude, height: 80.0);
        let collectionView = UICollectionView(frame: UIScreen.main.bounds, collectionViewLayout: flowLayout)
        collectionView.register(GroupCell.classForCoder(), forCellWithReuseIdentifier: "GroupCell")
        self.collectionView = collectionView
        super.init()
        collectionView.delegate = self
        collectionView.dataSource = self
    }
    
    public func update(dataProvider: GroupProvider?) {
        if self.dataProvider != dataProvider {
            self.dataProvider = dataProvider
        }
    }
    
    
    public func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return self.items.count
    }
    
    public func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let group = self.items[indexPath.row]
        guard let cell: GroupCell = collectionView.dequeueReusableCell(withReuseIdentifier: "GroupCell", for: indexPath) as? GroupCell else {
            return UICollectionViewCell()
        }
        cell.titleLabel.text = "\(group.title ?? "") (\(group.count))"
        let placeholder: UIImage? = nil
        
        self.setImage(thumbnailImageView: cell.thumbnailImageView, asset: group.lastAsset, placeholder: placeholder)
        return cell
    }

    public func setImage(thumbnailImageView: ThumbnailImageView, asset: PHAsset?, placeholder: UIImage?) {
        if let asset = asset {
            let option = GroupTableHandler.thumbnailOptions
            var changed = false
            if let old = thumbnailImageView.assetIdentifier {
                if old.identifier != asset.localIdentifier || old.options != option {
                    changed = true
                }
            } else {
                changed = true
            }
            if changed {
                let identifier = ThumbnailIdentifier(identifier: asset.localIdentifier, options: option)
                thumbnailImageView.assetIdentifier = identifier

                if let image = self.cachingImageManager.image(forKey: identifier) {
                    thumbnailImageView.image = image
                } else {
                    thumbnailImageView.image = placeholder
                    
                    self.cachingImageManager.requestThumbnailImage(asset: asset, options: option) { (_, image, error) in
                        if let e = error {
                            print(e)
                        }
                        if thumbnailImageView.assetIdentifier == identifier {
                            if let v = image {
                                thumbnailImageView.image = v
                            }
                        }
                    }
                }
            }
        } else {
            thumbnailImageView.assetIdentifier = nil
            thumbnailImageView.image = placeholder
        }
    }
}








