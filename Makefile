.PHONY: all clean
  
CROSS_COMPILE=/home/fanshuming/qz_proj/qz_mtk_openwrt/openwrt/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/
CC = mipsel-openwrt-linux-gcc
CFLAGS = -I ./include
LDFLAGS = -lpthread

INSTALL=install
STRIP=mipsel-openwrt-linux-uclibc-strip

LDFLAGS_RD = -L /home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/alsa_lib -lasound 
CFLAGS_RD = -I /home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/alsa_lib/include -I /home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/alsa-utils/include

CFLAGS_ALEXA = -I /home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/libcurl/include/curl
LDFLAGS_ALEXA = -L/home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/libcurl/lib -lcurl -L/home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/libng/lib -lnghttp2 -lm -L/home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/zlib/lib/ -lz -L/home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/libssl102/lib -lcrypto -L/home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/libssl102/lib/ -lssl

LDFLAGS_MOSQ = -L/home/fanshuming/alexa_c_realize/alexa_c_realize/third_party/mips/mosquitto-1.4.10/lib -lmosquitto


all: alexa

alexa: src/main.c src/spim.c src/wakeup.c src/utils.c src/record.c src/alexa.c src/cJSON.c src/ssap_protocol.c src/sub_client.c src/pub_client_src.c src/client_shared.c
	${CC} $^ ${CFLAGS} ${CFLAGS_RD} ${CFLAGS_ALEXA} ${LDFLAGS_ALEXA} ${LDFLAGS_RD} ${LDFLAGS_MOSQ} ${LDFLAGS} -o $@

install : all
	$(INSTALL) -d ./bin
	echo "$(INSTALL) -s --strip-program=${CROSS_COMPILE}${STRIP} alexa bin/alexa"
	$(INSTALL) -s --strip-program=${CROSS_COMPILE}${STRIP} alexa bin/alexa

uninstall :
	-rm -f bin/alexa

clean:
	rm -f alexa bin/alexa
