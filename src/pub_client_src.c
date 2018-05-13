/*
Copyright (c) 2009-2014 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Roger Light - initial implementation and documentation.
*/


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#include <winsock2.h>
#define snprintf sprintf_s
#endif

#include <mosquitto.h>
#include "client_shared.h"
#include "log.h"
#include "mqtt_protocol.h"
#include "cJSON.h"

#define STATUS_CONNECTING 0
#define STATUS_CONNACK_RECVD 1
#define STATUS_WAITING 2

/* Global variables for use in callbacks. See sub_client.c for an example of
 * using a struct to hold variables for use in callbacks. */
static char *topic = NULL;
static char *message = NULL;
static long msglen = 0;
static int qos = 0;
static int retain = 0;
static int mode = MSGMODE_NONE;
static int status = STATUS_CONNECTING;
static int mid_sent = 0;
static int last_mid = -1;
static int last_mid_sent = -1;
static bool connected = true;
static char *username = NULL;
static char *password = NULL;
static bool disconnect_sent = false;
static bool quiet = false;

static void my_connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	int rc = MOSQ_ERR_SUCCESS;

	//LOGD("my_connect_callback\n");
	if(!result){

	}else{
		if(result && !quiet){
			fprintf(stderr, "%s\n", mosquitto_connack_string(result));
		}
	}
}

static void my_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
	//LOGD("my_disconnect_callback\n");
}

static void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
	//LOGD("my_publish_callback\n");
	last_mid_sent = mid;
}

static void my_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str)
{
	LOGD("%s\n", str);
}

void mqtt_pub_json_msg(uint8_t *msg_topic, uint8_t *pub_msg_buffer)
{
    struct mosq_config cfg;
    struct mosquitto *mosq = NULL;
    int rc;
    int rc2;
    int argc=7;
    char *argv[7] = {
        "mosquto_pub",
        "-h",
        "120.27.138.117",
		"-t",
		"test_topic",
        "-m",
        "helloworld"
    };

    memset(&cfg, 0, sizeof(struct mosq_config));
    rc = client_config_load(&cfg, CLIENT_PUB, argc, argv);
    if(rc) {
        client_config_cleanup(&cfg);
        if(rc == 2){
            /* --help */
        }else{
            fprintf(stderr, "\nUse 'mosquitto_pub --help' to see usage.\n");
        }
        return ;
    }

    message = pub_msg_buffer;
    msglen = strlen(pub_msg_buffer);
    qos = cfg.qos;
    retain = cfg.retain;
    mode = MSGMODE_STDIN_LINE;//cfg.pub_mode;
    username = cfg.username;
    password = cfg.password;
    quiet = cfg.quiet;
	cfg.debug = 0;

    LOGD("message[%s], pub_mode[%d], quiet[%d], username[%s], password[%s]\n",
			message, mode, quiet, username, password);

    mosquitto_lib_init();

    if(client_id_generate(&cfg, "mosqpub")){
        return ;
    }
    mosq = mosquitto_new(cfg.id, true, NULL);
    if(!mosq){
        switch(errno){
            case ENOMEM:
                if(!quiet) fprintf(stderr, "Error: Out of memory.\n");
                break;
            case EINVAL:
                if(!quiet) fprintf(stderr, "Error: Invalid id.\n");
                break;
        }
        mosquitto_lib_cleanup();
        return ;
    }

    if(cfg.debug){
        mosquitto_log_callback_set(mosq, my_log_callback);
    }

#if 1
	/* when return back of this each action ,and will feedback this callback */
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_disconnect_callback_set(mosq, my_disconnect_callback);
    mosquitto_publish_callback_set(mosq, my_publish_callback);
#endif

    if(client_opts_set(mosq, &cfg)){
        return ;
    }

    rc = client_connect(mosq, &cfg);
    if(rc) {
		LOGE("client_connect errror\n");
		return ;//rc;
	}

    if(mode == MSGMODE_STDIN_LINE){
        mosquitto_loop_start(mosq);
    }

	rc2 = mosquitto_publish(mosq, &mid_sent, msg_topic, msglen, message, qos, retain);
	if(rc2){
	    if(!quiet) fprintf(stderr, "Error: Publish returned %d, disconnecting.\n", rc2);
	    mosquitto_disconnect(mosq);
	}

	mosquitto_disconnect(mosq);

    if(mode == MSGMODE_STDIN_LINE){
		//LOGD("mosquitto_loop_stop\n");
        mosquitto_loop_stop(mosq, false);
    }

    if(message && mode == MSGMODE_FILE){
		//LOGD("free message\n");
        free(message);
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    if(rc) {
        fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    }
    return ;//rc;
}

void mqtt_pub_msg_cjson(uint8_t *mqtt_topic, uint8_t *device_sn, uint8_t type, uint8_t *subtype, uint8_t *para)
//void mqtt_pub_msg_cjson(uint8_t *mqtt_topic, uint8_t type, uint8_t *subtype, uint8_t *para)
{
	uint8_t mosquitto_command[1024] = {0};
	uint8_t *msg_send_buffer;
	cJSON *json_msg;

	char * clientSecretStr = "c1be58b6a36d787935ae363e2368f8cff5ca84436cb7531b7a04ffdd2500ad90";
        //char * device_snStr = "123456";
        char * clientIdStr = "amzn1.application-oa2-client.b5989b4f46964b7d84d0f51d71c69bf1";
        char * productIdStr = "TopqizhiAlexa";
        char * access_tokenStr = "";
        char * refresh_tokenStr = "";




    	FILE *fp = fopen("/etc/config/alexa.json", "r");
    	fseek(fp, 0, SEEK_END);
    	long len = ftell(fp);
    	fseek(fp, 0, SEEK_SET);
    	char *data = (char*)malloc(len + 1);
    	fread(data, 1, len, fp);
    	fclose(fp);
    	printf("the alexa config data: %s", data);
    	//解析JSON数据
    	cJSON *root_json = cJSON_Parse(data);    //将字符串解析成json结构体
    	if (NULL == root_json)
    	{
        	printf("error:%s\n", cJSON_GetErrorPtr());
        	cJSON_Delete(root_json);
        	return;
    	}
    //"name":"EVDI"
	json_msg = cJSON_CreateObject();

    	cJSON *token_json = cJSON_GetObjectItem(root_json, "token");
    	cJSON *refreshToken_json = cJSON_GetObjectItem(root_json, "refreshToken");
	if((token_json != NULL) && (refreshToken_json != NULL))
	{
		printf("get token: %s \n", token_json);
		printf("get refresh token: %s\n",refreshToken_json);
		if((token_json->valuestring != NULL) && (refreshToken_json->valuestring != NULL))
		{
			cJSON_AddStringToObject(json_msg, "access_token", token_json->valuestring);
			cJSON_AddStringToObject(json_msg, "refresh_tokenStr", refreshToken_json->valuestring);
		}else{
			cJSON_AddStringToObject(json_msg, "access_token", "");
			cJSON_AddStringToObject(json_msg, "refresh_tokenStr", "");
		}
		
	}else{
		
		cJSON_AddStringToObject(json_msg, "access_token", "");
		cJSON_AddStringToObject(json_msg, "refresh_tokenStr", "");
	}
/*
    	if ((token_json != NULL) && (refreshToken_json !=NULL))
    	{
        	access_tokenStr = cJSON_Print(token_json);    //将JSON结构体打印到字符串中 需要自己释放
        	refresh_tokenStr = cJSON_Print(refreshToken_json);    //将JSON结构体打印到字符串中 需要自己释放
    	}
*/


	cJSON_AddStringToObject(json_msg, "sn", device_sn);
	cJSON_AddNumberToObject(json_msg, "type", type);
	cJSON_AddStringToObject(json_msg, "subtype", subtype);
	cJSON_AddStringToObject(json_msg, "language", para);
	cJSON_AddStringToObject(json_msg, "productId", productIdStr);
	cJSON_AddStringToObject(json_msg, "clientId", clientIdStr);
	cJSON_AddStringToObject(json_msg, "clientSecret", clientSecretStr);
	//cJSON_AddStringToObject(json_msg, "access_token", "123");
//	cJSON_AddItemToObject(json_msg,"token",token_json);
//	cJSON_AddItemToObject(json_msg,"refreshToken",refreshToken_json);
        

	printf("json_msg:%s\n",json_msg);        
	msg_send_buffer	= cJSON_PrintUnformatted(json_msg);

	LOGD("json msg buffer = %s\n", msg_send_buffer);

	mqtt_pub_json_msg(mqtt_topic, msg_send_buffer);

	if(json_msg != NULL) {
		LOGD("cJSON_Delete\n");
		cJSON_Delete(json_msg);
	}

	if(msg_send_buffer != NULL) {
		LOGD("free msg_send_buffer\n");
		free(msg_send_buffer);
	}
}

void send_net_connect_signal()
{
       char mac_addr[128];
       int i = 0;
       char * destStr = NULL;
       destStr = (char *)get_system_mac_topic();

       get_mac_addr(mac_addr);
       LOGD("get mac_addr=%s\n", mac_addr);
       LOGD("get system topic=%s\n", destStr);
       mqtt_pub_msg_cjson(destStr, mac_addr, 2, "connect", "alexa");
}

//int mqtt_pub_thread(void)
int mqtt_pub(void)
{
	uint32_t pub_count = 0;

	while(1) {
		pub_count++;
		printf("========================[%d]=================================\n", pub_count);
		mqtt_pub_msg_cjson("control", "12345678", SONG, SONG_PLAY, NULL);
		sleep(1);
		mqtt_pub_msg_cjson("system", "12345678", SONG, SONG_PLAY, NULL);
		sleep(1);
	}
}
