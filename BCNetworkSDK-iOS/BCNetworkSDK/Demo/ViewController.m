//
//  ViewController.m
//  Demo
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "ViewController.h"
#import "BCTestRequest.h"
#import "BCNetworkCrypto.h"
#import "BCNetworkManager.h"
#import "BCEnvConfig.h"

static NSString * host = @"http://192.168.1.189:8888";
//static NSString * host = @"http://192.168.1.189:8085";

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    
    NSHomeDirectory();
//    {
//        NSString * s = @"xC8TcDy4D+1tnnTaI7huhcaJMf0ZLjuf0Al8mzQ92USaZdzs7SG0wQK6KNApbeDdjkM5P1KA9eoO\nPDJOrWrvLuLVGPlTC9o1f6gSaIEWTAI/0alRhJVOsBDsH7stnXs5ZBD8FaFp6FSCOiaVdCGO3TlB\n1x6GJiXhAv0uDmq/CARcBNPXr2axSB6lYo+jJqi9/J6OO1c3syLGytx3HWpIwg==";
//        NSData * d = [BCNetworkCrypto base64DecodeFromString:s];
//    }
//
//
//    NSData *jsonData = [@"123456789" dataUsingEncoding:NSUTF8StringEncoding];
//    uint32_t byte = [BCNetworkCrypto crc32:jsonData];
    
    

    
//    [[BCHTTPSessionManager defaultNetworkManager] POST:@"" parameters:nil progress:^(NSProgress * _Nonnull uploadProgress) {
//
//    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
//
//    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
//
//    }];
    
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleViewTapped:)];
    [self.view addGestureRecognizer:tap];
}

- (void)handleViewTapped:(UITapGestureRecognizer *)tap {
    NSString *developEnv = @"7170ca6691014fa4ad1567ac9fae6057";
    
    BCEnvConfig *config = [[BCEnvConfig alloc] initWithName:developEnv];
    config.host = host;
    config.seed = @"baf1285a98db46a4a8781da8b8ac2e41";
    
    [[BCNetworkManager sharedInstance] addEnvConfig:config];
    BCTestRequest *requester = [[BCTestRequest alloc] initWithEnvName:developEnv];
    requester.type = 1;
    requester.mobile = @"17610247557";
    [requester sendRequestWithSuccessBlock:^(id data) {
        NSLog(@"success: result: %@", data);
    } failureBlock:^(NSError *error) {
        NSLog(@"failure: %@", error);
    }];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    
    
    
    
}


@end
