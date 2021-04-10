#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */ 
#include <stdlib.h>
#include <stdbool.h>

#define SCR_WIDTH 640
#define SHIP_SIZE 32
#define SCR_HEIGHT 480
#define SHIP_OFFSET_X 20
#define MAX_CLIENTS 4

#define SHIP_POS_X_DIFF ((SCR_WIDTH - (SHIP_OFFSET_X * 2)) / MAX_CLIENTS) - (SHIP_SIZE / 2)

typedef struct DATA_PACKET
{
	int8_t cId; //0-255 (server gives us an iden)
	float fPos[2]; //x&y
} DATA_PACKET;

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
	if (bind(iSock, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("error bind failed");
		close(iSock);
		exit(EXIT_FAILURE);
	}

	DATA_PACKET data;
	int iDataSize = sizeof(DATA_PACKET);

	while (1) 
	{
		iRecSize = recvfrom(iSock, &data, iDataSize, 0, (struct sockaddr*)&sa, &fromlen);
		if (iRecSize > 0)
		{
			sleep(1);
			//allocate the client a new id
			if (data.cId == -1)
			{
				if (iCurrentNumOfClients < MAX_CLIENTS)
				{
					data.fPos[0] = (SHIP_POS_X_DIFF * (iCurrentNumOfClients+1));
					data.fPos[1] = SCR_HEIGHT - SHIP_SIZE;
					data.cId = iCurrentNumOfClients++;
				}

				else
				{
					printf("Max Clients reached!\n", data.cId, data.fPos[0], data.fPos[1]);
					data.cId = -2;
				}

				printf("%i %f %f\n", data.cId, data.fPos[0], data.fPos[1]);
				sendto(iSock, &data, iDataSize, 0, (struct sockaddr*)&sa, sizeof(sa));
			}
		}
	}
}