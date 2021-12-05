#pragma once

#include <semaphore>
#include <mutex>

/// <summary>
/// A queue that can be accessed from multiple threads at the same time.
/// </summary>
template <typename _Type, size_t _Capacity>
class ThreadSafeQueue
{
public:
	// Enqueues a new item (producer pattern)
	void Enqueue(const _Type& item)
	{
		m_fullSemaphore.acquire();
		m_mutex.lock();

		data[m_start] = item;
		m_start = (m_start + 1) % _Capacity;

		m_mutex.unlock();
		m_emptySemaphore.release();
	}

	// Dequeues an item (consumer pattern)
	void Dequeue(_Type& item)
	{
		m_emptySemaphore.acquire();
		m_mutex.lock();

		item = data[m_end];
		m_end = (m_end + 1) % _Capacity;

		m_mutex.unlock();
		m_fullSemaphore.release();
	}
private:
	std::counting_semaphore<_Capacity> m_fullSemaphore = std::counting_semaphore<_Capacity>(_Capacity);
	std::counting_semaphore<_Capacity> m_emptySemaphore = std::counting_semaphore<_Capacity>(0);

	std::mutex m_mutex = std::mutex();
	_Type data[_Capacity];
	size_t m_start = 0;
	size_t m_end = 0;
};