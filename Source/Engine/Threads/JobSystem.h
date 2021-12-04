#pragma once

#include "../Commons.h"
#include "Job.h"
#include <queue>
#include <semaphore>

#define MAX_JOBS_PER_THREAD 100

/// <summary>
/// A system thread is a job consumer. A fixed number of system threads are created during initialization.
/// The number of system threads equals to the number of threads that can run parallel in the system.
/// </summary>
struct SystemThread
{
	std::counting_semaphore<MAX_JOBS_PER_THREAD> m_emptySemaphore = std::counting_semaphore<MAX_JOBS_PER_THREAD>(0);
	std::counting_semaphore<MAX_JOBS_PER_THREAD> m_fullSemaphore = std::counting_semaphore<MAX_JOBS_PER_THREAD>(MAX_JOBS_PER_THREAD);
	std::thread* thread;
	std::atomic_bool isEnabled = true;
	std::queue<Job*> jobs = std::queue<Job*>();
	std::binary_semaphore m_mutex = std::binary_semaphore(1);

	SystemThread(std::thread* thread) : thread(thread) {}

	SystemThread(const SystemThread& other) = delete;
	void operator=(const SystemThread& other) = delete;

	~SystemThread()
	{
		delete thread;
	}
};

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
	void AssignJobToSystemThreads(Job* job);

	JobSystem();
	~JobSystem();

private:
	/// <summary>
	/// std::thread callback.
	/// </summary>
	void ThreadCallback(int i);

	/// <summary>
	/// Array of system threads with a fixed size. Jobs scheduled are assigned to one of these threads.
	/// Therefore, each thread is part of a consumer-producer pattern.
	/// Producer: main thread.
	/// Consumer: threads which are not the main one.
	/// </summary>
	std::vector<SystemThread*> m_systemThreads;

	friend class Job;
};
