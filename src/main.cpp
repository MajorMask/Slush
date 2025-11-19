// Main application entry point
#include "voice_processor.h"
#include "display_manager.h"
#include "input_handler.h"
#include "storage_manager.h"
#include "i2c_communication.h"
#include "error_handler.h"

int main() {
    log_entry("main");
    // System initialization
    // ...existing code...
    // Demo: Register with keyword "Helsinki winter"
    const char* demo_keyword = "Helsinki winter";
    uint32_t demo_voice_hash = 0xABCDEF01; // Simulated hash for original user
    bool registered = register_user(demo_keyword, demo_voice_hash);
    if (registered) {
        show_assignment_number(42);
    } else {
        // Already registered
    }

    // Demo: Different person tries same keyword
    uint32_t other_voice_hash = 0x12345678; // Simulated hash for different person
    int match_result = match_user(demo_keyword, other_voice_hash);
    if (match_result > 0) {
        show_recognition_success(match_result);
    } else {
        show_rejection_feedback();
    }

    // Demo: Original person returns
    match_result = match_user(demo_keyword, demo_voice_hash);
    if (match_result > 0) {
        show_recognition_success(match_result);
    } else {
        show_rejection_feedback();
    }
    log_exit("main");
    return 0;
}
