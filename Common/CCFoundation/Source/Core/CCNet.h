//
//  CCNet.h
//  CoreEvent
//
//  Created by vector on 2019/5/5.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CCNet_h
#define CCNet_h


#if defined(__cplusplus)
extern "C" {
#endif


#include "CCBase.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>


/*

#include <net/if.h>

//指定网络接口名称字符串作为参数；若该接口存在，则返回相应的索引，否则返回0
unsigned if_nametoindex(const char *ifname);

//指定网络接口索引以及一块长度至少为IF_NAMESIZE(16)字节的内存区域作为参数；若索引对应的网络接口存在，则在内存区域中返回该接口的名称字符串，否则返回NULL，并将errno设置为相应的值
char *if_indextoname(unsigned ifindex, char *ifname);

//返回动态分配的struct if_nameindex结构数组，数组中的每一个元素分别对应一个本地网络接口；struct if_nameindex结构的if_index字段为接口索引，if_name字段为接口名称字符串；索引为0且名称字符串为NULL表示结构数组的末尾；调用出错时，返回NULL，并将errno设置为相应的值
struct if_nameindex *if_nameindex(void);

//通过if_nameindex()获取完毕接口名称与索引后，调用该函数以释放动态分配的内存区域。
void if_freenameindex(struct if_nameindex *ptr);
*/

typedef struct {
    CCUInt16 family;
    CCUInt16 port;//local的port， 不需要转换字节序

    CCUInt32 flowinfo;//ipv6有用 字段分为2个字段：低序20位是flow label，高序12位保留, IPV6报头中的通信流类别字段和流标签字段
    CCUInt32 scopeId;//ipv6有用 指定了使用哪个网络接口 eg: .scopeId = if_nametoindex("eth0")
    CCByte16 address;
} CCTcpSocketInfo;

//int CCSocketInfoGetLocal(int fd, CCSocketInfo * _Nonnull info);
//int CCSocketInfoGetPeer(int fd, CCSocketInfo * _Nonnull info);


//AddressFamily
typedef enum {
    CCSocketAddressLocal = PF_LOCAL,//Host-internal protocols, formerly called PF_UNIX
    CCSocketAddressInet = PF_INET,//Internet version 4 protocols
    CCSocketAddressRoute = PF_ROUTE,//Internal Routing protocol
    CCSocketAddressInet6 = PF_INET6,//Internet version 6 protocols
    CCSocketAddressKey = PF_KEY,//Internal key-management function
    CCSocketAddressSystem = PF_SYSTEM,//System domain
    CCSocketAddressNdrv = PF_NDRV,//Raw access to network device
} CCSocketAddress_e ;



typedef enum {
    CCSocketTypeStream = SOCK_STREAM,//tcp
    CCSocketTypeDatagram = SOCK_DGRAM,//udp
    CCSocketTypeRaw = SOCK_RAW,//The type SOCK_RAW, which is available only to the super-user.
} CCSocketType_e ;


/**
 * 创建套接字
 * addressFamily 地址协议， 以AF_ 开头
 * type 数据传输方式/套接字类型
 * protocol IPPROTO_ 开头， 定义在 <netinet/in.h>
 *  常见TCP: AF_INET+SOCK_STREAM+IPPROTO_TCP 或者 AF_INET6+SOCK_STREAM+IPPROTO_TCP
 * UDP: AF_INET+SOCK_DGRAM+IPPROTO_UDP 或者 AF_INET6+SOCK_DGRAM+IPPROTO_UDP
 *  LOCAL: AF_LOCAL+SOCK_STREAM+0          AF_LOCAL+SOCK_DGRAM+0
 * 返回值 != -1  为成功
 */
int CCSocketCreate(int addressFamily, int type, int protocol);


/**
 * 把一个本地协议地址赋予一个套接字。
 * socket 文件描述符
 * address 可以是 struct sockaddr_in, struct sockaddr_in6,  struct sockaddr_un
 * 返回值 ==0 成功， 非0时常见错误 EADDRINUSE（“Address already in use”地址已使用）
 */
int CCSocketBind(int socket, const struct sockaddr * _Nonnull address, socklen_t addressLength);

/**
* 监听链接
* socket 文件描述符
* backlog The maximum length for the queue of pending connections.
* 返回值 != -1 成功
*/
int CCSocketListen(int socket, int backlog);

/**
* 接受新的链接
* socket 文件描述符
* backlog The maximum length for the queue of pending connections.
* 返回值 != -1 成功, 非阻塞socket errno == EAGAIN || errno == EWOULDBLOCK 时， 没有新连接请求；
*/
int CCSocketAccept(int socket, struct sockaddr * _Nonnull address, socklen_t * _Nonnull addressLength);


/**
 * 获取socket本机地址
 * socket 文件描述符
 * address 可以是 struct sockaddr_in, struct sockaddr_in6,  struct sockaddr_un
 * 返回值 ==0 成功
 */
int CCSocketGetAddress(int socket, struct sockaddr * _Nonnull address, socklen_t * _Nonnull addressLength);

/**
* 获取socket对端地址
* socket 文件描述符
* address 可以是 struct sockaddr_in, struct sockaddr_in6,  struct sockaddr_un
* 返回值 ==0 成功
*/
int CCSocketGetPeerAddress(int socket, struct sockaddr * _Nonnull address, socklen_t * _Nonnull addressLength);


//int flags = fcntl(pio[0], F_GETFL, 0);
//int fresult = fcntl(pio[0], F_SETFL, flags | O_NONBLOCK);
//assert(fresult == 0);
//
//int flags1 = fcntl(pio[1], F_GETFL, 0);
//int fresult1 = fcntl(pio[1], F_SETFL, flags1 | O_NONBLOCK);
//assert(fresult1 == 0);


#if defined(__cplusplus)
}  // extern C
#endif

#endif /* CCNet_h */
