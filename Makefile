APP      = main

CC=g++
CFLAGS=-Wall -mwindows -Wreorder -Wwrite-strings -B include

TARGET=output/wedis
RESOURCE=output/main.res
SRCS=callbacks.cpp \
	common.cpp \
	control.cpp \
	main.cpp \
	smconfig.cpp \
	view.cpp \
	debug.cpp \
	redis.cpp

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS) $(RESOURCE)
	$(CC) $(CFLAGS) -o $@ $^ -lcomctl32 libs/smctrl.lib -lwsock32

$(RESOURCE):resource/res.rc
	windres -i resource/res.rc -O COFF -o $(RESOURCE)

clean:
	rm output/*.res output/*.exe 