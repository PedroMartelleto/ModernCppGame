#include "NetworkHost.h"

NetworkHost::NetworkHost(HostType type, const std::string& hostName) :
	m_eventPollPauseSemaphore(1)
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
}

void NetworkHost::SendPacket(const NetworkBuffer& buffer, enet_uint32 flags, int channelID)
{
	auto packet = enet_packet_create(&buffer[0], buffer.size(), flags);

	if (peer != nullptr)
	{
		enet_peer_send(peer, channelID, packet);
	}
	else
	{
		enet_host_broadcast(host, channelID, packet);
	}
}

void NetworkHost::SendPacket(const BitBuffer8& bitBuffer, PacketType type, enet_uint32 flags, int channelID)
{
	NetworkBuffer bufferToSend = { (uint8_t)type, bitBuffer.bits };
	SendPacket(bufferToSend, flags, channelID);
}

/// <summary>
/// Disconnects a client from the server.
/// </summary>
void NetworkHost::Disconnect()
{
	if (peer == nullptr) return;

	enet_peer_disconnect(peer, 0);

	ENetEvent event;

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

void HandlePacket(const ENetEvent& event)
{
	auto packetData = event.packet->data;
	auto dataLength = event.packet->dataLength;
	auto packetType = (PacketType)packetData[0];

	if (packetType < PacketType::Zero || packetType > PacketType::Max || dataLength <= 1) return;

	auto bitBuffer = BitBuffer8(packetData[1]);
	std::string packetName = std::string(magic_enum::enum_name(packetType));
	std::string  bitBufferString = bitBuffer.ToString();
	printf("A packet of length %u containing [%s %s] was received from %x:%u on channel %u.\n",
		dataLength,
		packetName.c_str(),
		bitBufferString.c_str(),
		event.peer->address.host,
		event.peer->address.port,
		event.channelID);
}

void NetworkHost::StartEventMonitoring()
{
	if (m_eventPoll == nullptr)
	{
		m_eventPoll = CreateRef<std::thread>(&NetworkHost::ProduceEvents, this);
	}
}

void NetworkHost::StopEventMonitoring()
{
	// Requests the thread to stop and waits for it to happen.
	m_stopPollStopRequest = true;
	m_eventPoll->join();
	m_eventPoll = nullptr;
}

void NetworkHost::ProduceEvents()
{
	ENetEvent event;

	while (!m_stopPollStopRequest)
	{
		// If there is no pause request, this semaphore does nothing.
		// If there is a pause request from the main thread, then this thread sleeps.
		// It needs to be woken up by the main thread.
		m_eventPollPauseSemaphore.acquire();

		// Poll events
		while (!m_stopPollStopRequest && enet_host_service(host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
				// TODO: Make the player create the world
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				HandlePacket(event);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("Client %x:%u disconnected.\n", event.peer->address.host, event.peer->address.port);
				event.peer->data = NULL;
				break;
			}

			if (m_stopPollStopRequest) break;
		}

		m_eventPollPauseSemaphore.release();
	}
}