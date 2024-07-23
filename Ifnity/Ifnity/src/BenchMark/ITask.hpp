#include "pch.h"


IFNITY_NAMESPACE
namespace benchmark{
	/*
	* Enum TaskState 
	*/
	enum TaskState
	{
		ACTIVE,
		AGAIN,
		INACTIVE
	};
	
	/// <summary>
	/// Interface for Task 
	/// </summary>
	class ITask
	{
	public:
		//Default Destructor
		virtual ~ITask() = default;
	
		//Run the task
		virtual void Run() = 0;
		virtual void Stop() = 0;
		virtual TaskState Execute() = 0;
	};

	/// <summary>
	/// Scheduler class to manage the different tasks 
	/// </summary>
	class Scheduler
	{
		//Forward Declaration
		struct TaskData;
	public:
		//Default Destructor
		 ~Scheduler() = default;

		//Run the task
		void AddTask(TaskData t , float period) ;
		void RemoveTask(TaskData t);
		void StopTask(TaskData t);
		void ResumeTask(TaskData t);
		void ResetAllTasks();
		void ResetTask(TaskData t);
		void SetPeriod(TaskData t, float period);
		void StopAllTasks();
		void Execute();
	private:

		struct TaskData
		{
			std::shared_ptr<ITask> task;
			float timer;
			float period;
			bool  active;
			
		};

		std::vector<TaskData> m_tasks;
	};

} //end namespace BenchMark
IFNITY_END_NAMESPACE