#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#define PORT 8080

//client vars
int iSock, iValReadClient;
struct sockaddr_in sAddressClient;
char* pHelloClient = "Hello from Client!";
char cBufferClient[1024];

bool clientInit(const char *restrict pIP);
bool clientRead();