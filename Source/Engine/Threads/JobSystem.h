#pragma once

#include "../Commons.h"
#include "Job.h"
#include <queue>
#include <semaphore>
#include "ThreadSafeQueue.h"

#define MAX_JOBS 256
#define MAX_THREADS 100

class JobSystem
{
public:
	/// <summary>
	/// We only need one job system, so we create a static system.
	/// </summary>
	static Ref<JobSystem> master;
	
	/// <summary>
	/// Assigns a job to run in this job system.
	/// </summary>
	void Execute(const JobFunction& jobFn);

	/// <summary>
	/// Waits for all jobs to be completed.
	/// </summary>
	void Wait();

	JobSystem();
private:
	/// <summary>
	/// std::thread callback.
	/// </summary>
	void ThreadCallback(int i);

	ThreadSafeQueue<JobFunction, MAX_JOBS> m_jobsQueue;
	std::atomic<bool>* m_isRunning;
	int m_threadCount;

	uint64_t m_jobCounter = 0;
	std::atomic<uint64_t> m_finishedJobCounter = 0;

	friend class Job;
};
