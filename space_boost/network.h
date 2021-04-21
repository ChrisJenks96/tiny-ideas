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

#include "object.h"

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
	int iStatus; //the id of this specific client so we don't have to index ourselves the data below
	DATA_PACKET data[SERVER_MAX_CLIENTS];
} DATA_PACKET_GLOBAL;

#define CLIENT_STATE_NULL -1
#define CLIENT_STATE_INIT_FAILED 0
#define CLIENT_STATE_INIT_SUCCESS 1
#define CLIENT_STATE_CONNECTING 2
#define CLIENT_STATE_CONNECTING_FAILED 3
#define CLIENT_STATE_CONNECTING_SUCCESS 4
#define CLIENT_STATE_NEW_ID 5
#define CLIENT_STATE_IN_GAME 6
#define CLIENT_STATE_SERVER_FULL 7

class cClient
{
	public:
		cClient()
		{
			mClientTimer = 0.0f;
			mServerID = -1;
			mClientInit = false;
			mClientState = CLIENT_STATE_NULL;
			memset(&mClientName[0], 0, 32);
		}
		
		bool Init();
		bool Connect(unsigned short sPort, int iMSDelay);
		void Free();
		void IPFree();
		bool GetNewID();
		void CreateUpdateThread(cMainShip& rShip);
		void Disconnect();
		void Update();
		void UpdateIPAddrText(char cKey);
		char* GetIPAddrText(){return mIPAddrText;}
		int& GetIPAddrTextLength(){return mIPTextLength;}
		void SetClientPos(float fX, float fY);
		int& GetServerID(){return mServerID;}
		DATA_PACKET_GLOBAL& GetGlobalData(){return mGlobalData;}
		DATA_PACKET& GetData(){return mData;}
		void SetClientReadyToPlay(bool bRTP){mData.bReadyToPlay = bRTP;}
		bool& GetServerReadyToLetUsPlay(){return mGlobalData.data[mServerID].bServerReady;}
		void SetDataShipDirection(bool bUp, bool bLeft, bool bRight);
		bool& GetConnectedToHost(){return mClientConnectedToHost;}
		const char* GetClientName(){return mClientName;}
		int GetClientNameLength(){return mClientNameLength;} 
		int& GetClientState(){return mClientState;}
	private:
		void GetClientNameFromSystem();
		char mClientName[32];
		int mClientNameLength;

		int mSock;
		struct sockaddr_in mSa;
		socklen_t mFromlen;
		int mClientState;
		bool mClientConnectedToHost;
		bool mClientInit;
		float mClientTimer;
		int mIPTextLength;
		int mThreadCount;
		pthread_t mThread1;
		pthread_mutex_t mMutex1;
		//we're just going to sent all the data from each client over the network
		DATA_PACKET_GLOBAL mGlobalData;
		//the data we get from the client
		DATA_PACKET mData;
		int mServerID;
		float mClientPos[2];
		char mIPAddrText[IP_MAX_TEXT];
};