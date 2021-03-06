#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */ 
#include <stdlib.h>
#include <stdbool.h>

#include <time.h>
#include <math.h>

#define SCR_WIDTH 640
#define SHIP_SIZE 32
#define SCR_HEIGHT 480
#define SHIP_OFFSET_X 40
#define MAX_CLIENTS 4

#define SHIP_POS_X_DIFF (SCR_WIDTH / 2) - (SHIP_SIZE / 2)

#define TARGET_FPS 1.0 / 30.0

typedef struct DATA_PACKET
{
	int8_t cId; //0-255 (server gives us an iden)
	float fPos[2]; //x&y
	bool bServerReady; //when all players have agreed to begin the game
	bool bReadyToPlay; //each players will press 'space' to begin
	bool bPressUp;
	bool bPressLeft;
	bool bPressRight;
	bool bConnected; //whether we are connected to the server or not
	char cName[32];
} DATA_PACKET;

typedef struct DATA_PACKET_GLOBAL
{
	int iStatus; //the status all this data, normally will default to last client id, but can be used to show server full etc...
	DATA_PACKET data[MAX_CLIENTS];
} DATA_PACKET_GLOBAL;

typedef struct CLIENT_ADDR
{
	struct sockaddr_in addr[MAX_CLIENTS];
} CLIENT_ADDR;

//we're just going to sent all the data from each client over the network
static DATA_PACKET_GLOBAL globalData;
//the data we get from the client
static DATA_PACKET data;
//storing all the client information for sending information back out to them
static CLIENT_ADDR clients;

static double dNow, dLast;

static double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}

int main(void)
{
	int iSock;
	struct sockaddr_in sa; 
	int iRecSize;
	socklen_t fromlen;

	int8_t iCurrentNumOfClients = 0;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(7654);
	fromlen = sizeof(sa);

	iSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//allows for multiple addr and port (in case we crash and don't close the socket down correctly...)
	setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	setsockopt(iSock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	
	if (bind(iSock, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("error bind failed");
		close(iSock);
		exit(EXIT_FAILURE);
	}

	//set the data, 0 will set bConnected to false...
	memset(&globalData, 0, sizeof(DATA_PACKET_GLOBAL));
	globalData.data[0].cId = -1;
	globalData.data[1].cId = -1;
	globalData.data[2].cId = -1;
	globalData.data[3].cId = -1;

	dLast = 0;

	while (1) 
	{
		dNow = clockToMilliseconds(clock());
		if ((dNow - dLast) >= TARGET_FPS)
		{
			/*struct timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			setsockopt(iSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));*/

			iRecSize = recvfrom(iSock, &data, sizeof(DATA_PACKET), 0, (struct sockaddr*)&sa, &fromlen);
			if (iRecSize > 0)
			{
				//allocate the client a new id
				if (data.cId == -1)
				{
					//see if the server has room for a new client
					if (iCurrentNumOfClients < MAX_CLIENTS)
					{
						for (int i = 0; i < MAX_CLIENTS; i++)
						{
							if (globalData.data[i].cId == -1)
							{
								globalData.data[i].fPos[0] = (SHIP_POS_X_DIFF + (SHIP_SIZE * (i+1))) - (SHIP_SIZE * MAX_CLIENTS) / 2;
								globalData.data[i].fPos[1] = SCR_HEIGHT - SHIP_SIZE;
								globalData.data[i].cId = i;
								globalData.data[i].bConnected = true;
								globalData.iStatus = i;
								//save the client whos just connected's information
								clients.addr[i] = sa;
								iCurrentNumOfClients++;
								break;
							}
						}
					}

					else
					{
						printf("Server at Max Capacity (%i/%i)\n", iCurrentNumOfClients, MAX_CLIENTS);
						globalData.iStatus = -2;
					}
				}

				//the client wishes to disconnect from the server
				else if (!data.bConnected && data.cId != -1)
				{
					if (globalData.data[data.cId].cId != -1)
					{
						//reset all the data for the client
						memset(&globalData.data[data.cId], 0, sizeof(DATA_PACKET));
						globalData.data[data.cId].cId = -1;
						memset(&globalData.data[data.cId].cName, 0, 32);
						//allocate a new slot for a possible new client
						iCurrentNumOfClients--;
						printf("Connected: %i\n", iCurrentNumOfClients);
					}	
				}

				//sending data back to the server from the client, this will have all the client data in
				else
				{
					globalData.data[data.cId].fPos[0] = data.fPos[0];
					globalData.data[data.cId].fPos[1] = data.fPos[1];
					globalData.data[data.cId].bPressLeft = data.bPressLeft;
					globalData.data[data.cId].bPressRight = data.bPressRight;
					globalData.data[data.cId].bPressUp = data.bPressUp;
					globalData.data[data.cId].bReadyToPlay = data.bReadyToPlay;
					
					//update once, the client name will never change mid game
					int clientNameLength = strlen(globalData.data[data.cId].cName);
					if (clientNameLength == 0)
					{
						strncpy(globalData.data[data.cId].cName, data.cName, 32);
					}
				}
			}

			//if we have some clients, we have to transmit the data across the network
			if (iCurrentNumOfClients > 0)
			{
				//find out if all the players are ready to play
				int iClientsReadyToPlay = 0;
				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if (globalData.data[i].bReadyToPlay)
					{
						iClientsReadyToPlay++;
					}
				}

				printf("Client number: %i\n", iCurrentNumOfClients);

				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if (globalData.data[i].bConnected)
					{
						if (iClientsReadyToPlay == iCurrentNumOfClients)
						{
							//toggle the server being ready to let the clients play
							if (!globalData.data[i].bServerReady)
							{
								globalData.data[i].bServerReady = true;
							}
						}

						sendto(iSock, &globalData, sizeof(DATA_PACKET_GLOBAL), 0, (struct sockaddr*)&clients.addr[i], sizeof(sa));
						printf("%i %f %f %i %i %i %s\n", 
							globalData.data[i].cId,
							globalData.data[i].fPos[0], globalData.data[i].fPos[1],
							globalData.data[i].bPressLeft, globalData.data[i].bPressRight, globalData.data[i].bPressUp, 
							globalData.data[i].cName);
					}
				}

				printf("\n");
			}

			dLast = dNow;
		}
	}
}