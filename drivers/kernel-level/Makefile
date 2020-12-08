obj-m := mds2450_dht11.o mds2450_sg90.o mds2450_led_onboard.o \
		 mds2450_mg995.o mds2450_szh_rlbg_012.o \
		 mds2450_leadcool120.o mds2450_szh_ssbh_040.o \
		 mds2450_fc_28.o

ccflags-y := -Wall

KDIR := /work/linux
CDIR := /usr/local/arm/4.4.1/bin
CC := $(CDIR)/arm-none-linux-gnueabi-gcc
CF := -Wall -O0
MISC_SRC := mds2450_dht11.c
CHAR_SRC := mds2450_sg90.c mds2450_sg90.h \
		    mds2450_led_onboard.c mds2450_led_onboard.h
PWD := $(shell pwd)

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
#	make -C ./rtl8188C_8192C_usb_linux_v4.0.2_9000.20130911

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	make -C ./rtl8188C_8192C_usb_linux_v4.0.2_9000.20130911 clean

install:
	cp $(MISC_SRC) /work/linux/drivers/misc/
	cp $(CHAR_SRC) /work/linux/drivers/char/

deploy:
	cp -r *.ko /nfsroot/root
	cp -r ./rtl8188C_8192C_usb_linux_v4.0.2_9000.20130911/*.ko /nfsroot/root
	cp -r ./rtl8188C_8192C_usb_linux_v4.0.2_9000.20130911/rtl_init /nfsroot/root
