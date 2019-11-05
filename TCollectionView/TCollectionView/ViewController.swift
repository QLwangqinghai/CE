//
//  ViewController.swift
//  TCollectionView
//
//  Created by vector on 2019/11/5.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UICollectionViewDelegateFlowLayout, UICollectionViewDataSource {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        let layout = UICollectionViewFlowLayout()
        let collectionView = UICollectionView(frame: self.view.bounds, collectionViewLayout: layout)
        
        self.view.addSubview(collectionView)
        collectionView.dataSource = self
        collectionView.delegate = self
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
            
    //        self.setImage(thumbnailImageView: cell.thumbnailImageView, asset: group.lastAsset, placeholder: placeholder)
            return cell
        }


}

