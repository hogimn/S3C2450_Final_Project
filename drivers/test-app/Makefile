CDIR := /usr/local/arm/4.4.1/bin
CC := $(CDIR)/arm-none-linux-gnueabi-gcc
APP := test_dht11 test_sg90
CFLAGS := -Wall -O0

all:
	$(CC) $(CF) -o test_dht11 test_dht11.c
	$(CC) $(CF) -o test_sg90  test_sg90.c -I../kernel-level 

clean:
	rm -rf $(APP)

deploy:
	cp $(APP) /nfsroot/root
