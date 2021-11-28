#pragma once

#include "../Commons.h"

class JobSystem;

typedef std::function<void()> JobFunction;

class Job
{
public:
	Job(const JobFunction& callback) : callback(callback) {}

	/// <summary>
	/// If this job has already been executed, returns immediately.
	/// Otherwise, helps the system thread that is supposed to run this job by becoming a consumer of that thread's jobs.
	/// This function should be called exclusively on the main thread. 
	/// </summary>
	void Wait();

	/// <summary>
	/// Schedules this job to run on some system thread. Call this before wait.
	/// This function should be called exclusively on the main thread.
	/// </summary>
	void Schedule();

	/// <summary>
	/// Pointer to a callback that executes the actual job. May be accessed by any thread.
	/// </summary>
	JobFunction callback;
	
	/// <summary>
	/// This bool is set to true as soon as the Job is done. May be accessed by any thread.
	/// </summary>
	std::atomic_bool isComplete = false;

	/// <summary>
	/// The thread index set as soon as this Job is scheduled to a system thread. Accessed exclusively from the main thread.
	/// </summary>
	int threadIndex = -1;
};