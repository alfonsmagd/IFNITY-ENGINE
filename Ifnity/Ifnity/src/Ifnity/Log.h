#pragma once



#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"



#define STRMESSAGE(msg, var) (std::string(msg) + std::string(var))

#define IFNITY_LOG(logger, level, fmt, ...) \
    do { \
        if (level == WARNING) { \
            logger->warn(fmt, ##__VA_ARGS__); \
        } else if (level == ERROR) { \
            logger->error("{} in function: {}", fmt, __FUNCTION__, ##__VA_ARGS__); \
        } else if (level == TRACE) { \
            logger->trace(fmt, ##__VA_ARGS__); \
        } else if (level == INFO) { \
            logger->info(fmt, ##__VA_ARGS__); \
        } else { \
            logger->info(fmt, ##__VA_ARGS__); \
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

    void Assert(bool condition, const char* file, int line, const char* format, ...);

	
}

#define LogApp  IFNITY::Log::GetAppLogger()
#define LogCore	IFNITY::Log::GetCoreLogger()
#define WARNING spdlog::level::warn
#define INFO    spdlog::level::info
#define ERROR   spdlog::level::err
#define TRACE   spdlog::level::trace

#define IFNITY_ASSERT_MSG(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            IFNITY::Assert(false, __FILE__, __LINE__, (fmt), ##__VA_ARGS__); \
        } \
    } while (0)

#define IFNITY_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            IFNITY::Assert(false, __FILE__, __LINE__, "Assertion failed: " #cond); \
        } \
    } while (0)

#define IFNITY_VERIFY(cond) ((cond) ? true : (IFNITY::Assert(false, __FILE__, __LINE__, #cond), false))

