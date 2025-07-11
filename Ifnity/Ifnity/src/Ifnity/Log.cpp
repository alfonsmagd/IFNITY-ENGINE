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


	void Assert(bool condition, const char* file, int line, const char* format, ...)
	{
		if(!condition)
		{
			va_list args;
			va_start(args, format);
			Log::GetCoreLogger()->error("Assertion failed at {}:{}: ", file, line);
            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), format, args);
            Log::GetCoreLogger()->error(buffer);
			va_end(args);
			std::abort(); // Termina el programa
		}
	}

}



