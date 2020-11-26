CC		:= arm-none-linux-gnueabi-gcc

BUILD	:= build
SOURCE	:= .

SOURCES	:= $(shell find $(SOURCE) -name '*.c')
OBJECTS	:= $(addprefix $(BUILD)/,$(SOURCES:%.c=%.o))

CFLAGS	:= -Wall -g -I../kernel-level/
LDFLAGS	:= -lc 

all: DRIVERS_LIB

DRIVERS_LIB: $(OBJECTS)
	ar -rc $(BUILD)/libdrivers.a $(OBJECTS)
	

$(BUILD)/%.o: %.c
	@echo - $<
	@mkdir -p $(BUILD)/$(SOURCE)
	@$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@

clean:
	@rm -rf $(BUILD)

deploy:
	cp live-framebuffer /nfsroot/root
