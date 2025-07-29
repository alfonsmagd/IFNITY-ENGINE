#include "pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace IFNITY
{
	
	std::shared_ptr<spdlog::logger> Log::log_AppLogger;
	std::shared_ptr<spdlog::logger> Log::log_CoreLogger;


	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n %v%$");

		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
		auto file_sink_app  = std::make_shared<spdlog::sinks::basic_file_sink_mt>("ifnity_app.log", true);
		auto file_sink_core = std::make_shared<spdlog::sinks::basic_file_sink_mt>("ifnity_core.log", true);

		// Asignamos formato limpio a los archivos (sin color)
		file_sink_app->set_pattern("[%T] %n %v");
		file_sink_core->set_pattern("[%T] %n %v");
		console_sink->set_pattern("%^[%T] %n %v%$");

		// Crear loggers como en tu estilo original, pero combinando sinks
		log_AppLogger = std::make_shared<spdlog::logger>("[IFNITY-APP]", spdlog::sinks_init_list{ console_sink, file_sink_app });
		log_AppLogger->set_level(spdlog::level::trace);

		log_CoreLogger = std::make_shared<spdlog::logger>("[IFNITY-CORE]", spdlog::sinks_init_list{ console_sink, file_sink_core });
		log_CoreLogger->set_level(spdlog::level::trace);



		spdlog::register_logger(log_AppLogger);
		spdlog::register_logger(log_CoreLogger);

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



