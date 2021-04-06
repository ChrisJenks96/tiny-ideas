#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#define PORT 8080

//server vars
int iServerFD, iNewSocket, iValReadServer;
struct sockaddr_in sAddressServer;
int iOpt = 1;
int iAddrLen = sizeof(struct sockaddr_in);
char cBufferServer[1024];
char* pHelloServer = "Hello from Server!";

static bool serverInit()
{
	//create socket file descriptor
	if ((iServerFD = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		printf("Server: Socket failed\n");
		return false;
	}

	//attach the socket to the PORT
	if (setsockopt(iServerFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &iOpt, sizeof(int)))
	{
		printf("Server: 'setsockopt' failed\n");
		return false;
	}

	sAddressServer.sin_family = AF_INET;
	sAddressServer.sin_addr.s_addr = INADDR_ANY;
	sAddressServer.sin_port = htons(PORT);

	if (bind(iServerFD, (struct sockaddr*)&sAddressServer, iAddrLen) < 0)
	{
		printf("Server: 'bind' failed\n");
		return false;
	}

	return true;
}

static bool serverRead()
{
	if (listen(iServerFD, 3) < 0)
	{
		printf("Server: 'listen' failed\n");
		return false;
	}

	if ((iNewSocket = accept(iServerFD, (struct sockaddr*)&sAddressServer, (socklen_t*)&iAddrLen)) < 0)
	{
		printf("Server: 'accept' failed\n");
		return false;
	}

	iValReadServer = read(iNewSocket, cBufferServer, 1024);
	printf("Server: %s\n", cBufferServer);
	send(iNewSocket, pHelloServer, strlen(pHelloServer), 0);
	printf("Server: Message sent\n");
	return true;
} 

int main(int argc, char** argv)
{
	bool go = serverInit();
	while (go)
	{
		serverRead();
	}

	close(iServerFD);
	close(iNewSocket);
	return 0;
}