#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */ 
#include <stdlib.h>
#include <stdbool.h>

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
	int iBytesSent;

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
				data.cId = iCurrentNumOfClients++;
				iBytesSent = sendto(iSock, &data, iDataSize, 0, (struct sockaddr*)&sa, sizeof(sa));
			}
		}
	}
}