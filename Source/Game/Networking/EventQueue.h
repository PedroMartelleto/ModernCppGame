#pragma once

#include "../../Engine/Engine.h"
#include <queue>
#include "Packet.h"
#include "Events.h"

#define MAX_TRANSFER_QUEUE_SIZE 100

using NID = uint16_t;

struct EventData
{
	EventType type = EventType::Zero;
	Ref<void> data = nullptr;
};

#define MAX_MESSAGE_QUEUE_EVENTS 1024

class EventQueue
{
public:
	EventQueue() {}
	void Enqueue(EventType type, const Ref<void>& data);
	EventData Dequeue();
	inline bool IsEmpty() const { return m_events.empty(); }
private:
	std::queue<EventData> m_events = std::queue<EventData>();
};