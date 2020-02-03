//
//  CSTargetConditionals.h
//  CoreSystem
//
//  Created by vector on 2018/12/10.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef __CS_TARGET__
#define __CS_TARGET__


#if __APPLE__
#define CS_TARGET_OS_DARWIN       1
#define CS_TARGET_OS_LINUX        0
#define CS_TARGET_OS_WINDOWS      0
#define CS_TARGET_OS_BSD          0
#define CS_TARGET_OS_ANDROID      0
#define CS_TARGET_OS_CYGWIN       0
#elif __ANDROID__
#define CS_TARGET_OS_DARWIN       0
#define CS_TARGET_OS_LINUX        1
#define CS_TARGET_OS_WINDOWS      0
#define CS_TARGET_OS_BSD          0
#define CS_TARGET_OS_ANDROID      1
#define CS_TARGET_OS_CYGWIN       0
#elif __linux__
#define CS_TARGET_OS_DARWIN       0
#define CS_TARGET_OS_LINUX        1
#define CS_TARGET_OS_WINDOWS      0
#define CS_TARGET_OS_BSD          0
#define CS_TARGET_OS_ANDROID      0
#define CS_TARGET_OS_CYGWIN       0
#elif __CYGWIN__
#define CS_TARGET_OS_DARWIN       0
#define CS_TARGET_OS_LINUX        1
#define CS_TARGET_OS_WINDOWS      0
#define CS_TARGET_OS_BSD          0
#define CS_TARGET_OS_ANDROID      0
#define CS_TARGET_OS_CYGWIN       1
#elif _WIN32 || _WIN64
#define CS_TARGET_OS_DARWIN       0
#define CS_TARGET_OS_LINUX        0
#define CS_TARGET_OS_WINDOWS      1
#define CS_TARGET_OS_BSD          0
#define CS_TARGET_OS_ANDROID      0
#elif __unix__
#define CS_TARGET_OS_DARWIN       0
#define CS_TARGET_OS_LINUX        0
#define CS_TARGET_OS_WINDOWS      0
#define CS_TARGET_OS_BSD          1
#define CS_TARGET_OS_ANDROID      0
#else
#error unknown operating system
#endif

#define CS_TARGET_OS_WIN32        TARGET_OS_WINDOWS
#define CS_TARGET_OS_MAC          TARGET_OS_DARWIN
#define CS_TARGET_OS_OSX          TARGET_OS_DARWIN

#if __x86_64__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       1
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __arm64__ || __aarch64__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        1
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __mips64__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       1
#define CS_TARGET_CPU_S390X        0
#elif __powerpc64__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        1
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __i386__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          1
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __arm__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          1
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __mips__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         1
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __powerpc__
#define CS_TARGET_CPU_PPC          1
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        0
#elif __s390x__
#define CS_TARGET_CPU_PPC          0
#define CS_TARGET_CPU_PPC64        0
#define CS_TARGET_CPU_X86          0
#define CS_TARGET_CPU_X86_64       0
#define CS_TARGET_CPU_ARM          0
#define CS_TARGET_CPU_ARM64        0
#define CS_TARGET_CPU_MIPS         0
#define CS_TARGET_CPU_MIPS64       0
#define CS_TARGET_CPU_S390X        1
#else
#error unknown architecture
#endif

#if __LITTLE_ENDIAN__
#define CS_TARGET_RT_LITTLE_ENDIAN 1
#define CS_TARGET_RT_BIG_ENDIAN    0
#elif __BIG_ENDIAN__
#define CS_TARGET_RT_LITTLE_ENDIAN 0
#define CS_TARGET_RT_BIG_ENDIAN    1
#else
#error unknown endian
#endif

#if defined(__LP64__) && __LP64__
#define CS_TARGET_RT_64_BIT        1
#elif defined(__LLP64__) && __LLP64__
#define CS_TARGET_RT_64_BIT        1
#else
#define CS_TARGET_RT_64_BIT        0
#endif



#if defined(__cplusplus)
#define CS_EXTERN_C_BEGIN extern "C" {
#define CS_EXTERN_C_END   }
#else
#define CS_EXTERN_C_BEGIN
#define CS_EXTERN_C_END
#endif




#endif  /* __TARGETCONDITIONALS__ */
