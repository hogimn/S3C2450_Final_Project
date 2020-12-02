CC:=arm-none-linux-gnueabi-gcc
CFLAGS:=-Wall -O0
LDFLAGS:=-L../drivers/user-level/build -lpthread -ldrivers
INCLUDE:=-I../drivers/user-level/inc -I../drivers/kernel-level/
SAMPLES:= samples_temphumid samples_servo

all: 
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_temphumid samples_temphumid.c $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDE) -o samples_servo     samples_servo.c     $(LDFLAGS)

clean:
	rm -rf $(SAMPLES)

deploy:
	cp $(SAMPLES) /nfsroot/root
