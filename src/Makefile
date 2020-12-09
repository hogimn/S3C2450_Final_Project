CC=arm-none-linux-gnueabi-gcc
LDFLAGS=-L../drivers/lib/build/ \
		-L../lib/build \
	    -lpthread -ldrivers -lutils -lrt
INCLUDE=-I../drivers/lib/inc \
        -I../drivers/kernel-level/ \
        -I../lib/inc
all: main.c 
	make -C ../drivers/lib/
	make -C ../drivers/kernel-level/
	make -C ../drivers/test-app/
	make -C ../lib/
	make -C ../samples/
	$(CC) $(INCLUDE) -Wall -o main main.c $(LDFLAGS)

clean:
	make -C ../drivers/lib/ 	 clean
	make -C ../drivers/kernel-level/ clean
	make -C ../drivers/test-app/     clean
	make -C ../lib/					 clean
	make -C ../samples/              clean
	rm -rf main

deploy:
#	make -C ../drivers/lib/   deploy
	make -C ../drivers/kernel-level/ deploy
	make -C ../drivers/test-app/     deploy
	make -C ../samples/              deploy
	cp -r main music /nfsroot/root
