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
