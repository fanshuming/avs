.PHONY: clean
  
CURRENT_DIR = $(shell pwd)

#toolchain path
#CROSS_COMPILE=$(HOME)/openwrt/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/

CC = mipsel-openwrt-linux-gcc
CFLAGS = -I ./include
LDFLAGS = -lpthread

INSTALL=install
STRIP=mipsel-openwrt-linux-uclibc-strip

LDFLAGS_RD = -L $(CURRENT_DIR)/third_party/mips/alsa_lib -lasound 
CFLAGS_RD = -I $(CURRENT_DIR)/third_party/mips/alsa_lib/include -I $(CURRENT_DIR)/third_party/mips/alsa-utils/include

CFLAGS_ALEXA = -I $(CURRENT_DIR)/third_party/mips/libcurl/include/curl
LDFLAGS_ALEXA = -L $(CURRENT_DIR)/third_party/mips/libcurl/lib -lcurl -L $(CURRENT_DIR)/third_party/mips/libng/lib -lnghttp2 -lm -L $(CURRENT_DIR)/third_party/mips/zlib/lib/ -lz -L $(CURRENT_DIR)/third_party/mips/libssl102/lib -lcrypto -L $(CURRENT_DIR)/third_party/mips/libssl102/lib/ -lssl

LDFLAGS_MOSQ = -L $(CURRENT_DIR)/third_party/mips/mosquitto-1.4.10/lib -lmosquitto  -L ./lib -lcares


TARGET=alexa
BIN_TARGET=bin/alexa

OBJECTS := $(patsubst %.c,%.o,$(wildcard ./src/*.c))

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS_RD) $(LDFLAGS_ALEXA) $(LDFLAGS_MOSQ)

%.o : %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_RD) $(CFLAGS_ALEXA) $< -o $@


#alexa: src/main.c src/spim.c src/wakeup.c src/utils.c src/record.c src/alexa.c src/cJSON.c src/ssap_protocol.c src/sub_client.c src/pub_client_src.c src/client_shared.c src/spi_communicate.c src/crc.c src/http_avs.c
#	${CC} $^ ${CFLAGS} ${CFLAGS_RD} ${CFLAGS_ALEXA} ${LDFLAGS_ALEXA} ${LDFLAGS_RD} ${LDFLAGS_MOSQ} ${LDFLAGS} -o $@

install : $(TARGET)
	$(INSTALL) -d ./bin
	echo "$(INSTALL) -s --strip-program=${STRIP} $(TARGET) $(BIN_TARGET)"
	$(INSTALL) -s --strip-program=${STRIP} $(TARGET) $(BIN_TARGET)

uninstall :
	-rm -f bin/alexa

clean:
	@rm -f ./src/*.o $(TARGET) $(BIN_TARGET)
