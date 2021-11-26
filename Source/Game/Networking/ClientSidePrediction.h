#pragma once

#include "../../Engine/Engine.h"
#include "../Physics/WorldSnapshotEvent.h"

class GameCore;

class ClientSidePrediction
{
public:
	ClientSidePrediction(GameCore* gameCore) : m_gameCore(gameCore) {}

	void RegisterSnapshot(const Ref<WorldSnapshotEvent>& snapshot);
	void Update(float deltaTime);
private:
	/// <summary>
	/// Snapshot of the physics world from the last received physics frame.
	/// </summary>
	Ref<WorldSnapshotEvent> m_pendingSnapshot = nullptr;

	uint64_t m_clientFrameCounter = 0;
	uint64_t m_lastPacketRegistered = 0;
	bool m_startCounter = false;
	int8_t m_packetLateCounter = -1;

	GameCore* m_gameCore;
};