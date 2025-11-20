// Logging and error management
#include "error_handler.h"
#include <iostream>

void log_entry(const char* function) {
    std::cout << "[ENTRY] " << function << std::endl;
}

void log_exit(const char* function) {
    std::cout << "[EXIT] " << function << std::endl;
}

void log_error(int code, const char* message) {
    std::cout << "[ERROR] Code: " << code << " - " << message << std::endl;
}

// Severity levels: 0=INFO, 1=WARNING, 2=ERROR
void log_severity(int level, const char* message) {
     const char* levels[] = {"INFO", "WARNING", "ERROR"};
     std::cout << "[" << levels[level] << "] " << message << std::endl;
}

// Performance metrics logging
void log_performance(const char* metric, float value) {
     std::cout << "[PERF] " << metric << ": " << value << std::endl;
}


// Network retry logic
bool retry_network_operation(std::function<bool()> operation, int max_retries = 3) {
    log_entry("retry_network_operation");
    
    for (int attempt = 1; attempt <= max_retries; attempt++) {
        if (operation()) {
            log_exit("retry_network_operation");
            return true;
        }
        
        if (attempt < max_retries) {
#ifdef ESP32
            delay(1000 * attempt); // Exponential backoff
#endif
        }
    }
    
    log_error(0x04, "All retry attempts failed");
    log_exit("retry_network_operation");
    return false;
}