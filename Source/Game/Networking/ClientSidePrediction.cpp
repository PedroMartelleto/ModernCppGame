#include "ClientSidePrediction.h"
#include "../GameCore.h"
#include "../GameData.h"

void ClientSidePrediction::RegisterSnapshot(const Ref<WorldSnapshotEvent>& snapshot)
{
	m_startCounter = true;
	m_pendingSnapshot = snapshot;
}

void ClientSidePrediction::Update(float deltaTime)
{
	if (m_clientFrameCounter % GameData::GetWorldSnapshotTickRate() == 0)
	{
		// Handles receiving packets on-time (or early)
		if (m_pendingSnapshot != nullptr)
		{
			m_pendingSnapshot->Load(m_gameCore);
			m_pendingSnapshot = nullptr;
			m_packetLateCounter = -1;
		}
		else
		{
			m_packetLateCounter = 0;
		}
	}

	// Handles receiving a packet too late
	if (m_packetLateCounter >= 1 && m_pendingSnapshot != nullptr)
	{
		m_pendingSnapshot->Load(m_gameCore);

		while (m_packetLateCounter >= 1)
		{
			m_gameCore->PhysicsStep(deltaTime);
			m_packetLateCounter -= 1;
		}

		m_pendingSnapshot = nullptr;
		m_packetLateCounter = -1;
	}

	// Counters used for synchronization
	if (m_startCounter)
	{
		m_clientFrameCounter += 1;

		if (m_packetLateCounter >= 0)
		{
			m_packetLateCounter += 1;
		}
	}
}