//
//  AppDelegate.m
//  Life
//
//  Created by vector on 2020/6/16.
//  Copyright © 2020 haoqi. All rights reserved.
//

#import "AppDelegate.h"
#include <sys/mman.h>


@interface AppDelegate ()

@end

@implementation AppDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        size_t size = PAGE_SIZE;

        NSMutableData * data = [NSMutableData dataWithLength:size];
        [data writeToFile:<#(nonnull NSString *)#> atomically:<#(BOOL)#>]
        assert(size > 0);
        mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, <#int#>, <#off_t#>)
    }
    return self;
}


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    return YES;
}


#pragma mark - UISceneSession lifecycle


- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}


@end
