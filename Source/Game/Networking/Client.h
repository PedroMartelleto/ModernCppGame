#pragma once
#include "../../Engine/Engine.h"
#include "NetworkHost.h"

class Client : public NetworkHost
{
public:
	ENetHost* client = nullptr;
	ENetPeer* peer = nullptr;
	ENetAddress address = ENetAddress();
	ENetEvent event = ENetEvent();

	Client()
	{
		client = enet_host_create(nullptr, 1, 1, 0, 0);

		if (client == NULL)
		{
			fprintf(stderr, "An error occurred while trying to create an enet host.\n");
			exit(1);
		}
	}

	~Client()
	{
		enet_host_destroy(client);
	}

	void Connect()
	{
		enet_address_set_host(&address, "127.0.0.1");
		address.port = 7777;

		peer = enet_host_connect(client, &address, 1, 0);

		if (client == NULL)
		{
			fprintf(stderr, "No available peers for initiating an enet connection.\n");
			exit(1);
		}

		if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			DEBUG_LOG("NET", LOG_MSG, "Connection to server succeded.");
		}
		else
		{
			enet_peer_reset(peer);
			DEBUG_LOG("NET", LOG_MSG, "Connection to server failed.");
			return;
		}
	}

	void Disconnect()
	{
		enet_peer_disconnect(peer, 0);

		while (enet_host_service(client, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				DEBUG_LOG("NET", LOG_MSG, "Disconnection succeded.");
				break;
			}
		}
	}

	void Update(float deltaTime)
	{
		while (enet_host_service(client, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				DEBUG_LOG("NET", LOG_MSG, "A packet of length %u containing %s was from %x:%u on channel %u.",
					event.packet->dataLength,
					event.packet->data,
					event.peer->address.host,
					event.peer->address.port,
					event.channelID);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				DEBUG_LOG("NET", LOG_MSG, "Server disconnected.");
				break;
			}
		}
	}
};

