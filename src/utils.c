/**
  *****************************************************************************
  * @file:			utils.c
  * @author			Tian Feng
  * @email          fengtian@topqizhi.com
  * @version		V1.0.0
  * @data
  * @Brief
  ******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/stat.h>

#include "log.h"

#include <semaphore.h>
#include "utils.h"

static char sysTopic[40] = "system-";
static char tokenTopic[40] = "token-";
static char sofaTopic[40] = "emomo_sofa-";
static char macAddr[30] = {};

void sem_wakeup_init(void)
{
        sem_init(&sem_mic_wakeup, 0, 0);
}

void post_wakeup(void)
{
        LOGD("[SEMAPHORE]:post wake up\n");
        sem_post(&sem_mic_wakeup);
}

/**
 * @brief	get mac address
 * @param
 *
 * @return
 * @note@
 */
int get_mac_addr(char *mac_addr)
{
	struct ifreq ifr;
	struct ifconf ifc;
	char buf[2048];
	int success = 0;

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock == -1) {
		LOGE("socket error\n");
		return -1;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
		LOGE("ioctl error\n");
		return -1;
	}

	struct ifreq* it = ifc.ifc_req;
	const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
	char szMac[64];
	int count = 0;
	for (; it != end; ++it) {
		strcpy(ifr.ifr_name, it->ifr_name);
		if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
			if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
				if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
					count ++ ;
					unsigned char * ptr ;
					ptr = (unsigned char  *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
					snprintf(szMac,64,"%02X:%02X:%02X:%02X:%02X:%02X",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
					//printf("%d,Interface name : %s , Mac address : %s \n",count,ifr.ifr_name,szMac);
					if(strstr(ifr.ifr_name, "br-lan")) {
						if((NULL != mac_addr) && (NULL != szMac)) {
							memcpy(mac_addr, szMac, strlen(szMac) + 1);
							memcpy(macAddr, szMac, strlen(szMac) + 1);
							break;
						}
					}
				}
			}
		}else{
			LOGE("get mac info error\n");
			return -1;
		}
	}
}

char * get_system_mac_topic(void)
{
	uint8_t mac_addr[32];
	
	if(strstr(sysTopic, ":") != NULL)
	{	
		LOGD("get_system_mac_topic:%s\n",sysTopic);	
		return sysTopic;
	}else{
        	get_mac_addr(mac_addr);
		strcat(sysTopic, macAddr);

		LOGD("get_system_mac_topic:%s\n",sysTopic);	
		return sysTopic;
	}
}

char * get_token_topic(void)
{
        uint8_t mac_addr[32];

        if(strstr(tokenTopic, ":") != NULL)
        {
                LOGD("get_token_topic : %s\n",tokenTopic);
                return tokenTopic;
        }else{
                get_mac_addr(mac_addr);
                strcat(tokenTopic, macAddr);

                LOGD("get_token_topic : %s\n",tokenTopic);
                return tokenTopic;
        }
}

char * get_sofa_topic(void)
{
        uint8_t mac_addr[32];

        if(strstr(sofaTopic, ":") != NULL)
        {
                LOGD("get_sofa_topic : %s\n",sofaTopic);
                return sofaTopic;
        }else{
                get_mac_addr(mac_addr);
                strcat(sofaTopic, macAddr);

                LOGD("get_sofa_topic : %s\n",sofaTopic);
                return sofaTopic;
        }
}


#define NET_PORT 8080
#define NET_IP "120.27.138.117"

//获取联网状态
int net_check(void)
{
    int fd;
    int in_len=0;
    struct sockaddr_in servaddr;
    //char buf[128];

    in_len = sizeof(struct sockaddr_in);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("socket");
        return -1;
    }

    /*设置默认服务器的信息*/
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NET_PORT);
    servaddr.sin_addr.s_addr = inet_addr(NET_IP);
    memset(servaddr.sin_zero,0,sizeof(servaddr.sin_zero));

    /*connect 函数*/
    while(1) {
        if(connect(fd, (struct sockaddr* )&servaddr, in_len) < 0) {
            LOGD("not connect to internet!\n ");
            continue;
        } else {
            LOGD("=====connect ok!=====\n");
            close(fd);
            return 1;
        }
    }
}

/**
 * @brief get file total length
 * @param[in]  file path
 * @param[out] file size
 */
uint32_t get_file_size(const char *path)
{
    uint32_t filesize = 0;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0) {
        LOGE("stat error\n");
        return filesize;
    } else {
        filesize = statbuff.st_size;
    }
    return filesize;
}
