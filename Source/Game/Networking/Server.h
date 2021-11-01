#pragma once
#include "../../Engine/Engine.h"
#include "NetworkHost.h"

class Server : public NetworkHost
{
public:
	ENetAddress address = ENetAddress();
	ENetEvent event = ENetEvent();
	ENetHost* server = nullptr;

	Server()
	{
		address.host = ENET_HOST_ANY;
		address.port = 7777;
	
		server = enet_host_create(&address, 32, 1, 0, 0);

		if (server == NULL)
		{
			fprintf(stderr, "An error occurred while trying to create an enet server host.\n");
			exit(1);
		}
	}

	~Server()
	{
		enet_host_destroy(server);
	}

	void Connect() {}
	void Disconnect() {}

	void Update(float deltaTime)
	{
		while (enet_host_service(server, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %u containing %s was from %x:%u on channel %u.\n",
					event.packet->dataLength,
					event.packet->data,
					event.peer->address.host,
					event.peer->address.port,
					event.channelID);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x:%u disconnected.\n", event.peer->address.host, event.peer->address.port);
				event.peer->data = NULL;
				break;
			}
		}
	}
};

