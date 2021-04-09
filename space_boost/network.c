#include "network.h"

int iSock;
struct sockaddr_in sa;
int iBytesSent, iBytesRec;
socklen_t fromlen;
int iClientState = CLIENT_STATE_NULL;
bool bClientConnectedToHost = false;
float fClientTimer = 0.0f;
bool bClientInit = false;

pthread_t thread1;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

DATA_PACKET myData;

bool clientInit()
{
	/* create an Internet, datagram, socket using UDP */
	iSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (iSock == -1) 
	{
		iClientState = CLIENT_STATE_INIT_FAILED;
	    return false;
	}

	iClientState = CLIENT_STATE_INIT_SUCCESS;
	return true;
}

void clientGetNewID()
{
	pthread_mutex_lock(&mutex1);
	iBytesRec = recvfrom(iSock, &myData, sizeof(DATA_PACKET), 0, (struct sockaddr*)&sa, &fromlen);	
	//there has been no packet sent back, so this means we're at capacity on the server
	if (myData.cId == -1)
	{
		iClientState = CLIENT_STATE_SERVER_FULL;
	}

	else
	{
		//tells us that the handshake packet is received from host
		iClientState = CLIENT_STATE_NEW_ID;
	}
	
	pthread_mutex_unlock(&mutex1);
}

bool clientConnect(char* pIP, unsigned short sPort, int iMSDelay)
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
		/*while (*pIP != '\0')
		{
			*pIP += (*pIP >= 65 && *pIP <= 90) ? 32 : 0;
			pIP++;
		}*/
	}

	//tell the server we need an id allocated from the server
	myData.cId = -1;
	fromlen = sizeof(sa);

	/* Zero out socket address */
	memset(&sa, 0, sizeof(sa));

	/* The address is IPv4 */
	sa.sin_family = AF_INET;

	/* IPv4 addresses is a uint32_t, convert a string representation of the octets to the appropriate value */
	sa.sin_addr.s_addr = inet_addr(pIP);

	/* sockets are unsigned shorts, htons(x) ensures x is in network byte order, set the port to 7654 */
	sa.sin_port = htons(sPort);

	iBytesSent = sendto(iSock, &myData, sizeof(DATA_PACKET), 0,(struct sockaddr*)&sa, sizeof(sa));
	if (iBytesSent < 0)
	{
		close(iSock);
		iClientState = CLIENT_STATE_CONNECTING_FAILED;
		return false;
	}

	iClientState = CLIENT_STATE_CONNECTING_SUCCESS;
	return true;
}

void clientUpdate(float* fDeltaTime)
{
	fClientTimer += 1.0f * *fDeltaTime;
	if (fClientTimer > CLIENT_NETWORK_UPDATE)
	{
		//do client update shit here...

		fClientTimer = 0.0f;
	}
}

void clientDestroy()
{
	close(iSock);
}