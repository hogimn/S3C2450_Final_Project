CC		:= arm-none-linux-gnueabi-gcc

BUILD	:= build
SOURCE	:= .

SOURCES	:= $(shell find $(SOURCE) -name '*.c')
OBJECTS	:= $(addprefix $(BUILD)/,$(SOURCES:%.c=%.o))

CFLAGS	:= -Wall -g -Iinc -I./sqlite3/include/
LDFLAGS	:= -L./sqlite3/lib/ -lsqlite3

all: DRIVERS_LIB

DRIVERS_LIB: $(OBJECTS)
	ar -rc $(BUILD)/libutils.a $(OBJECTS)
	

$(BUILD)/%.o: %.c
	@echo - $<
	@mkdir -p $(BUILD)/$(SOURCE)
	@$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@ $(LDFLAGS)

clean:
	@rm -rf $(BUILD)

deploy:
	cp ./sqlite3/lib/*so*    /nfsroot/lib
	cp ./sqlite3/bin/sqlite3 /nfsroot/usr/bin
