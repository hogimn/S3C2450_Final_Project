CC:=arm-none-linux-gnueabi-gcc
CFLAGS:=-Wall -O0
LDFLAGS:=-L../drivers/lib/build -lpthread -ldrivers
INCLUDE:=-I../drivers/lib/inc -I../drivers/kernel-level/
SAMPLES:= samples_temphumid samples_servo samples_relay samples_photo \
			samples_moisture samples_magnetic samples_fan \
			samples_on_off_devices

all: 
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_temphumid samples_temphumid.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_servo     samples_servo.c     $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_relay samples_relay.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_photo samples_photo.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_moisture samples_moisture.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_magnetic samples_magnetic.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_fan samples_fan.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_on_off_devices samples_on_off_devices.c $(LDFLAGS)

clean:
	rm -rf $(SAMPLES)

deploy:
	cp $(SAMPLES) /nfsroot/root
