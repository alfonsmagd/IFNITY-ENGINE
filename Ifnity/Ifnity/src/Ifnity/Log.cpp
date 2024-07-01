#include "pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace IFNITY
{
	
	std::shared_ptr<spdlog::logger> Log::log_AppLogger;
	std::shared_ptr<spdlog::logger> Log::log_CoreLogger;


	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n %v%$");

		log_AppLogger = spdlog::stdout_color_mt("[IFNITY-APP]");
		log_AppLogger->set_level(spdlog::level::trace);

		log_CoreLogger = spdlog::stdout_color_mt("[IFNITY-CORE]");
		log_CoreLogger->set_level(spdlog::level::trace);

	}

}



