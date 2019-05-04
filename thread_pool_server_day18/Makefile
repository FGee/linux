SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=thread_pool_server
CC:=gcc
CFLAGS:= -g -Wall -D DEBUG
$(ELF):$(OBJS)
	gcc $^ -o $@ -lpthread
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS)
