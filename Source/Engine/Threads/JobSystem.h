#pragma once

#include "../Commons.h"
#include "Job.h"
#include <queue>
#include <semaphore>

#define MAX_JOBS_PER_THREAD 100

struct SystemThread
{
	std::counting_semaphore<MAX_JOBS_PER_THREAD> emptySemaphore = std::counting_semaphore<MAX_JOBS_PER_THREAD>(0);
	std::counting_semaphore<MAX_JOBS_PER_THREAD> fullSemaphore = std::counting_semaphore<MAX_JOBS_PER_THREAD>(MAX_JOBS_PER_THREAD);
	std::thread* thread;
	std::atomic_bool isEnabled = true;
	std::queue<Job> jobs = std::queue<Job>();
	std::binary_semaphore mutex = std::binary_semaphore(1);

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
	JobSystem();
	~JobSystem();

	void AssignJob(const Job& job);
private:
	void ThreadCallback(int i);

	std::vector<SystemThread*> m_systemThreads;
};
