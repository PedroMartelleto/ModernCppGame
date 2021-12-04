#include "EventQueue.h"

void EventQueue::Enqueue(EventType type, const Ref<void>& m_data)
{
	m_events.push(EventData{ type, m_data });
}

EventData EventQueue::Dequeue()
{
	auto back = m_events.front();
	m_events.pop();
	return back;
}