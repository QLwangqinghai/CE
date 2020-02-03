//
//  CSPosixHeader.h
//  CoreSystem
//
//  Created by vector on 2018/12/12.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CSPosixHeader_h
#define CSPosixHeader_h


//https://en.wikipedia.org/wiki/C_POSIX_library
//平台实现情况 https://blog.csdn.net/harkhuang/article/details/8104810
#pragma mark - 不在C语言标准库之内的POSIX标准的头文件

//v5 异步I/O
#include <aio.h>

//v6 IP 函数
#include <arpa/inet.h>

//v3 cpio文件格式
#include <cpio.h>

//v2 目录项
#include <dirent.h>

//v5 动态链接
#include <dlfcn.h>

//v1 文件控制
#include <fcntl.h>

//v4 消息显示结构
#include <fmtmsg.h>

//v4 文件名匹配类型
#include <fnmatch.h>

//v1 文件名遍历
#include <ftw.h>

//v4 路径名模式匹配类型
#include <glob.h>

//v1 用户Group identifier (Unix)信息与控制。
#include <grp.h>

//v4 字符编码转换
#include <iconv.h>

//v2 语言信息常量。建于Locale.h之上。
#include <langinfo.h>

//v4 路径名操作
#include <libgen.h>

//v4 货币单位的字符串格式化
#include <monetary.h>

//v5 消息队列
//#include <mqueue.h>

//v4 NDBM数据库操作
#include <ndbm.h>

//v6 本地网络接口列表
#include <net/if.h>

//v6 把本地协议与主机名翻译为数值地址。
#include <netdb.h>

//v6 定义互联网协议与地址族
#include <netinet/in.h>

//v6 额外的TCP的控制选项
#include <netinet/tcp.h>

//v2 本体化消息分类函数
#include <nl_types.h>

//v4 异步多任务文件描述符
#include <poll.h>

//v5 POSIX线程
#include <pthread.h>

//v1 passwd文件访问与控制
#include <pwd.h>

//v4 正则表达式
#include <regex.h>

//v5 执行调度
#include <sched.h>

// 搜索表
//未使用
//#include <search.h>

//v5 POSIX 信号量
#include <semaphore.h>

//v6 spawning子进程
#include <spawn.h>

//v4 大小写不敏感字符串比较
#include <strings.h>

//v4 流操作，包括ioctl
//mac不支持
//#include <stropts.h>

//v2 进程间通信 (IPC)
#include <sys/ipc.h>

//v4 内存管理，包括POSIX 共享内存 (进程间通信)与内存映射文件
#include <sys/mman.h>

//v2 POSIX 消息队列。
#include <sys/msg.h>

//v4 资源使用，优先级与限制
#include <sys/resource.h>

//v6 Select(Unix)
#include <sys/select.h>

//v2 XSI (SysV风格的) 信号量
#include <sys/sem.h>

//v2 XSI (SysV风格的)共享内存 (进程间通信)
#include <sys/shm.h>

//v6 socket
#include <sys/socket.h>

//v1 文件信息(stat (Unix)等)
//未使用
#include <sys/stat.h>

//v4 文件系统信息
//未使用
//#include <sys/statvfs.h>

//v4 时间与日期函数与结构
#include <sys/time.h>

//v1 clock_t 进程时间、系统时间等时间相关函数
#include <sys/times.h>

//v1 不同的数据类型
#include <sys/types.h>

//v4 向量I/O操作 readv和write函数用于在一次函数调用中读，写多个非连续缓冲区。
#include <sys/uio.h>

//v6 Unix域套接字
#include <sys/un.h>

//v1 操作系统信息，包括uname
#include <sys/utsname.h>

//v3 终止子进程的状态(见wait (Unix))
#include <sys/wait.h>

//v4 syslog系统日志
//未使用
//#include <syslog.h>

//v3 tar文件格式的Magic number
//未使用
//#include <tar.h>

//v3 允许串口界面
//未使用
//#include <termios.h>

//v6 运行时行为追踪(弃用)
//#include <trace.h>

//v1 资源限制（被<sys/resource.h>代替）
//#include <ulimit.h>

//v1 多种必要的POSIX函数与常量
#if __has_include(<unistd.h>)
#include <unistd.h>
#endif

//v3 inode访问与修改时间
//未使用
//#include <utime.h>

//v4 用户账号数据库函数
//未使用
//#include <utmpx.h>

//v4 子扩展，类似于shell被执行
//未使用
//#include <wordexp.h>


#endif /* CSPosixHeader_h */
