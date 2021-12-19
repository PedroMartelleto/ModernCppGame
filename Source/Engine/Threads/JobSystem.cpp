#include "JobSystem.h"
#include "../Core/Logger.h"

/// Number of threads reserved for the Operating System.
#define NUM_RESERVED_THREADS 0

Ref<JobSystem> JobSystem::master = CreateRef<JobSystem>();

JobSystem::JobSystem()
{
	// Spawns the maximum amount of parallel threads that this system supports
	m_threadCount = (int)std::thread::hardware_concurrency() - 1 - NUM_RESERVED_THREADS;
	m_threadCount = std::clamp<int>(m_threadCount, MIN_THREADS, MAX_THREADS);

	m_isRunning = new std::atomic_bool[m_threadCount];

	for (int i = 0; i < m_threadCount; ++i)
	{
		m_isRunning[i] = true;
		std::thread worker(&JobSystem::ThreadCallback, this, i);
		worker.detach();
	}

	DEBUG_LOG("THREADS", LOG_MSG, "Spawning %d threads...", m_threadCount);
}

void JobSystem::Wait()
{
	JobFunction jobFn;

	while (m_jobCounter < m_finishedJobCounter)
	{
		// Dequeues a job and runs it
		m_jobsQueue.Dequeue(jobFn);
		jobFn();
		m_finishedJobCounter.fetch_add(1);
	}
}

void JobSystem::Execute(const JobFunction& jobFn)
{
	m_jobCounter += 1;
	m_jobsQueue.Enqueue(jobFn);
}

void JobSystem::ThreadCallback(int i)
{
	// Thread callback (everything here is run on a separate thread)

	JobFunction jobFn;

	// While this thread is enabled...
	while (m_isRunning[i].load())
	{
		// Dequeues a job and runs it
		m_jobsQueue.Dequeue(jobFn);
		jobFn();
		m_finishedJobCounter.fetch_add(1);
	}
}