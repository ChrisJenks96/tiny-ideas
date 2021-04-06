#include "network.h"

bool clientInit(const char *restrict pIP)
{
	if ((iSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Client: Socket failed\n");
		return false;
	}

	sAddressClient.sin_family = AF_INET;
	sAddressClient.sin_port = htons(PORT);

	if (inet_pton(AF_INET, pIP, &sAddressClient.sin_addr) <= 0)
	{
		printf("Client: Invalid address - Address not supported\n");
		return false;
	}

	if (connect(iSock, (const struct sockaddr*)&sAddressClient, sizeof(struct sockaddr_in)) < 0)
	{
		printf("Client: Connection failed\n");
		return false;
	}

	return true;
}

bool clientRead()
{
	send(iSock, pHelloClient, strlen(pHelloClient), 0);
	printf("Client: Message sent\n");
	iValReadClient = read(iSock, cBufferClient, 1024);
	printf("Client: %s\n", cBufferClient);
	return true;
}