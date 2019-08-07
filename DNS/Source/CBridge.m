//
//  CBridge.m
//  DNS
//
//  Created by vector on 2019/8/7.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "CBridge.h"

@import Darwin;

char * haddr1 = "223.5.5.5";
//char * haddr = "223.6.6.6";
char * haddr = "114.114.114.114";



void printLastError(void) {
    printf("%s\n", strerror(errno));
}


ssize_t insendto(int fd, const void * data, size_t length, int option, const struct sockaddr_in * inaddr, socklen_t sl) {
    return sendto(fd, data, length, option, inaddr, sl);
}



@implementation CBridge

@end
