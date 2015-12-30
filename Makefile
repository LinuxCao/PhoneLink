##############################################
#
#	make CMD=yes
#
##############################################


GCC_PATH =/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-
INCLUDE_PATH = /usr/local/minigui

CC = $(GCC_PATH)gcc
LD = $(GCC_PATH)gcc

SRC_DIR = .
OBJ_DIR = ./obj
#ROOT := $(shell pwd)
#INCLUDE := $(ROOT)/include
INCLUDE := ./include

MKDIR = mkdir -p

PWD := $(shell pwd | sed 's,^\(.*/\)\?\([^/]*\),\2,')

CFLAGS = -c -I $(INCLUDE) -D__KEY_PHONE_LINK_CAO
LDFLAGS = -lpthread -lrt

EXTS := *.c
DIRS := ${shell find ${SRC_DIR} -type d -print}
SRCS := $(foreach dir,$(DIRS),$(wildcard $(addprefix $(dir)/,$(EXTS))))
OBJS := $(patsubst %c, $(OBJ_DIR)/%o, $(SRCS))
OBJS += ./lib/libbtclient.so

TARGET = phoneLink.bin
.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%o: %c
	@$(MKDIR) $(dir $@ hacks)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f  $(TARGET)
	rm -rf $(OBJ_DIR)

print:
	@echo SRCS: ${SRCS}	
	@echo OBJS: ${OBJS}

