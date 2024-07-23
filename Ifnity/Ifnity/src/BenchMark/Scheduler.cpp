
#include "ITask.hpp"


IFNITY_NAMESPACE
namespace benchmark
{
	void Scheduler::AddTask(TaskData t, float period)
	{
		if(t.task != nullptr)
		{
			t.timer = period;
			t.period = period;
			t.active = true;
			m_tasks.push_back(t);
		}
	}

	void Scheduler::RemoveTask(TaskData t)
	{
		if(t.task != nullptr)
		{
			t.active = false;
			m_tasks.erase(std::remove_if(m_tasks.begin(), m_tasks.end(),
				[](const TaskData& t) { return !t.active; }), m_tasks.end());


		}
	}

	void Scheduler::StopTask(TaskData t)
	{
		if(t.task != nullptr)
		{
			t.active = false;
			t.task->Stop();
		}

	}

	void Scheduler::ResumeTask(TaskData t)
	{
		if(t.task != nullptr)
		{
			t.active = true;
			t.task->Run();
		}
	}

	void Scheduler::ResetAllTasks()
	{
		for(auto& t : m_tasks)
		{
			t.active = false;
			t.timer = t.period;
			t.task->Stop();
		}
	}

	void Scheduler::ResetTask(TaskData t)
	{
		if(t.task != nullptr)
		{
			t.active = false;
			t.timer = t.period;
			t.task->Stop();
		}
	}



	void Scheduler::SetPeriod(TaskData t, float period)
	{
		if(t.task != nullptr)
		{
			t.period = period;

		}
	}

	void Scheduler::StopAllTasks()
	{
		for(auto& t : m_tasks)
		{
			t.active = false;
			t.task->Stop();
		}
	}

	//This is more complecate 
	void Scheduler::Execute()
	{



	}


} //end namespace BenchMark
IFNITY_END_NAMESPACE