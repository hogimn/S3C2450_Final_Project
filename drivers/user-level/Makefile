NAME	:= test_c270_live
CC		:=arm-none-linux-gnueabi-gcc

BUILD	:= build
SOURCE	:= .

SOURCES	:= $(shell find $(SOURCE) -name '*.c')
OBJECTS	:= $(addprefix $(BUILD)/,$(SOURCES:%.c=%.o))

CFLAGS	:= -Wall -g
LDFLAGS	:= -lc 

all: C270_LIB

C270_LIB: $(OBJECTS)
	ar -rc $(BUILD)/libc270.a $(OBJECTS)
	

$(BUILD)/%.o: %.c
	@echo - $<
	@mkdir -p $(BUILD)/$(SOURCE)
	@$(CC) $(CFLAGS) -I$(dir $<) -c $< -o $@

clean:
	@rm -rf $(BUILD) $(NAME)

deploy:
	cp live-framebuffer /nfsroot/root
