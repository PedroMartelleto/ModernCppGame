#include "EventQueue.h"

void EventQueue::Enqueue(EventType type, Ref<void> data)
{
	m_events.push(EventData{ type, data });
}

EventData EventQueue::Dequeue()
{
	auto back = m_events.front();
	m_events.pop();
	return back;
}