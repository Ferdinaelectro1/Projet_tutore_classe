#include "logger.h"
#include <Arduino.h>

void Logger::log(const LogLevel level, const char* message)
{
    const char* levelStr = LOG_LEVEL_STR(level);
    Serial.print("[");
    Serial.print(levelStr);
    Serial.print("] ");
    Serial.println(message);
}


