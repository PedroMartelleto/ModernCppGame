#include "NetworkHost.h"
#include "../GameCore.h"
#include "../GameData.h"
#include "../ECS/Components.h"

#define CONNECT_TIMEOUT 5000
#define DISCONNECT_TIMEOUT 3000

NetworkHost::NetworkHost(GameCore* gameCore, HostType type, const std::string& hostName) :
	gameCore(gameCore),
	type(type)
{
	assert(type != HostType::NONE);

	if (type == HostType::SERVER)
	{
		assert(hostName.length() > 0);
		address.host = ENET_HOST_ANY;
		address.port = GAME_PORT;
		m_host = enet_host_create(&address, MAX_PLAYERS, MAX_CHANNELS, 0, 0);
		m_host->checksum = enet_crc32;
	}
	else
	{
		m_host = enet_host_create(nullptr, 1, MAX_CHANNELS, 0, 0);
		enet_address_set_host(&address, hostName.c_str());
		address.port = GAME_PORT;
		m_host->checksum = enet_crc32;
	}

	if (m_host == nullptr)
	{
		DEBUG_LOG("NET", LOG_ERROR, "An error occurred while trying to create an enet host.");
		return;
	}

	if (type == HostType::CLIENT)
	{
		Connect(hostName.c_str(), 0);
	}
}

NetworkHost::~NetworkHost()
{
	enet_host_destroy(m_host);
}

/// <summary>
/// Connects a client to a server.
/// </summary>
/// <param name="hostName">Host name of the server.</param>
void NetworkHost::Connect(const std::string& hostName, uint8_t mobTypeID)
{
	enet_address_set_host(&address, hostName.c_str());
	address.port = GAME_PORT;

	peer = enet_host_connect(m_host, &address, MAX_CHANNELS, mobTypeID);

	if (m_host == NULL)
	{
		DEBUG_LOG("NET", LOG_ERROR, "No available peers for initiating an enet connection.");
		return;
	}

	ENetEvent event;
	if (enet_host_service(m_host, &event, CONNECT_TIMEOUT) > 0)
	{
		if (event.type == ENET_EVENT_TYPE_CONNECT)
		{
			DEBUG_LOG("NET", LOG_MSG, "Client connection successful.");
			return;
		}
	}
}

void NetworkHost::SendPacket(const NetworkBuffer& buffer, enet_uint32 flags, int channelID) const
{
	auto packet = enet_packet_create(&buffer[0], buffer.size(), flags);

	if (peer != nullptr)
	{
		enet_peer_send(peer, channelID, packet);
	}
	else
	{
		enet_host_broadcast(m_host, channelID, packet);
	}
}

void NetworkHost::SendPacket(const PacketData& packetData, enet_uint32 flags, int channelID) const
{
	json dataToSend;
	Serialization::to_json(dataToSend, packetData);
	const NetworkBuffer& buffer = json::to_cbor(dataToSend);
	SendPacket(buffer, flags, channelID);
}

/// <summary>
/// Disconnects a client from the server.
/// </summary>
void NetworkHost::Disconnect()
{
	if (peer == nullptr) return;

	enet_peer_disconnect(peer, 0);

	ENetEvent event;

	// Poll events
	if (enet_host_service(m_host, &event, DISCONNECT_TIMEOUT) > 0 && event.type == ENET_EVENT_TYPE_DISCONNECT)
	{
		DEBUG_LOG("NET", LOG_MSG, "Disconnected from server successfully.");
	}
}

void NetworkHost::HandlePacket(const ENetEvent& event)
{
	auto packet = event.packet;

	if (packet == nullptr) return;

	//printf("[%s] A packet of length %u was received from %x:%u on channel %u with contents \n[", std::string(magic_enum::enum_name(type)).c_str(), event.packet->dataLength, event.peer->address.host, event.peer->address.port, event.channelID);
	
	auto data = (uint8_t*)packet->data;
	auto bytes = NetworkBuffer(data, data + packet->dataLength);
	json receivedData = json::from_cbor(bytes);
	PacketData packetData;
	Serialization::from_json(receivedData, packetData);

	if (packetData.events.size() != packetData.types.size()) return;

	for (uint32_t i = 0; i < packetData.events.size(); ++i)
	{
		EventType packetType = packetData.types[i];
		const json& j = packetData.events[i];
		//printf("%s", std::string(magic_enum::enum_name(packetType)).c_str());

		if (i + 1 < packetData.events.size()) printf(", ");

		switch (packetType)
		{
		case EventType::WorldSnapshot:
			eventQueue.Enqueue(packetType, Utils::RefFromJSON<WorldSnapshotEvent>(j));
			break;
		case EventType::Map:
			eventQueue.Enqueue(packetType, Utils::RefFromJSON<MapDataEvent>(j));
			break;
		case EventType::SpawnPlayer:
			eventQueue.Enqueue(packetType, Utils::RefFromJSON<SpawnPlayerEvent>(j));
			break;
		case EventType::MobInputs:
			eventQueue.Enqueue(packetType, Utils::RefFromJSON<MobInputsEvent>(j));
			break;
		}
	}
	//printf("]\n");
}

void NetworkHost::ServerHandleNewConnection(ENetPeer* peer, uint8_t mobTypeID)
{
	// Sends spawn player request to spawn the current players
	std::vector<json> events;
	std::vector<EventType> eventTypes;

	events.push_back(Utils::ToJSON(MapDataEvent{ Utils::LoadFile(gameCore->mapFilepath) }));
	eventTypes.push_back(EventType::Map);

	for (const auto& [mobID, entity]: gameCore->mobs)
	{
		const auto& mobComponent = gameCore->registry.get<MobComponent>(entity);
		const auto* localInputComponent = gameCore->registry.try_get<LocalInputComponent>(entity);
		auto hostType = localInputComponent == nullptr ? HostType::CLIENT : HostType::SERVER;

		if (mobComponent.isPlayer)
		{
			events.push_back(Utils::ToJSON(SpawnPlayerEvent{ mobComponent.mobID, hostType, 8, 8, mobComponent.name }));
			eventTypes.push_back(EventType::SpawnPlayer);
		}
	}

	// Also adds the newly connected player for spawning
	auto newPlayerSpawnEvent = SpawnPlayerEvent{ gameCore->CreateMobID(), HostType::CLIENT, 12, 8, GameData::GetMobNameFromTypeID(mobTypeID) };
	events.push_back(Utils::ToJSON(newPlayerSpawnEvent));
	eventTypes.push_back(EventType::SpawnPlayer);

	// Add the player locally (to this server)
	eventQueue.Enqueue(EventType::SpawnPlayer, CreateRef<SpawnPlayerEvent>(newPlayerSpawnEvent));

	auto packetData = PacketData{events, eventTypes};

	// Sends response to client
	json packetJson;
	Serialization::to_json(packetJson, packetData);
	NetworkBuffer bufferToSend = json::to_cbor(packetJson);
	auto packet = enet_packet_create(&bufferToSend[0], bufferToSend.size(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void NetworkHost::PollEvents()
{
	ENetEvent event;

	// Poll events
	while (enet_host_service(m_host, &event, 0) > 0)
	{
		if (event.type == ENET_EVENT_TYPE_CONNECT)
		{
			printf("[%s] A new client connected from %x:%u.\n", std::string(magic_enum::enum_name(type)).c_str(), event.peer->address.host, event.peer->address.port);
			if (type == HostType::SERVER) ServerHandleNewConnection(event.peer, (uint8_t)event.data);
		}
		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			HandlePacket(event);
			enet_packet_destroy(event.packet);
		}
		else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
		{
			printf("[%s] Client %x:%u disconnected.\n", std::string(magic_enum::enum_name(type)).c_str(), event.peer->address.host, event.peer->address.port);
			event.peer->data = NULL;
		}
	}
}