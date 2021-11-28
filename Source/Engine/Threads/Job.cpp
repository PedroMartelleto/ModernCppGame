#include "Job.h"
#include "JobSystem.h"
#include <assert.h>

void Job::Wait()
{
	assert(threadIndex >= 0);
	auto jobSystem = JobSystem::master;
	auto* systemThread = jobSystem->m_systemThreads[threadIndex];

	while (!isComplete)
	{
		// Consumer pattern, but with a twist:
			// If we ever find that there are no jobs to execute, we simply return.
			// It is safe to return since the job we've been waiting for has already been executed.
		if (!systemThread->emptySemaphore.try_acquire()) return;

		systemThread->mutex.acquire();

		auto jobToRun = systemThread->jobs.back();
		systemThread->jobs.pop();
		jobToRun->threadIndex = threadIndex;

		systemThread->mutex.release();
		systemThread->fullSemaphore.release();

		jobToRun->callback();
		jobToRun->isComplete = true;
		delete jobToRun;
	}
}

void Job::Schedule()
{
	JobSystem::master->AssignJobToSystemThreads(this);
}