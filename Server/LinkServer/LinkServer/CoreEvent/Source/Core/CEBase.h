//
//  CEBase.h
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef CEBase_h
#define CEBase_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "CEBaseType.h"

#pragma mark - time

CEMicrosecondTime CEGetCurrentTime(void);


#pragma mark - log


static const uint32_t CELogMaskVerbose = 1;
static const uint32_t CELogMaskDebug = 2;
static const uint32_t CELogMaskInfo = 4;
static const uint32_t CELogMaskWarning = 8;
static const uint32_t CELogMaskError = 16;

extern uint32_t CELogMask;

// level >= CELogVerboseLevel 时log
extern uint32_t CELogVerboseLevel;


#define CELogVerbosable 1
#define CELogDebugable 1
#define CELogInfoable 1
#define CELogWarningable 1
#define CELogErrorable 1


#if CELogVerbosable

#define CELogVerbose(format, ...) if (CELogMask & CELogMaskVerbose) {\
uint64_t __ce_log_time = CEGetCurrentTime(); \
fprintf(stdout, "%08llu.%03llu_%03llu: ", __ce_log_time / 1000000ull,  __ce_log_time % 1000000ull / 1000ull, __ce_log_time % 1000ull);                                          \
fprintf(stdout, format, ##__VA_ARGS__);                                               \
fprintf(stdout, "\n");                                          \
}

#define CELogVerbose2(level, format, ...) if ((CELogMask & CELogMaskVerbose) && level >= CELogVerboseLevel) {\
uint64_t __ce_log_time = CEGetCurrentTime(); \
fprintf(stdout, "%08llu.%03llu_%03llu: ", __ce_log_time / 1000000ull,  __ce_log_time % 1000000ull / 1000ull, __ce_log_time % 1000ull);                                          \
fprintf(stdout, format, ##__VA_ARGS__);                                               \
fprintf(stdout, "\n");                                          \
}


#else

#define CELogVerbose(format, ...) do {\
} while(0);

#endif

#if CELogDebugable

#define CELogDebug(format, ...) if (CELogMask & CELogMaskDebug) {\
uint64_t __ce_log_time = CEGetCurrentTime(); \
fprintf(stdout, "%08llu.%03llu_%03llu: ", __ce_log_time / 1000000ull,  __ce_log_time % 1000000ull / 1000ull, __ce_log_time % 1000ull);                                          \
fprintf(stdout, format, ##__VA_ARGS__);                                               \
fprintf(stdout, "\n");                                          \
}

#else

#define CELogDebug(format, ...) do {\
} while(0);

#endif


#if CELogInfoable

#define CELogInfo(format, ...) if (CELogMask & CELogMaskInfo) {\
uint64_t __ce_log_time = CEGetCurrentTime(); \
fprintf(stdout, "%08llu.%03llu_%03llu: ", __ce_log_time / 1000000ull,  __ce_log_time % 1000000ull / 1000ull, __ce_log_time % 1000ull);                                          \
fprintf(stdout, format, ##__VA_ARGS__);                                               \
fprintf(stdout, "\n");                                          \
}

#else

#define CELogInfo(format, ...) do {\
} while(0);

#endif


#if CELogWarningable


#define CELogWarning(format, ...) if (CELogMask & CELogMaskWarning) {\
uint64_t __ce_log_time = CEGetCurrentTime(); \
fprintf(stdout, "%08llu.%03llu_%03llu: ", __ce_log_time / 1000000ull,  __ce_log_time % 1000000ull / 1000ull, __ce_log_time % 1000ull);                                          \
fprintf(stdout, format, ##__VA_ARGS__);                                               \
fprintf(stdout, "\n");                                          \
}

#else

#define CELogWarning(format, ...) do {\
} while(0);

#endif

#if CELogErrorable

#define CELogError(format, ...) if (CELogMask & CELogMaskError) {\
uint64_t __ce_log_time = CEGetCurrentTime(); \
fprintf(stderr, "%08llu.%03llu_%03llu: ", __ce_log_time / 1000000ull,  __ce_log_time % 1000000ull / 1000ull, __ce_log_time % 1000ull);                                          \
fprintf(stderr, format, ##__VA_ARGS__);                                               \
fprintf(stderr, "\n");                                          \
}

#else

#define CELogError(format, ...) do {\
} while(0);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* CEBase_h */
