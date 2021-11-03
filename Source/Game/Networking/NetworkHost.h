#pragma once

#include "../../Engine/Engine.h"
#include "Packet.h"
#include "BitBuffer.h"

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
	void SendPacket(const NetworkBuffer& buffer, enet_uint32 flags, int channelID);

	/// <summary>
	/// Sends a bit buffer packet.
	/// </summary>
	void SendPacket(const BitBuffer8& bitBuffer, PacketType type, enet_uint32 flags, int channelID);

	/// <summary>
	/// Disconnects a client from the server.
	/// </summary>
	void Disconnect();

	/// <summary>
	/// Starts a new thread (producer) that produces events for the main thread (consumer).
	/// </summary>
	void StartEventMonitoring();

	/// <summary>
	/// Stops the thread created.
	/// </summary>
	void StopEventMonitoring();
private:
	Ref<std::thread> m_eventPoll = nullptr;
	std::binary_semaphore m_eventPollPauseSemaphore;
	bool m_stopPollStopRequest = false;

	void ProduceEvents();
};

