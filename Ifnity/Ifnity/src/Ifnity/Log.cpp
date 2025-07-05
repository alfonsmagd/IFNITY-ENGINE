#include "pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog\sinks\basic_file_sink.h>

namespace IFNITY
{
	
	std::shared_ptr<spdlog::logger> Log::log_AppLogger;
	std::shared_ptr<spdlog::logger> Log::log_CoreLogger;


	void Log::init()
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("IFNITY_LOG.txt", true);

		console_sink->set_pattern("%^[%n]: %v%$");
		file_sink->set_pattern("[%n]: %v");

		std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };

		log_AppLogger = std::make_shared<spdlog::logger>("[IFNITY-APP]", sinks.begin(), sinks.end());
		spdlog::register_logger(log_AppLogger);
		log_AppLogger->set_level(spdlog::level::trace);
		log_AppLogger->flush_on(spdlog::level::trace);

		log_CoreLogger = std::make_shared<spdlog::logger>("[IFNITY-CORE]", sinks.begin(), sinks.end());
		spdlog::register_logger(log_CoreLogger);
		log_CoreLogger->set_level(spdlog::level::trace);
		log_CoreLogger->flush_on(spdlog::level::trace);

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



