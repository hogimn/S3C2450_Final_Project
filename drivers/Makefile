obj-m := mds2450_dht11.o mds2450_sg90.o
ccflags-y := -Wall

APP := test_dht11 test_sg90
KDIR := /work/linux
CDIR := /usr/local/arm/4.4.1/bin
CC := $(CDIR)/arm-none-linux-gnueabi-gcc
CF := -Wall -O0

MISC_SRC := mds2450_dht11.c
CHAR_SRC := mds2450_sg90.c mds2450_sg90.h

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	$(CC) $(CF) -o test_dht11 test_dht11.c
	$(CC) $(CF) -o test_sg90  test_sg90.c

clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -rf $(APP)

install:
	cp $(MISC_SRC) /work/linux/drivers/misc/
	cp $(CHAR_SRC) /work/linux/drivers/char/

deploy:
	cp $(APP) /nfsroot/root
	cp *.ko /nfsroot/root
