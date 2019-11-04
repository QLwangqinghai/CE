//
//  Collection.h
//  Collection
//
//  Created by vector on 2019/11/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

//! Project version number for Collection.
FOUNDATION_EXPORT double CollectionVersionNumber;

//! Project version string for Collection.
FOUNDATION_EXPORT const unsigned char CollectionVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <Collection/PublicHeader.h>


typedef struct CCIndexOrder {
    uintptr_t a;
    uintptr_t value;    
} CCIndexOrder_t;
