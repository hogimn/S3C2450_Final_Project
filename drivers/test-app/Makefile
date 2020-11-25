CDIR := /usr/local/arm/4.4.1/bin
CC := $(CDIR)/arm-none-linux-gnueabi-gcc
APP := test_dht11 test_sg90 test_c270 test_led_onboard
CFLAGS := -Wall -O0

all:
	$(CC) $(CF) -o test_dht11 test_dht11.c
	$(CC) $(CF) -o test_sg90  test_sg90.c -I../kernel-level
	$(CC) $(CF) -o test_c270  test_c270.c -I../user-level -L../user-level/build -lc270
	$(CC) $(CF) -o test_led_onboard  test_led_onboard.c -I../kernel-level -lrt

clean:
	rm -rf $(APP)

deploy:
	cp $(APP) /nfsroot/root
