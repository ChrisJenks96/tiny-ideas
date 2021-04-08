#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>

#define PORT 8080

//client vars
extern int iSock, iValReadClient;
extern struct sockaddr_in sAddressClient;
extern char* pHelloClient;
extern char cBufferClient[1024];

bool clientInit(const char *restrict pIP);
bool clientRead();