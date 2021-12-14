#include "Job.h"
#include "JobSystem.h"
#include <assert.h>

void Job::Wait()
{
	JobFunction jobFn;

	// While we need to wait...
	while (!m_isCompleteSemaphore.try_acquire())
	{
		// Dequeues a job and runs it
		JobSystem::master->m_jobsQueue.Dequeue(jobFn);
		jobFn();
		JobSystem::master->m_finishedJobCounter.fetch_add(1);
	}
}

Job* Job::Schedule()
{
	if (JobSystem::master->m_threadCount <= 0)
	{
		callback();
		m_isCompleteSemaphore.release();
	}
	else
	{
		JobSystem::master->Execute([this]()
		{
			callback();
			m_isCompleteSemaphore.release();
		});
	}

	return this;
}