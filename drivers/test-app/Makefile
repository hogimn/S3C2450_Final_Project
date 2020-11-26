CDIR := /usr/local/arm/4.4.1/bin
CC := $(CDIR)/arm-none-linux-gnueabi-gcc
APP := test_dht11 test_sg90 test_c270 test_led_onboard
CFLAGS := -Wall -O0 -I../kernel-level -I../user-level
LDFLAGS := -L../user-level/build -ldrivers

all:
	$(CC) $(CFLAGS) -o test_dht11 test_dht11.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_sg90  test_sg90.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_c270  test_c270.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_led_onboard  test_led_onboard.c  $(LDFLAGS) -lrt

clean:
	rm -rf $(APP)

deploy:
	cp $(APP) /nfsroot/root
