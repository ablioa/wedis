APP      = output/wedis

CC=gcc
CFLAGS=-g --std=c11 -Wcpp -Wall -Wdiscarded-qualifiers -Wwrite-strings -Wincompatible-pointer-types -B include
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

$(RESOURCE):resource/res.rc resource/wedis_cn.rc1 resource/wedis_en.rc1
	windres --target=pe-x86-64 -D WX_CPU_AMD64 -i resource/res.rc -O COFF -o $(RESOURCE)

clean:
	rm output/*.res output/*.exe output/*.o
