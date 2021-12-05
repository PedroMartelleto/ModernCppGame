#include "Job.h"
#include "JobSystem.h"
#include <assert.h>

void Job::Wait()
{
	JobFunction jobFn;

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
	JobSystem::master->Execute([this]()
	{
		callback();
		m_isCompleteSemaphore.release();
	});
	return this;
}