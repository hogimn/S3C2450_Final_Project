CDIR := /usr/local/arm/4.4.1/bin
CC := $(CDIR)/arm-none-linux-gnueabi-gcc
APP := test_dht11 test_sg90 test_c270 test_led_onboard \
	   test_szh_rlbg_012 test_bh1750 test_mg995 \
	   test_leadcool120 test_szh_ssbh_040 test_fc_28 test_humidifier
	   
CFLAGS := -Wall -O0 -I../kernel-level -I../lib/inc
LDFLAGS := -L../lib/build -ldrivers

all:
	$(CC) $(CFLAGS) -o test_dht11   test_dht11.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_sg90    test_sg90.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_c270    test_c270.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_bh1750  test_bh1750.c  $(LDFLAGS) 
	$(CC) $(CFLAGS) -o test_mg995   test_mg995.c  $(LDFLAGS) 
	$(CC) $(CFLAGS) -o test_led_onboard  test_led_onboard.c  $(LDFLAGS) -lrt
	$(CC) $(CFLAGS) -o test_szh_rlbg_012 test_szh_rlbg_012.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_leadcool120 test_leadcool120.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_szh_ssbh_040 test_szh_ssbh_040.c  $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_fc_28 test_fc_28.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o test_humidifier   test_humidifier.c  $(LDFLAGS)
	
clean:
	rm -rf $(APP)

deploy:
	cp $(APP) /nfsroot/root
