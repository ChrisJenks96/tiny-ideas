#pragma once

#include <enet/enet.h>
#include <stdbool.h>
#include <string.h>

//client vars
extern ENetHost* pClient;
extern ENetAddress clientAddress;
extern ENetEvent clientEvent;
extern ENetPeer* clientPeer;
extern ENetPacket* pClientPacket;
extern bool bClientConnectedToHost;

extern int iClientState;
extern float fClientTimer;

#define CLIENT_STATE_NULL -1
#define CLIENT_STATE_INIT_FAILED 0
#define CLIENT_STATE_INIT_SUCCESS 1
#define CLIENT_STATE_CONNECTING 2
#define CLIENT_STATE_CONNECTING_FAILED 3
#define CLIENT_STATE_CONNECTING_SUCCESS 4

#define CLIENT_NETWORK_UPDATE 1.0f

bool clientInit();
bool clientConnect(char* pIP, short sPort, int iMSDelay);
bool clientUpdate();
void clientDestroy();