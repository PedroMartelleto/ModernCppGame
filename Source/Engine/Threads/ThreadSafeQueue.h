#pragma once

#include <semaphore>
#include <mutex>

template <typename _Type, size_t _Capacity>
class ThreadSafeQueue
{
public:
	void Enqueue(const T& item)
	{
		m_fullSemaphore.acquire();
		m_mutex.lock();

		m_data[m_start] = item;
		m_start = (m_start + 1) % _Capacity;

		m_mutex.unlock();
		m_emptySemaphore.release();
	}

	void Dequeue(T& item)
	{
		m_emptySemaphore.acquire();
		m_mutex.lock();

		item = m_data[m_end];
		m_end = (m_end + 1) % _Capacity

		m_mutex.unlock();
		m_fullSemaphore.release();
	}
private:
	std::counting_semaphore<_Capacity> m_fullSemaphore = std::counting_semaphore<_Capacity>(_Capacity);
	std::counting_semaphore<_Capacity> m_emptySemaphore = std::counting_semaphore<_Capacity>(0);

	std::m_mutex m_mutex  = std::m_mutex();
	T m_data[_Capacity];
	size_t m_start = 0;
	size_t m_end = 0;
};

