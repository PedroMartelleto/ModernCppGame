#include "NetworkHost.h"

NetworkHost::NetworkHost(HostType type, const std::string& hostName)
{
	if (type == HostType::SERVER)
	{
		assert(hostName.length() > 0);
		address.host = ENET_HOST_ANY;
		address.port = GAME_PORT;
		host = enet_host_create(&address, MAX_PLAYERS, MAX_CHANNELS, 0, 0);
	}
	else
	{
		host = enet_host_create(nullptr, 1, MAX_CHANNELS, 0, 0);
		enet_address_set_host(&address, hostName.c_str());
		address.port = GAME_PORT;

		Connect(hostName.c_str());
	}

	if (host == nullptr)
	{
		DEBUG_LOG("NET", LOG_ERROR, "An error occurred while trying to create an enet host.");
	}
}

NetworkHost::~NetworkHost()
{
	Disconnect();
	enet_host_destroy(host);
}

/// <summary>
/// Connects a client to a server.
/// </summary>
/// <param name="hostName">Host name of the server.</param>
void NetworkHost::Connect(const std::string& hostName)
{
	enet_address_set_host(&address, hostName.c_str());
	address.port = GAME_PORT;

	peer = enet_host_connect(host, &address, MAX_CHANNELS, 0);

	if (host == NULL)
	{
		DEBUG_LOG("NET", LOG_ERROR, "No available peers for initiating an enet connection.");
		return;
	}

	if (enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		DEBUG_LOG("NET", LOG_MSG, "Connection to server succeded.");
	}
	else
	{
		enet_peer_reset(peer);
		DEBUG_LOG("NET", LOG_MSG, "Connection to server failed.");
	}
}

void NetworkHost::SendPacket(const void* data, int32_t dataLength, enet_uint32 flags, int channelID)
{
	auto packet = enet_packet_create(data, dataLength, flags);

	if (peer != nullptr)
	{
		enet_peer_send(peer, channelID, packet);
	}
	else
	{
		enet_host_broadcast(host, channelID, packet);
	}
}

/// <summary>
/// Disconnects a client from the server.
/// </summary>
void NetworkHost::Disconnect()
{
	if (peer == nullptr) return;

	enet_peer_disconnect(peer, 0);

	while (enet_host_service(host, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			DEBUG_LOG("NET", LOG_MSG, "Disconnection succeded.");
			peer = nullptr;
			break;
		}
	}
}

void NetworkHost::Update(float deltaTime)
{
	while (enet_host_service(host, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT: // Server-only
			printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
			break;
		case ENET_EVENT_TYPE_RECEIVE: // Client or server
			printf("A packet of length %u containing %s was from %x:%u on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->address.host,
				event.peer->address.port,
				event.channelID);
			break;
		case ENET_EVENT_TYPE_DISCONNECT: // Server only
			printf("Client %x:%u disconnected.\n", event.peer->address.host, event.peer->address.port);
			event.peer->data = NULL;
			break;
		}
	}
}