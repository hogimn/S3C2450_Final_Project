obj-m := mds2450_dht11.o mds2450_sg90.o mds2450_led_onboard.o
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

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean

install:
	cp $(MISC_SRC) /work/linux/drivers/misc/
	cp $(CHAR_SRC) /work/linux/drivers/char/

deploy:
	cp *.ko /nfsroot/root
