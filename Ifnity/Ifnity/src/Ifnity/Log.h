#pragma once



#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"



#define IFNITY_LOG(logger, level,  ...) \
        do { \
        if (level == WARNING) { \
            logger->warn(__VA_ARGS__); \
        } else if (level == ERROR) { \
            logger->error(__VA_ARGS__); \
        } else if (level == TRACE) { \
            logger->trace(__VA_ARGS__); \
        } else if (level == INFO) { \
            logger->info(__VA_ARGS__); \
        } else { \
            logger->info(__VA_ARGS__); \
        } \
    } while (0)
namespace IFNITY
{

	class  IFNITY_API  Log
	{
	public:
			static void init();

			IFNITY_INLINE  static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return log_CoreLogger; }
			IFNITY_INLINE  static std::shared_ptr<spdlog::logger>& GetAppLogger()  { return log_AppLogger; }


	private :

		static std::shared_ptr<spdlog::logger> log_CoreLogger;
		static std::shared_ptr<spdlog::logger> log_AppLogger;

		
	};

	
}

#define LogApp  IFNITY::Log::GetAppLogger()
#define LogCore	IFNITY::Log::GetCoreLogger()
#define WARNING spdlog::level::warn
#define INFO    spdlog::level::info
#define ERROR   spdlog::level::err
#define TRACE   spdlog::level::trace