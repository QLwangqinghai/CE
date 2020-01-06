//
//  Memory.m
//  TD
//
//  Created by vector on 2020/1/6.
//  Copyright © 2020 angfung. All rights reserved.
//

#import "Memory.h"

/*
 
 X: compare 64MB Bytes 77194 微妙   1206微妙/MB
 
 */

@interface Memory ()

@end

@implementation Memory

+ (void)test {
    // Do any additional setup after loading the view.
    NSInteger count = (1024 * 1024 * 8);
    uint64_t * buffer = malloc(sizeof(uint64_t) * count);
    uint64_t * buffer1 = malloc(sizeof(uint64_t) * count);

    uint64_t * changes = malloc(sizeof(uint64_t) * count * 3);
    NSInteger offset = 0;
    
    NSTimeInterval b = CACurrentMediaTime();
    for (NSInteger i=0; i<count; i++) {
        if (buffer[i] != buffer1[i]) {
            changes[offset] = i;
            offset += 1;
            changes[offset] = buffer[i];
            offset += 1;
            changes[offset] = buffer1[i];
            offset += 1;
        }
    }
    NSTimeInterval e = CACurrentMediaTime();

    NSLog(@"%.06lf", e - b);
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
