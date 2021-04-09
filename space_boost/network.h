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

typedef struct DATA_PACKET
{
	int8_t cId; //-127-127 (server gives us an iden)
	float fPos[2]; //x&y
} DATA_PACKET;

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

extern DATA_PACKET myData;

#define CLIENT_STATE_NULL -1
#define CLIENT_STATE_INIT_FAILED 0
#define CLIENT_STATE_INIT_SUCCESS 1
#define CLIENT_STATE_CONNECTING 2
#define CLIENT_STATE_CONNECTING_FAILED 3
#define CLIENT_STATE_CONNECTING_SUCCESS 4
#define CLIENT_STATE_NEW_ID 5

#define CLIENT_NETWORK_UPDATE 1.0f

bool clientInit();
bool clientConnect(char* pIP, unsigned short sPort, int iMSDelay);
bool clientUpdate();
void clientDestroy();
void clientGetNewID();