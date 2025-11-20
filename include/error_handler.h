#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <functional>

// Logging functions
void log_entry(const char* function);
void log_exit(const char* function);
void log_error(int code, const char* message);
void log_severity(int level, const char* message);
void log_performance(const char* metric, float value);

// Network retry logic
bool retry_network_operation(std::function<bool()> operation, int max_retries = 3);

#endif // ERROR_HANDLER_H