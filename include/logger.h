#pragma once

class Logger
{
    public: 
        enum class LogLevel { DEBUG, INFO, WARN, ERROR };
        static void log(LogLevel level, const char* message);

        template<typename... Args>
        static void logf(LogLevel level,const char *format, Args... args)
        {
            char buffer[256];
            const int size =  snprintf(buffer,sizeof(buffer), format,args...);
            if(size > 0)
            {
                log(level, buffer);
            }
            else
            {
                log(LogLevel::ERROR, "Logging format error");
            }
        }

        const static char* LOG_LEVEL_STR(LogLevel level)
        {
            switch(level)
            {
                case LogLevel::DEBUG: return "DEBUG";
                case LogLevel::INFO:  return "INFO";
                case LogLevel::WARN:  return "WARN";
                case LogLevel::ERROR: return "ERROR";
                default:              return "UNKNOWN";
            }
        }

    private:
        Logger() = delete; // Prevent instantiation
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
};