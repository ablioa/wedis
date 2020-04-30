APP      = main

CC=gcc
CFLAGS=-Wall -mwindows

TARGET=wedis

SRCS=main.c

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS) main.res
	$(CC) $(CFLAGS) -o $@ $^ -lwsock32 -lcomctl32

main.res:main.rc
	windres -i main.rc -O COFF -o main.res

clean:
	rm *.o *.res *.exe

