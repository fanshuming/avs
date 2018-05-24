/**
 **************************************************************************************
 * @file    main.c
 * @brief   SPI Stream Audio Player (Master)
 *
 * @author  Tian Feng
 * @email   fengtian@topqizhi
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>

#include "spim.h"
#include "log.h"
#include "utils.h"
#include "wakeup.h"
#include "record.h"
#include "alexa.h"
#include "sub_client.h"

#define  VERSION "V1.0.0"
#define  MAX_CHANNEL_NUMBER 5
#define  MAX_THREAD_NUMBER 1


typedef enum thread_func_enum
{
	SPI_TOKEN_PID,
	WEB_DOWNLOAD_PID,
	DOWNLOAD_DATA_TO_SPIM_PID,
	SET_SOFA_COMMAND_PID,
	MICROPHONE_WAKEUP_PID,
	CHANGE_PLAY_SONG_MODE_PID,
	SPI_MP3_PID,
} thread_func_enum_t;

typedef struct thread_func_ops
{
	void (*thread_func)(void);
	pthread_t thread_id;
} thread_func_ops_t;


/* start here */
int main(int argc, char * argv[])
{
    	int ret;
	uint32_t tts_ret;
	pthread_t wakeup_thread_id;
	pthread_t record_thread_id;
	pthread_t upload_data_to_avs_thread_id;
	pthread_t mosq_sub_thread_id;

	spi_master_init();
  
        int fd[2];  
        //char readbuf[1024];  
  
        if(pipe(fd) < 0)  
        {  
                printf("pipe error!\n");  
        }  
  
        // read buf from child thread  
        //read( fd[0], readbuf, sizeof(readbuf) );  
        //printf("read buf = %s\n", readbuf); 

	sem_wakeup_init();
	sleep(2);

	//microphone_wakeup_thread();
	if(pthread_create(&wakeup_thread_id, NULL, microphone_wakeup_poll_thread, NULL) == -1) {
		LOGE("create wake up thread error\n");
	}

	//record_thread();
	if(pthread_create(&record_thread_id, NULL, record_thread, &fd[1]) == -1) {
		LOGE("create record thread error\n");
	}
	// upload data to avs thread

	if(pthread_create(&upload_data_to_avs_thread_id, NULL, upload_data_to_avs_thread, &fd[0]) == -1)
	{
		LOGE("create upload data to alexa thread error\n");
	}

	//mosq sub thread
	if(pthread_create(&mosq_sub_thread_id, NULL, mosq_sub_thread, NULL))
	{
		LOGE("create mosq sub thread error\n");
	}
	
	//wait wakeup thread
	if(pthread_join(wakeup_thread_id, NULL)) {
		LOGE("join wakeup thread  error\n");
	}
	//wait record thread
	if(pthread_join(record_thread_id, NULL))
	{
		LOGE("join record thread error\n");
	}
	//wait upload data to avs thread
	if(pthread_join(upload_data_to_avs_thread_id, NULL))
	{
		LOGE("join upload data to avs thread error\n");
	}
	//wait mosq sub thread
	if(pthread_join(mosq_sub_thread_id, NULL))
	{
		LOGE("join mosq sub thread error\n");
	}

	spi_master_deinit();

	return ret;
}

/* code end here */
