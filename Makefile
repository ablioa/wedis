APP      = output/wedis

CC=gcc
CFLAGS=-g -Wall -Wwrite-strings -Wincompatible-pointer-types -B include
LFLAGS=-mwindows
SYSLIB=-lcomctl32 -lwsock32

RESOURCE=output/main.res

DIRM=./output

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
OBJS:=$(addprefix $(DIRM)/,$(OBJS))

$(APP):$(OBJS) $(RESOURCE)
	$(CC) $(LFLAGS) -o $@ $^ $(SYSLIB)

$(DIRM)/%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $^

$(RESOURCE):resource/res.rc
	windres -i resource/res.rc -O COFF -o $(RESOURCE)

clean:
	rm output/*.res output/*.exe output/*.o