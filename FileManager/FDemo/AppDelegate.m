//
//  AppDelegate.m
//  FDemo
//
//  Created by vector on 2019/7/22.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "AppDelegate.h"
#import <FileManager/FileManager.h>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application

    aa();
    
    NSString * libraryPath = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory,NSUserDomainMask,YES) firstObject];
    NSString * directoryPath = [libraryPath stringByAppendingPathComponent:@"recover"];

    
    
    NSData * data = [@"dasferf" dataUsingEncoding:NSUTF8StringEncoding];
    
    NSString * p = [libraryPath stringByAppendingPathComponent:@"adata"];
    [data  writeToFile:p atomically:true];

    NSLog(@"p : %@", p);

    NSError * e = nil;
    [[NSFileManager defaultManager] moveItemAtPath:p toPath:[directoryPath stringByAppendingPathComponent:@"bb"] error:&e];

    
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
