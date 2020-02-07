//
//  main.c
//  TC
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <unistd.h>
#define MAXPATH 4096

int main(int argc, const char * argv[]) {
    char path[MAXPATH] = {};
    ssize_t ret = readlink("/proc/self/exe", path, MAXPATH-1);
    printf("readlink result:%s\n", path);

    printf(strerror(errno));
    
    uint32_t size = MAXPATH;
    int res = _NSGetExecutablePath(path, &size);

    printf("_NSGetExecutablePath result:%s\n", path);

    
    return 0;
}

