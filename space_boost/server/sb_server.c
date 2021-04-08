#include <enet/enet.h>
#include <stdio.h>

int main(int argc, char** argv) 
{
	if (argc > 1)
	{
		if (enet_initialize () != 0) 
	    {
	        printf("An error occurred while initializing ENet.\n");
	        return 1;
	    }

	    ENetAddress address = {0};
	    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
	    address.port = atoi(argv[1]);

	    /* create a server */
	    ENetHost* pServer = enet_host_create(&address, 32, 2, 0, 0);

	    if (pServer == NULL) 
	    {
	        printf("An error occurred while trying to create an ENet server host.\n");
	        return 1;
	    }

	    printf("Started a server [localhost] [%s]...\n", argv[1]);

	    ENetEvent event;

	    /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
	    int iRet = enet_host_service(pServer, &event, 1000);
	    char cChar;
	    while ((iRet > 0 || iRet == 0) && cChar != '\x1b') 
	    {
	    	cChar = getchar();
	    	iRet = enet_host_service(pServer, &event, 1000);
	        switch (event.type) 
	        {
	            case ENET_EVENT_TYPE_CONNECT:
	                printf("A new client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);
	                /* Store any relevant client information here. */
	                event.peer->data = "Client information";
	                break;

	            case ENET_EVENT_TYPE_RECEIVE:
	                printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
	                        event.packet->dataLength,
	                        event.packet->data,
	                        event.peer->data,
	                        event.channelID);
	                /* Clean up the packet now that we're done using it. */
	                enet_packet_destroy (event.packet);
	                break;

	            case ENET_EVENT_TYPE_DISCONNECT:
	                printf("%s disconnected.\n", event.peer->data);
	                /* Reset the peer's client information. */
	                event.peer->data = NULL;
	                break;

	            case ENET_EVENT_TYPE_NONE:
	                break;
	        }
	    }

	    if (iRet < 0)
	    {
	    	printf("Server failure!\n");
	    }

	    printf("Server closing down...\n");
	    enet_host_destroy(pServer);
    	enet_deinitialize();
	}
    
    return 0;
}
