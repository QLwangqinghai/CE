//
//  main.m
//  IOTest
//
//  Created by 王青海 on 2018/11/22.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

void printerror(int error) {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        if (errno == EAGAIN) {
            printf("EAGAIN");
        } else if (errno == EWOULDBLOCK) {
            printf("EWOULDBLOCK");
        } else {
            printf("EINTR");
        }
    } else if (errno == ECONNRESET) {
        printf("ECONNRESET");
    } else {
        printf("OTher %ld", error);
    }
}

void server(){
    int welcomeSocket, newSocket;
    char buffer[8192];
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    
    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    
    /*---- Bind the address struct to the socket ----*/
    bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
    /*---- Listen on the socket, with 5 max connection requests queued ----*/
    if(listen(welcomeSocket,5)==0)
        printf("Listening\n");
    else
        printf("Error\n");
    
    /*---- Accept call creates a new socket for the incoming connection ----*/
    addr_size = sizeof serverStorage;
    newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    
//    /*---- Send message to the socket of the incoming connection ----*/
//    send(newSocket,buffer,13,0);
    memset(buffer, 1, 8192);
    
    
    int flags = fcntl(newSocket, F_GETFL, 0); //获取文件的flags值。
    fcntl(newSocket, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式；
    
    
    while (1) {
        ssize_t sendResult = send(newSocket, buffer, 8192, MSG_DONTWAIT);
        printf("sendResult %lld\n", sendResult);
        if (sendResult <= 0) {
            printerror(errno);
            printf("\n");

            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                if (errno == EAGAIN) {
                    ssize_t aa = send(newSocket, buffer, 0, MSG_DONTWAIT);
                    printf("aa %lld\n", aa);

                }
                break;
            } else if (errno == ECONNRESET) {
                break;
            } else {
                break;
            }
        }
    }
    NSLog(@"=send-end");
    
    sleep(15);
    close(newSocket);
    
    NSLog(@"newSocket close");

}
void client(){
    int clientSocket;
    char buffer[8192];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    
    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    
    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    
    sleep(10);
    
    
    while (1) {
        ssize_t sendResult = recv(clientSocket, buffer, 8192, MSG_DONTWAIT);
        printf("readResult %lld\n", sendResult);
        if (sendResult <= 0) {
            printerror(errno);
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                
                if (errno == EAGAIN) {
                    ssize_t aa = recv(clientSocket, buffer, 0, MSG_DONTWAIT);
                    printf("rraa %lld\n", aa);
                    
                }
                
                break;
            } else if (errno == ECONNRESET) {
                break;
            } else {
                break;
            }
        }
    }
    
    NSLog(@"=read-end");
    
    
    
    while (1) {
        sleep(3);

        ssize_t r1 = recv(clientSocket, buffer, 8192, MSG_DONTWAIT);
        printf("r1 %lld\n", r1);
        if (r1 <= 0) {
            printerror(errno);
        }
    }
    
    NSLog(@"=read-end");
    
}


int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        

        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            server();
        });
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            client();
        });
        
        [[NSRunLoop currentRunLoop] run];
        
        
    }
    return 0;
}
