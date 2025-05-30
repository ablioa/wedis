#include "connection.h"

/* global connection pool */
RedisConnectionPool RedisCp;

void ginit(){
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0){
        exit(0);
    }
}

void gdestroy(){
    WSACleanup();
}

char * get_real_host(const char * hostname){
    struct hostent * host = gethostbyname(hostname);
    if(!host){
        return NULL;
    }

    return inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
}

RedisConnection init(char * address,int port){
    char * realhost = get_real_host(address);
    if(realhost == NULL){
        realhost = address;
    }

    RedisConnection stream = (RedisConnection)calloc(1,sizeof(struct redis_connection));

    stream->host = (char*)calloc(255,sizeof(char));
    strcpy(stream->host,address);
    stream->port = port;

    stream->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (stream->socket == INVALID_SOCKET){
        printf("invalid socket !");
        return NULL;
    }

    stream->address.sin_family = AF_INET;
    stream->address.sin_port = htons(port);
    stream->address.sin_addr.S_un.S_addr = inet_addr(realhost);

connect:
    if (connect(stream->socket, (struct sockaddr *)&(stream->address), sizeof(stream->address)) == SOCKET_ERROR){
        int result = DumpMessage(ERR_CANNOT_CONNECTED);
        if(result == 4){
            goto connect;
        }else{
            closesocket(stream->socket);
        }
        return NULL;
    }

	pool_add_connection(stream);

    return stream;
}

void sendmsg(RedisConnection stream,char * message,int length){
    send(stream->socket, message, length, 0);
}

/* close redis connection */
void close_connection(RedisConnection stream){
    closesocket(stream->socket);
}

void init_connection_pool(){
	memset(&RedisCp,0,sizeof(RedisCp));

	RedisCp.size =0 ;

	RedisCp.con_pool = build_list();
}

/* add connection into connection pool */
void pool_add_connection(const RedisConnection con){
	con->connection_id=RedisCp.size++;
	add_node(RedisCp.con_pool,con);
}

/* redis connection comparator */
int connection_comparator(const void * a,const void * b){
	if(((RedisConnection)a)->connection_id == ((RedisConnection)b)->connection_id){
        return 1;
    }
	return 0;
}

/* remove connection from cache*/
void pool_remove_connection(const int connection_id){
	RedisConnection conn = (RedisConnection)calloc(1,sizeof(struct redis_connection));
	conn->connection_id = connection_id;

	RedisConnection ret = (RedisConnection)find_from_list(RedisCp.con_pool,connection_comparator,conn);
	if(ret != NULL){
		close_connection(ret);
	}
}