//
//  AppDelegate.m
//  TC
//
//  Created by vector on 2019/11/15.
//  Copyright © 2019 vector. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    void * p = malloc(0x40000000);
    printf("%p", p);
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
