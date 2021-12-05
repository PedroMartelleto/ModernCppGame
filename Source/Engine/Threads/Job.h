#pragma once

#include "../Commons.h"
#include <semaphore>
#include <mutex>

class JobSystem;

typedef std::function<void()> JobFunction;

class Job
{
public:
	Job() {}

	Job(const JobFunction& callback)
	{
		this->callback = callback;
	}

	/// <summary>
	/// If this job has already been executed, returns immediately.
	/// Otherwise, the calling thread helps the other threads by executing other jobs from the pool until this job is executed.
	/// This function should be called exclusively on the main thread. 
	/// </summary>
	void Wait();

	/// <summary>
	/// Schedules this job to run on some system thread. Call this before wait.
	/// This function should be called exclusively on the main thread.
	/// </summary>
	Job* Schedule();

	/// <summary>
	/// Pointer to a callback that executes the actual job. May be accessed by any thread.
	/// </summary>
	JobFunction callback;

	Job(const Job&) = delete;
	Job& operator=(const Job&) = delete;
private:
	/// <summary>
	/// This bool is set to true as soon as the Job is done. May be accessed by any thread.
	/// </summary>
	std::binary_semaphore m_isCompleteSemaphore = std::binary_semaphore(false);
};