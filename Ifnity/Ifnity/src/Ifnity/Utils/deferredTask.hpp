#pragma once

#include <future>


IFNITY_NAMESPACE


template <typename FinishHandle>
struct DeferredTask
{
	//Default Constructors.
	DeferredTask(std::packaged_task<void()>&& task, FinishHandle handle): 
		task_(std::move(task)), FinishHandle(handle) {}


	std::packaged_task<void()> task_;
	FinishHandle handle_;
};




IFNITY_END_NAMESPACE