//
//  main.m
//  CEBaseApi
//
//  Created by vector on 2020/2/14.
//  Copyright © 2020 angfung. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "CEBaseApi.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>


static dispatch_source_t staticTimer = NULL;

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        void * api = CEApiCreate(1024);

        
        socklen_t slen = sizeof(struct sockaddr_in);

        struct sockaddr_in server_addr = {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8000);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        int val =1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val))<0) {
            perror("setsockopt()");
        }

        assert(fd >= 0);
        assert(bind(fd, (struct sockaddr *)&server_addr, slen) == 0);
        assert(listen(fd, 1024) == 0);
        
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            while (1) {
                socklen_t aslen = slen;
                struct sockaddr_in clientAddress = {};
                int newFd = accept(fd, (struct sockaddr *)&clientAddress, &aslen);
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    CEApiAddEvent(api, newFd);
                    CEApiUpdateReadEvent(api, newFd, true);
                    CEApiUpdateReadEvent(api, newFd, true);
                    CEApiUpdateReadEvent(api, newFd, false);
                    CEApiUpdateReadEvent(api, newFd, false);

                    CEApiUpdateWriteEvent(api, newFd, true);
                    CEApiRemoveEvent(api, newFd);
                });
            }
        });
        

        dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
        dispatch_source_set_timer(timer, DISPATCH_TIME_NOW, 10 * NSEC_PER_SEC, 1 * NSEC_PER_SEC);
        dispatch_source_set_event_handler(timer, ^{
            
        });
        dispatch_resume(timer);
        staticTimer = timer;
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}


