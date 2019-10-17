//
//  ChooseButton.m
//  MyButton
//
//  Created by vector on 2019/10/17.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ChooseButton.h"

@interface ChooseButton ()

@property (nonatomic, strong, nullable) UIImage * normalImage;
@property (nonatomic, strong, nullable) UIImage * choosedImage;
@property (nonatomic, strong, nullable) UIImage * normalAndDisableImage;
@property (nonatomic, strong, nullable) UIImage * choosedAndDisableImage;

@end
@implementation ChooseButton

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.exclusiveTouch = true;
        [self refreshStatus];        
        self.minSize = CGSizeMake(24, 24);
    }
    return self;
}

- (void)handleClicked {
    if (self.onValidClick) {
        switch (self.chooseStatus) {
            case ChooseStatusNormal: {
                self.onValidClick(self, false);
            }
                break;
            case ChooseStatusChoosed: {
                self.onValidClick(self, true);
            }
                break;
            case ChooseStatusNormalAndDisable:
            case ChooseStatusChoosedAndDisable:
            default: {
                //无效点击
            }
                break;
        }
    }
}
- (void)setChooseStatus:(ChooseStatus)chooseStatus {
    if (_chooseStatus != chooseStatus) {
        _chooseStatus = chooseStatus;
        [self refreshStatus];
    }
}
- (void)refreshStatus {
    UIImage * icon = nil;
    switch (self.chooseStatus) {
        case ChooseStatusNormal: {
            icon = self.normalImage;
        }
            break;
        case ChooseStatusChoosed: {
            icon = self.choosedImage;
        }
            break;
        case ChooseStatusNormalAndDisable: {
            icon = self.normalAndDisableImage;
        }
            break;
        case ChooseStatusChoosedAndDisable: {
            icon = self.choosedAndDisableImage;
        }
            break;
        default:
            break;
    }
    self.image = icon;
}

- (void)setImage:(UIImage *)image forChooseState:(ChooseStatus)chooseStatus {
    switch (chooseStatus) {
        case ChooseStatusNormal: {
            self.normalImage = image;
        }
            break;
        case ChooseStatusChoosed: {
            self.choosedImage = image;
        }
            break;
        case ChooseStatusNormalAndDisable: {
            self.normalAndDisableImage = image;
        }
            break;
        case ChooseStatusChoosedAndDisable: {
            self.choosedAndDisableImage = image;
        }
            break;
        default: {
        }
            break;
    }
    [self refreshStatus];
}

@end
