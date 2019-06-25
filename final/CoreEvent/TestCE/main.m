//
//  main.m
//  TestCE
//
//  Created by vector on 2019/6/14.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "TestCE-Swift.h"
#import <CoreEvent/CoreEvent.h>
//
//
//- (void) reportFromDelegate {
//    /*┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
//     │ the packet is trustworthy -- compute and store offset in 8-slot fifo ...                         │
//     └──────────────────────────────────────────────────────────────────────────────────────────────────┘*/
//
//    fifoQueue[fifoIndex++ % 8] = _offset;                           // store offset in seconds
//    fifoIndex %= 8;                                                 // rotate index in range
//
//    /*┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
//     │ look at the (up to eight) offsets in the fifo and and count 'good', 'fail' and 'not used yet'    │
//     └──────────────────────────────────────────────────────────────────────────────────────────────────┘*/
//    short           good = 0, fail = 0, none = 0;
//    _offset = 0.0;                                                  // reset for averaging
//
//    for (short i = 0; i < 8; i++) {
//        if (isnan(fifoQueue[i])) {                                  // fifo slot is unused
//            none++;
//            continue;
//        }
//        if (isinf(fifoQueue[i]) || fabs(fifoQueue[i]) < 0.0001) {   // server can't be trusted
//            fail++;
//            continue;
//        }
//
//        good++;
//        _offset += fifoQueue[i];                                    // accumulate good times
//    }
//
//    /*┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
//     │   .. if we have at least one 'good' server response or four or more 'fail' responses, we'll      │
//     │      inform our management accordingly.  If we have less than four 'fails' we won't make any     │
//     │      note of that ... we won't condemn a server until we get four 'fail' packets.                │
//     └──────────────────────────────────────────────────────────────────────────────────────────────────┘*/
//    double stdDev = 0.0;
//    if (good > 0 || fail > 3) {
//        _offset = _offset / good;                                   // average good times
//
//        for (short i = 0; i < 8; i++) {
//            if (isnan(fifoQueue[i])) continue;
//
//            if (isinf(fifoQueue[i]) || fabs(fifoQueue[i]) < 0.001) continue;
//
//            stdDev += (fifoQueue[i] - _offset) * (fifoQueue[i] - _offset);
//        }
//        stdDev = sqrt(stdDev/(float)good);
//
//        _trusty = (good+none > 4) &&                                // four or more 'fails'
//        (fabs(_offset) < .050 ||                         // s.d. < 50 mSec
//         (fabs(_offset) > 2.0 * stdDev));                 // s.d. < offset * 2
//
//        if (self.offsetChangedBlock) {
//            self.offsetChangedBlock(_offset, _trusty);
//        }
//
//        NTP_Logging(@"  [%@] {%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f} ↑=%i, ↓=%i, %3.1f(%3.1f) %@", _server,
//                    fifoQueue[0]*1000.0, fifoQueue[1]*1000.0, fifoQueue[2]*1000.0, fifoQueue[3]*1000.0,
//                    fifoQueue[4]*1000.0, fifoQueue[5]*1000.0, fifoQueue[6]*1000.0, fifoQueue[7]*1000.0,
//                    good, fail, _offset*1000.0, stdDev*1000.0, _trusty ? @"↑" : @"↓");
//    }
//
//    /*┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
//     │   .. if the association is providing times which don't vary much, we could increase its polling  │
//     │      interval.  In practice, once things settle down, the standard deviation on any time server  │
//     │      seems to fall in the 70-120mS range (plenty close for our work).  We usually pick up a few  │
//     │      stratum=1 servers, it would be a Good Thing to not hammer those so hard ...                 │
//     └──────────────────────────────────────────────────────────────────────────────────────────────────┘*/
//    if ((stratum == 1 && pollingIntervalIndex != 6) ||
//        (stratum == 2 && pollingIntervalIndex != 5)) {
//        pollingIntervalIndex = 7 - stratum;
//    }
//}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        
        [S main];
    }
    return 0;
}
