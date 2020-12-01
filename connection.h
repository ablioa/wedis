#ifndef myconnection_h
#define myconnection_h

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include "wedis.h"

void ginit();

RedisConnection init(char * address,int port);

void sendmsg(RedisConnection stream,char * message);

void close(RedisConnection stream);

#endif
