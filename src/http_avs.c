#include <stdio.h>
#include <string.h>
#include "curl.h"
#include "cJSON.h"
#include <stdlib.h>
#include "log.h"


size_t write_token_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    LOGD("write token data begin...\n");
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    fflush((FILE *)stream);
    return written;
}

void get_access_token_file_from_avs(void)
{
	const char * tokenUrl = "https://api.amazon.com/auth/o2/token";
	uint8_t *msg_send_buffer;
	char * returnBody;

	int result = 0;

	cJSON * json_msg;

	static FILE *saveBodyFile = NULL;
	saveBodyFile = fopen("token.txt",  "w+" );

	FILE *fp = fopen("/etc/config/token.json", "r");

        fseek(fp, 0, SEEK_END);
        long len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *data = (char*)malloc(len + 1);
        fread(data, 1, len, fp);
        fclose(fp);
        LOGD("the alexa config data: %s", data);
        //解析JSON数据
        cJSON *root_json = cJSON_Parse(data);    //将字符串解析成json结构体
        if (NULL == root_json)
        {
                LOGD("error:%s\n", cJSON_GetErrorPtr());
                cJSON_Delete(root_json);
		free(data);
                return;
        }

        json_msg = cJSON_CreateObject();

        cJSON *clinetID = cJSON_GetObjectItem(root_json, "client_id");
        cJSON *clientSecret = cJSON_GetObjectItem(root_json, "client_secret");
        cJSON *refreshToken = cJSON_GetObjectItem(root_json, "refresh_token");
        cJSON *grantType = cJSON_GetObjectItem(root_json, "grant_type");

        if((clinetID != NULL) && (clientSecret != NULL) && (refreshToken != NULL) && (grantType != NULL))
        {
                if((clinetID->valuestring != NULL) && (clientSecret->valuestring != NULL) && (refreshToken->valuestring != NULL) && (grantType->valuestring != NULL))
                {
                        cJSON_AddStringToObject(json_msg, "client_id", clinetID->valuestring);
                        cJSON_AddStringToObject(json_msg, "client_secret", clientSecret->valuestring);
                        cJSON_AddStringToObject(json_msg, "refresh_token", refreshToken->valuestring);
                        cJSON_AddStringToObject(json_msg, "grant_type", grantType->valuestring);
                }
              
        }else{
		LOGD("token config file fail\n");
        }

        //LOGD("json_msg:%s\n",json_msg);
        msg_send_buffer = cJSON_PrintUnformatted(json_msg);
        //LOGD("msg_send_buffer:%s\n",msg_send_buffer);


	CURL *m_curl = curl_easy_init();	

        /* set the opt */
	struct curl_slist *plist = curl_slist_append(NULL,"Content-Type:application/json;charset=UTF-8");  
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, plist); 

        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0 );
        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(m_curl, CURLOPT_CAINFO, "/tmp/cacert.pem");


	curl_easy_setopt(m_curl, CURLOPT_URL, tokenUrl);
	curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, msg_send_buffer);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_token_data);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, saveBodyFile);

	result = curl_easy_perform(m_curl);

	if (result != CURLE_OK){
		LOGE("result:%d\n",result);
	}
	fclose(fp);
}

get_token_from_file(char * token, const char * tokenFile)
{
		
	
        //FILE *fp = fopen("token.txt", "r");
        FILE *fp = fopen(tokenFile, "r");

        fseek(fp, 0, SEEK_END);
        long len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *data = (char*)malloc(len + 1);
        fread(data, 1, len, fp);
        fclose(fp);
        LOGD("the token data: %s", data);
        //解析JSON数据
        cJSON *root_json = cJSON_Parse(data);    //将字符串解析成json结构体
        if (NULL == root_json)
        {
                LOGE("error:%s\n", cJSON_GetErrorPtr());
                cJSON_Delete(root_json);
                free(data);
                return;
        }


        cJSON *refreshToken = cJSON_GetObjectItem(root_json, "refresh_token");
        cJSON *accessToken = cJSON_GetObjectItem(root_json, "access_token");

        if((accessToken != NULL) && (refreshToken != NULL))
        {
                if((accessToken->valuestring != NULL) && (refreshToken->valuestring != NULL))
                {
			//token = accessToken->valuestring;
			//printf("token length:%d\n",strlen(accessToken->valuestring));
			strcpy(token,accessToken->valuestring);
			//printf("\n\n%s\n",token);
		}
	}

	//fclose(fp);
}

/*
void main(void)
{
	get_access_token_file_from_avs();
	get_token_from_file("token.txt");
}
*/
