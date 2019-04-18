//
//  AppDelegate.m
//  FileAtt
//
//  Created by vector on 2018/12/21.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    NSString * path = @"/Users/wangqinghai/work/rockgame/1.py";
    NSFileManager * fileManger = [NSFileManager defaultManager];
    NSDictionary *attributes = [fileManger attributesOfItemAtPath:path error:nil];
    
    NSLog(@"文件的属性：%@",attributes);
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
