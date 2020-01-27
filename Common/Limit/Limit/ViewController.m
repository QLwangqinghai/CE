//
//  ViewController.m
//  Limit
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#import "ViewController.h"
#include <limits.h>
#include <sys/sysctl.h>
#include <fcntl.h>


@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.

    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE,&limit) != 0) {
        printf("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
        abort();
    }
    printf("max: %ld, current: %ld\n", limit.rlim_max, limit.rlim_cur);
//    limit.rlim_cur = limit.rlim_cur > 1024 * 1024 ? limit.rlim_cur : 1024 * 1024;
//    limit.rlim_cur = OPEN_MAX < limit.rlim_max ? OPEN_MAX : limit.rlim_max;
    
    
    int mib[2] = {CTL_KERN, KERN_MAXFILESPERPROC};
    int maxFilesPerProc = 0;
    size_t size = sizeof(maxFilesPerProc);

    if (sysctl(mib, 2, &maxFilesPerProc, &size, NULL, 0) != 0) {
        printf("sysctl KERN_MAXFILESPERPROC error %s; \n", strerror(errno));
        abort();
    }
    printf("maxFilesPerProc: %ld\n", maxFilesPerProc);

    
    
    if (limit.rlim_cur < maxFilesPerProc) {
        limit.rlim_cur = maxFilesPerProc;
        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
            printf("setrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
            abort();
        } else {
            printf("success to setrlimit rlim_cur %lld; \n", limit.rlim_cur);
        }
    }

}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
    
}


@end
