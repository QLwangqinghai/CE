//
//  ViewController.m
//  Tranf
//
//  Created by vector on 2019/11/28.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"


@interface DrawingView : UIView

@end

@implementation DrawingView

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    NSLog(@"touches %@", NSStringFromCGPoint([touches.anyObject locationInView:self]));
}

@end

@interface DrawingContinerView : UIView
@property (nonatomic, strong) DrawingView * contentView;
@end

@implementation DrawingContinerView

- (void)updateFrame:(CGRect)frame {
    
    
    
    
}

@end

@interface ViewController ()
@property (nonatomic, strong) DrawingContinerView * contentView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    DrawingView * dView = [[DrawingView alloc] initWithFrame:CGRectMake(0, 0, 480, 270)];
    DrawingContinerView * container = [[DrawingContinerView alloc] initWithFrame:CGRectMake(0, 0, 480, 270)];
    container.contentView = dView;
    [container addSubview:dView];
    self.contentView = container;
    
    {
        UIButton * button = [UIButton buttonWithType:UIButtonTypeCustom];
        [button setTitle:@"+" forState:UIControlStateNormal];
        [self.view addSubview:button];
        [button.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor].active = true;
        [button.leftAnchor constraintEqualToAnchor:self.view.leftAnchor].active = true;
        [button.widthAnchor constraintEqualToConstant:100].active = true;
        [button.heightAnchor constraintEqualToConstant:30].active = true;
        [button addTarget:self action:@selector(add) forControlEvents:UIControlEventTouchUpInside];
    }
    {
        UIButton * button = [UIButton buttonWithType:UIButtonTypeCustom];
        [button setTitle:@"-" forState:UIControlStateNormal];
        [self.view addSubview:button];
        [button.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor].active = true;
        [button.rightAnchor constraintEqualToAnchor:self.view.rightAnchor].active = true;
        [button.widthAnchor constraintEqualToConstant:100].active = true;
        [button.heightAnchor constraintEqualToConstant:30].active = true;
        [button addTarget:self action:@selector(sub) forControlEvents:UIControlEventTouchUpInside];
    }
}

- (void)add {
    
}
- (void)sub {
    
}


@end
