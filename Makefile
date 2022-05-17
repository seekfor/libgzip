
CC := gcc
AR := ar

CFLAGS := -fPIC -pthread -I./src -D_GNU_SOURCE
CFLAGS += -I./include

LDFLAGS += -lpthread -lm -lc -ldl

SRCS += $(wildcard ./src/*.c)

OBJS := $(patsubst %.c,%.o, $(SRCS))


%.o: %.c
	@echo compiling $<
	@$(CC) $(CFLAGS) -o $@ -c $<

all: ./test/testpng ./test/testgz

./test/testgz: $(OBJS) ./test/testgz.o ./test/bmp.o
	@$(CC) -o $@ $(LDFLAGS) $^
	@echo build ./test/testpng OK!
./test/testpng: $(OBJS) ./test/testpng.o ./test/bmp.o
	@$(CC) -o $@ $(LDFLAGS) $^
	@echo build ./test/testpng OK!
clean:
	-@rm -rf $(OBJS) ./test/*.o ./test/testgz ./test/testpng





