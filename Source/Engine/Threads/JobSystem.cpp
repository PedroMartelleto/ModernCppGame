#include "JobSystem.h"

/// Number of threads reserved for the Operating System
#define NUM_RESERVED_THREADS 0

JobSystem::JobSystem()
{
	int numThreads = (int)std::thread::hardware_concurrency() - 1 - NUM_RESERVED_THREADS;

	if (numThreads <= 0)
	{
		numThreads = 1;
	}

	for (int i = 0; i < numThreads; ++i)
	{
		m_systemThreads.push_back(new SystemThread(new std::thread(&JobSystem::ThreadCallback, this, i)));
	}
}

JobSystem::~JobSystem()
{
	for (auto* systemThread : m_systemThreads)
	{
		// Pushes a "terminate" job to safely terminate the thread
		systemThread->fullSemaphore.acquire();
		systemThread->mutex.acquire();
		systemThread->isEnabled = false;

		systemThread->jobs.push([systemThread]()
		{
			systemThread->isEnabled = false;
		});

		systemThread->mutex.release();
		systemThread->emptySemaphore.release();

		// Waits for the thread to execute the TerminateThreadJob
		systemThread->thread->join();

		// Deletes allocated heap memory
		delete systemThread;
	}
}

void JobSystem::AssignJob(const Job& job)
{
	// TODO: Balance this...
	int destIndex = 0;

	for (int i = 1; i < (int)m_systemThreads.size(); ++i)
	{
		if (m_systemThreads[i]->jobs.size() < m_systemThreads[destIndex]->jobs.size())
		{
			destIndex = i;
		}
	}

	auto& systemThread = m_systemThreads[destIndex];

	systemThread->fullSemaphore.acquire();
	systemThread->mutex.acquire();

	systemThread->jobs.push(job);

	systemThread->mutex.release();
	systemThread->emptySemaphore.release();
}

void JobSystem::ThreadCallback(int i)
{
	auto* systemThread = m_systemThreads[i];

	while (systemThread->isEnabled)
	{
		systemThread->emptySemaphore.acquire();

		if (!systemThread->isEnabled) return;

		systemThread->mutex.acquire();

		if (!systemThread->isEnabled) return;

		auto jobToRun = systemThread->jobs.back();
		systemThread->jobs.pop();

		systemThread->mutex.release();
		systemThread->fullSemaphore.release();

		jobToRun();
	}
}