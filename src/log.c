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
#include <curl/curl.h>

#include "crc.h"
#include "ssap_protocol.h"
#include "string_convert.h"
#include "spim.h"
#include "queue.h"
#include "log.h"

#define LOG_FILE_LENTHG_MAX (1*1024)
#define LOG_FILE_NUMBER_MAX 1

void log_system_thread(void)
{
	time_t cur_t;
	struct tm* cur_tm;
	uint8_t log_file_name[32];
	uint8_t pre_log_file_name[32];
	uint32_t file_len = 0;
	uint8_t sys_cmd[128];

	if(!access("log", F_OK)) {
		system("rm -rf log");
	}

	LOGD("create log direct\n");
	system("mkdir log");

	time(&cur_t);
	cur_tm=gmtime(&cur_t);
	sprintf(log_file_name, "log/%d-%d-%d-%d-%d-%d.log",
		cur_tm->tm_year+1900, cur_tm->tm_mon+1, cur_tm->tm_mday, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
	printf("first_time_file_name=%s\n", log_file_name);

	if(freopen(log_file_name, "a+", stdout) == NULL) {
		fprintf(stderr, "error redirecting stdout\n");
	}

	while(1) {
		sleep(2);
		file_len = get_file_size(log_file_name);
		LOGD("file_len=%d\n", file_len);
		if(file_len > LOG_FILE_LENTHG_MAX) {
			file_len = 0;

			time(&cur_t);
			cur_tm=gmtime(&cur_t);

			memcpy(pre_log_file_name, log_file_name, strlen(log_file_name)+1);
			sprintf(log_file_name, "log/%d-%d-%d-%d-%d-%d.log",
				cur_tm->tm_year+1900, cur_tm->tm_mon+1, cur_tm->tm_mday, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
			printf("pre_log_file_name=%s, cur_time_file_name=%s\n", pre_log_file_name, log_file_name);

			if(freopen(log_file_name, "a+", stdout) == NULL) {
				fprintf(stderr, "error redirecting stdout\n");
			}

			fclose(pre_log_file_name);
			sprintf(sys_cmd, "rm %s", pre_log_file_name);
			system(sys_cmd);
			
		}
	}

}

/* log system control */
uint32_t log_system_init(void)
{
	uint32_t ret = 0;
	pthread_t log_check_system_id;

	if(pthread_create(&log_check_system_id, NULL, log_system_thread, NULL) == -1) {
		LOGE("log system thread create error\n");
		ret = 1;
	}

	if(pthread_detach(log_check_system_id)) {
		ret = 2;
		LOGE("detach log system thread\n");
	}

	return ret;
}

