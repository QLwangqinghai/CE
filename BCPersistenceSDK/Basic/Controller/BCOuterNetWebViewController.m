//
//  BCOuterNetWebViewController.m
//  BichanWallet
//
//  Created by 王青海 on 2018/9/10.
//  Copyright © 2018年 陈少楠. All rights reserved.
//

#import "BCOuterNetWebViewController.h"
#import <JavaScriptCore/JavaScriptCore.h>


@interface BCBaseUIWebViewController ()
@property (nonatomic, weak) UIWebView *webView;

@end

@implementation BCBaseUIWebViewController

- (void)setURLRequest:(NSURLRequest *)URLRequest {
    _URLRequest = [URLRequest copy];
    
    if (self.webView) {
        [self p_loadURLRequest];
    }
}

- (void)p_loadURLRequest {

    if (self.URLRequest) {
        [self.webView loadRequest:self.URLRequest];//加载
    }
}

- (void)p_reloadURLRequest {
    if (self.URLRequest) {
        [self.webView reload];
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CGRect bounds = [[UIScreen mainScreen] applicationFrame];
    UIWebView* webView = [[UIWebView alloc]initWithFrame:bounds];
    [self.view addSubview:webView];
    self.webView = webView;
    webView.scalesPageToFit = YES;//自动对页面进行缩放以适应屏幕
    webView.dataDetectorTypes = UIDataDetectorTypeNone;
    webView.delegate = self;

    [self p_loadURLRequest];

//
    
}
-(void)webViewDidStartLoad:(UIWebView*)webView {
    NSLog(@"BCBaseUIWebViewController webViewDidStartLoad");
    [self fullLoading];

}
-(void)webViewDidFinishLoad:(UIWebView*)webView {
    [self cancelLoading];

    NSLog(@"BCBaseUIWebViewController webViewDidFinishLoad");
    // 还可以为js端提供完整的原生方法供其调用（记得导入#import <JavaScriptCore/JavaScriptCore.h>）
    JSContext *context = [webView valueForKeyPath:@"documentView.webView.mainFrame.javaScriptContext"];
    // 可以定义供js调用的方法, testMethod为js调用的方法名
    context[@"testMethod"] = ^() {
        dispatch_async(dispatch_get_main_queue(), ^{
            UIAlertView *alert = [[UIAlertView alloc]initWithTitle:@"" message:@"js调用方法" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil, nil];
            [alert show];
        });
    };
}
-(void)webView:(UIWebView*)webView DidFailLoadWithError:(NSError*)error {
    NSLog(@"BCBaseUIWebViewController DidFailLoadWithError: %@", error);
    
    [self cancelLoading];
    [self showErrorViewWithError:nil reloadAction:^BOOL(BCErrorView *netView, BCBaseUIWebViewController *controller) {
        [controller p_reloadURLRequest];
        return true;
    }];

}
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
    // 获取点击页面加载的url
//    NSString *url = request.URL.absoluteString;
//
//    if ([url rangeOfString:@"此处写想拦截的url字符串"].location != NSNotFound) {
//        // 通过获取当前点击页面加载的url与指定url进行比较，拦截页面请求，进行自己的逻辑处理
//        // 进行移动端的逻辑处理
//        UIAlertView *alert = [[UIAlertView alloc]initWithTitle:@"" message:@"拦截页面方法" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil, nil];
//        [alert show];
//
//        return NO;
//    }
    NSLog(@"BCBaseUIWebViewController shouldStartLoadWithRequest: %@", request);

    return YES;
}
@end



@interface BCOuterNetWebViewController ()

@end

@implementation BCOuterNetWebViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
