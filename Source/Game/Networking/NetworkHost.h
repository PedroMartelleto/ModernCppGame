#pragma once

#include "../../Engine/Engine.h"
#include "Packet.h"
#include "BitBuffer.h"
#include "EventQueue.h"

/// <summary>
/// Helper class to deal with ENet UDP networking.
/// </summary>
class NetworkHost
{
public:
	static const int GAME_PORT = 8888;
	static const int MAX_PLAYERS = 8;
	static const int MAX_CHANNELS = 2;

	ENetPeer* peer = nullptr;
	ENetAddress address = ENetAddress();
	const HostType type;
	GameCore* gameCore;
	EventQueue eventQueue;

	NetworkHost(GameCore* gameCore, HostType type, const std::string& hostName = "");
	~NetworkHost();

	/// <summary>
	/// Connects a client to a server.
	/// </summary>
	/// <param name="hostName">Host name of the server.</param>
	void Connect(const std::string& hostName, uint8_t mobTypeID);

	/// <summary>
	/// Sends a packet. If host type is client, sends to the server. Otherwise, broadcasts to all clients.
	/// </summary>
	void SendPacket(const PacketData& packetData, enet_uint32 flags, int channelID) const;

	/// <summary>
	/// Sends a packet. If host type is client, sends to the server. Otherwise, broadcasts to all clients.
	/// </summary>
	void SendPacket(const NetworkBuffer& buffer, enet_uint32 flags, int channelID) const;
	
	void PollEvents();

	/// <summary>
	/// Disconnects a client from the server.
	/// </summary>
	void Disconnect();
private:
	ENetHost* m_host = nullptr;

	void HandlePacket(const ENetEvent& event);
	void ServerHandleNewConnection(ENetPeer* peer, uint8_t mobTypeID);
};

