#include <stdio.h>

#ifndef __LOG_ALEXA_H__
#define __LOG_ALEXA_H_

#define __WARNNING_LOG__
#define __DEBUG_LOG__
#define __ERROR_LOG__

#ifdef __DEBUG_LOG__
#define LOGD(format,...) printf("[D]"__FILE__" %s [line:%d]: "format"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGD(format,...)
#endif

#ifdef __ERROR_LOG__
#define LOGE(format,...) printf("[E]"__FILE__" %s [line:%d]: "format"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGE(format,...)
#endif

#ifdef __WARNNING_LOG__
#define LOGW(format,...) printf("[W]"__FILE__" %s [line:%d]: "format"",__FUNCTION__,  __LINE__, ##__VA_ARGS__)
#else
#define LOGW(format,...)
#endif

#endif
