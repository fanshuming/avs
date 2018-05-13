/**
 *****************************************************************************
 * @file:          sub_client.h
 * @author         Shuming Fan
 * @email          smfan@topqizhi.com
 * @version        V1.0.0
 * @data
 * @Brief          sequeue header file.
 ******************************************************************************
*/

#ifndef __SUB_CLIENT_THREAD_H__
#define __SUB_CLIENT_THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif/* __cplusplus */

void * mosq_sub_thread(void * status);

#ifdef __cplusplus
}
#endif/* __cplusplus */

#endif /* __SUB_CLIENT_THREAD_H__ */
