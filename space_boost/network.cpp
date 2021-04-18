#include "network.h"

bool cClient::Init()
{
	if (!mClientInit)
	{
		/* create an Internet, datagram, socket using UDP */
		mSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (mSock == -1) 
		{
			mClientState = CLIENT_STATE_INIT_FAILED;
			mClientConnectedToHost = false;
			return false;
		}

		mClientState = CLIENT_STATE_INIT_SUCCESS;
		mClientInit = true;
		mClientConnectedToHost = true;

		mMutex1 = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(&mMutex1, NULL);
	}

	else
	{
		//when the client is set up, we still call this function, but resetting to client connecting
		mClientConnectedToHost = true;
	}

	return true;
}

void cClient::UpdateIPAddrText(char cKey)
{
	//backspace
	if (cKey == 3)
	{
		mIPTextLength -= 1;
		mIPAddrText[mIPTextLength] = 0;
		if (mIPTextLength < 0)
		{
			mIPTextLength = 0;
		}
	}

	else
	{
		mIPAddrText[mIPTextLength++] = cKey;
		if (mIPTextLength > IP_MAX_TEXT)
		{
			mIPTextLength = IP_MAX_TEXT;
		}
	}
}

bool cClient::GetNewID()
{
	//if the host cannot be contacted in 5 seconds, then abort and presume the ip provided is crap...
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	setsockopt(mSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	int iBytesRec = recvfrom(mSock, &mGlobalData, sizeof(DATA_PACKET_GLOBAL), 0, (struct sockaddr*)&mSa, &mFromlen);
	if (iBytesRec == -1)
	{
		mClientState = CLIENT_STATE_CONNECTING_FAILED;
		mClientConnectedToHost = false;
		return false;
	}

	else
	{
		//there has been no packet sent back, so this means we're at capacity on the server
		if (mGlobalData.iStatus == -2)
		{
			mClientState = CLIENT_STATE_SERVER_FULL;
			mClientConnectedToHost = false;
			return false;
		}

		else
		{
			//globalData.iStatus holds the last client id added if success
			mServerID = mGlobalData.iStatus;
			//sending data back to the server
			mData.cId = mGlobalData.iStatus;
			//tells us that the handshake packet is received from host
			mClientState = CLIENT_STATE_NEW_ID;
			mClientConnectedToHost = true;
			return true;
		}
	}
}

bool cClient::Connect(unsigned short sPort, int iMSDelay)
{
	if (mIPTextLength == 0)
	{
		mClientState = CLIENT_STATE_CONNECTING_FAILED;
		mClientConnectedToHost = false;
		return false;
	}

	else
	{
		//if it's a character based ip, make sure to lower the char
		int iCount = 0;
		while (iCount < mIPTextLength)
		{
			if (mIPAddrText[iCount] >= 65 && mIPAddrText[iCount] <= 90)
			{
				mIPAddrText[iCount] += 32;
			}

			iCount++;
		}
	}

	//tell the server we need an id allocated from the server
	mData.cId = -1;
	mFromlen = sizeof(mSa);

	/* Zero out socket address */
	memset(&mSa, 0, sizeof(mSa));

	/* The address is IPv4 */
	mSa.sin_family = AF_INET;

	/* IPv4 addresses is a uint32_t, convert a string representation of the octets to the appropriate value */
	mIPAddrText[mIPTextLength] = 0;
	mSa.sin_addr.s_addr = inet_addr(mIPAddrText);

	/* sockets are unsigned shorts, htons(x) ensures x is in network byte order, set the port to 7654 */
	mSa.sin_port = htons(sPort);

	int iBytesSent = sendto(mSock, &mData, sizeof(DATA_PACKET), 0, (struct sockaddr*)&mSa, sizeof(mSa));
	if (iBytesSent < 0)
	{
		mClientState = CLIENT_STATE_CONNECTING_FAILED;
		mClientConnectedToHost = false;
		return false;
	}

	mClientState = CLIENT_STATE_CONNECTING;
	mClientConnectedToHost = true;
	mData.bConnected = true;

	return true;
}

static void* UpdateThreadInvoke(void* args)
{
	((cClient*)args)->Update();
	return NULL;
}

void cClient::Update()
{
	while (mClientState == CLIENT_STATE_IN_GAME)
	{
		pthread_mutex_lock(&mMutex1);
		
		//send our data to the server for updating
		mData.fPos[0] = mClientPos[0];
		mData.fPos[1] = mClientPos[1];
		int iBytesSent = sendto(mSock, &mData, sizeof(DATA_PACKET), 0, (struct sockaddr*)&mSa, sizeof(mSa));
		if (iBytesSent > 0)
		{

		}
		
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		setsockopt(mSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		//receive data back from the server
		int iRecSize = recvfrom(mSock, &mGlobalData, sizeof(DATA_PACKET_GLOBAL), 0, (struct sockaddr*)&mSa, &mFromlen);
		if (iRecSize == -1 || !mGlobalData.data[mServerID].bConnected)
		{
			//change the state to force the client to stop seeking packets from server
			mClientState = CLIENT_STATE_CONNECTING_SUCCESS;
			mClientConnectedToHost = false;
		}

		pthread_mutex_unlock(&mMutex1);
	}
}

void cClient::CreateUpdateThread(cMainShip& rShip)
{
	//update the ships data
	rShip.SetXY(mGlobalData.data[mServerID].fPos[0], 
		mGlobalData.data[mServerID].fPos[1]);

	//setup the thread to run the send/rec from server
	int iRet;
	if ((iRet = pthread_create(&mThread1, NULL, UpdateThreadInvoke, (void*)this)))
	{
		printf("Thread creation failed!\n");
		//kill the connection and go back to the menu
	}

	//update to the client being in the online game
	mClientState = CLIENT_STATE_IN_GAME;
}

void cClient::IPFree()
{
	memset(&mIPAddrText, 0, IP_MAX_TEXT);
	mIPTextLength = 0;
}

void cClient::Disconnect()
{
	//disconnect from the server, kill the receiving thread
	if (mClientConnectedToHost)
	{
		//send a packet that tells the server we're done
		mData.bConnected = false;
		//make main thread wait till the networking thread is done
		pthread_join(mThread1, NULL);
	}
}

void cClient::Free()
{
	mClientInit = false;
	pthread_mutex_destroy(&mMutex1);
	close(mSock);
}