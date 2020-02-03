


#import <stdio.h>
#import <dispatch/dispatch.h>

int main(int argc, const char * argv[]) {
    printf("hello\n");
    
    dispatch_queue_t queue = dispatch_queue_create("q", NULL);
    dispatch_async(queue, ^{
        printf("async\n");
    });
    
    sleep(20);
    
    return 0;
}
