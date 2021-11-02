#pragma once

#include "../../Engine/Engine.h"

/// <summary>
/// Helper class to deal with ENet UDP networking.
/// </summary>
class NetworkHost
{
public:
	static const int GAME_PORT = 7777;
	static const int MAX_PLAYERS = 8;
	static const int MAX_CHANNELS = 1;

	ENetHost* host = nullptr;
	ENetPeer* peer = nullptr;
	ENetAddress address = ENetAddress();
	ENetEvent event = ENetEvent();
	HostType type = HostType::NONE;

	NetworkHost(HostType type, const std::string& hostName = "");
	~NetworkHost();

	/// <summary>
	/// Connects a client to a server.
	/// </summary>
	/// <param name="hostName">Host name of the server.</param>
	void Connect(const std::string& hostName);

	/// <summary>
	/// Sends a packet. If host type is client, sends to the server. Otherwise, broadcasts to all clients.
	/// </summary>
	void SendPacket(const void* data, int32_t dataLength, enet_uint32 flags, int channelID);

	/// <summary>
	/// Disconnects a client from the server.
	/// </summary>
	void Disconnect();

	void Update(float deltaTime);
};

