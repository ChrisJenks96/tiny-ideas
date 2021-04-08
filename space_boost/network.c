#include "network.h"

ENetHost* pClient = NULL;
ENetAddress clientAddress;
ENetEvent clientEvent;
ENetPeer* clientPeer = NULL;
ENetPacket* pClientPacket = NULL;
int iClientState = CLIENT_STATE_NULL;
bool bClientConnectedToHost = false;
float fClientTimer = 0.0f;

bool clientInit()
{
	if (enet_initialize () != 0) 
	{
		return false;
	}

	pClient = enet_host_create(NULL, 32, 2, 0, 0);

	if (pClient == NULL)
	{
		iClientState = CLIENT_STATE_INIT_FAILED;
	    return false;
	}

	iClientState = CLIENT_STATE_INIT_SUCCESS;
	return true;
}

bool clientConnect(char* pIP, short sPort, int iMSDelay)
{
	int iLength = strlen(pIP);
	if (iLength == 0)
	{
		iClientState = CLIENT_STATE_CONNECTING_FAILED;
		return false;
	}

	else
	{
		//if it's a character based ip, make sure to lower the char
		while (*pIP != '\0')
		{
			*pIP += (*pIP >= 65 && *pIP <= 90) ? 32 : 0;
			pIP++;
		}
	}

	enet_address_set_host(&clientAddress, pIP);
	clientAddress.port = sPort;

	//init connection, alloc the two channels 0 and 1
	clientPeer = enet_host_connect(pClient, &clientAddress, 2, 0);
	if (clientPeer == NULL)
	{
		iClientState = CLIENT_STATE_CONNECTING_FAILED;
		return false;
	}

	if (enet_host_service(pClient, &clientEvent, iMSDelay) > 0 && clientEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		iClientState = CLIENT_STATE_CONNECTING_SUCCESS;
		return true;
	}

	else
	{
		iClientState = CLIENT_STATE_CONNECTING_FAILED;
		enet_peer_reset(clientPeer);
		return false;
	}
}

bool clientUpdate()
{
	//host deals with freeing the packet on arrival
	pClientPacket = enet_packet_create("packet", 7, ENET_PACKET_FLAG_RELIABLE);
	//send the packet to peer we've connected too
	enet_peer_send(clientPeer, 0, pClientPacket);
	return true;
}

void clientDestroy()
{
	if (pClient != NULL)
	{
		//destroy the active packet used to send
		enet_packet_destroy(pClientPacket);
		//force disconnect a peer
		enet_peer_reset(clientPeer);
		enet_host_destroy(pClient);
	}
	
	enet_deinitialize();
}