CC=arm-none-linux-gnueabi-gcc
LDFLAGS=-L/home/hogi/MDS-Embedded-Systems-Final-Project/drivers/user-level/build/ \
		-L../lib/build \
	    -lpthread -ldrivers -lutils
INCLUDE=-I/home/hogi/MDS-Embedded-Systems-Final-Project/drivers/user-level/inc \
        -I/home/hogi/MDS-Embedded-Systems-Final-Project/drivers/kernel-level/ \
        -I../lib/inc
all: main.c 
	make -C ../drivers/user-level/
	make -C ../drivers/kernel-level/
	make -C ../drivers/test-app/
	make -C ../lib/
	make -C ../samples/
	$(CC) $(INCLUDE) -Wall -o main main.c $(LDFLAGS)

clean:
	make -C ../drivers/user-level/ 	 clean
	make -C ../drivers/kernel-level/ clean
	make -C ../drivers/test-app/     clean
	make -C ../lib/					 clean
	make -C ../samples/              clean
	rm -rf main

deploy:
#	make -C ../drivers/user-level/   deploy
	make -C ../drivers/kernel-level/ deploy
	make -C ../drivers/test-app/     deploy
	make -C ../samples/              deploy
	cp main /nfsroot/root
