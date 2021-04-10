#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "global.h"
#include <pthread.h>

typedef struct DATA_PACKET
{
	int8_t cId; //-127-127 (server gives us an iden)
	float fPos[2]; //x&y
} DATA_PACKET;

typedef struct DATA_PACKET_GLOBAL
{
	int iMyID; //the id of this specific client so we don't have to index ourselves the data below
	DATA_PACKET data[SERVER_MAX_CLIENTS];
} DATA_PACKET_GLOBAL;

//client vars
extern int iSock;
extern struct sockaddr_in sa;
extern socklen_t fromlen;
extern int iBytesSent;
extern char cBuffer[256];
extern int iClientState;
extern bool bClientConnectedToHost;
extern bool bClientInit;
extern float fClientTimer;
extern int iIPTextLength;
extern pthread_t thread1;
extern pthread_mutex_t mutex1;

//we're just going to sent all the data from each client over the network
extern DATA_PACKET_GLOBAL globalData;
//the data we get from the client
extern DATA_PACKET data;
extern int iServerID;

extern float fClientPos[2];

static char cIPAddrText[IP_MAX_TEXT];

#define CLIENT_STATE_NULL -1
#define CLIENT_STATE_INIT_FAILED 0
#define CLIENT_STATE_INIT_SUCCESS 1
#define CLIENT_STATE_CONNECTING 2
#define CLIENT_STATE_CONNECTING_FAILED 3
#define CLIENT_STATE_CONNECTING_SUCCESS 4
#define CLIENT_STATE_NEW_ID 5
#define CLIENT_STATE_IN_GAME 6
#define CLIENT_STATE_SERVER_FULL 7

bool clientInit();
bool clientConnect(char* pIP, unsigned short sPort, int iMSDelay);
void* clientUpdate();
void clientDestroy();
void clientIPFree();
void* clientGetNewID();