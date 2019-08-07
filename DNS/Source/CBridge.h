//
//  CBridge.h
//  DNS
//
//  Created by vector on 2019/8/7.
//  Copyright © 2019 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>

extern char * haddr;

void printLastError(void);

ssize_t insendto(int fd, const void * data, size_t length, int option, const struct sockaddr_in * inaddr, socklen_t sl);


NS_ASSUME_NONNULL_BEGIN

@interface CBridge : NSObject

@end

NS_ASSUME_NONNULL_END
