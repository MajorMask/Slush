// TFT and LED control
#include "display_manager.h"
#include "error_handler.h"

void show_assignment_number(uint16_t number) {
    log_entry("show_assignment_number");
    // ...display logic...
    log_exit("show_assignment_number");
}
void show_recognition_success(uint16_t number) {
    log_entry("show_recognition_success");
    // Display success feedback (e.g., green LED, confirmation message)
    // tft.print("Recognition successful! Number: ");
    // tft.print(number);
    // Set LED matrix to green
    // ...existing code...
    log_exit("show_recognition_success");
}

void show_rejection_feedback() {
    log_entry("show_rejection_feedback");
    // Display rejection feedback (e.g., red LED, error message)
    // tft.print("Recognition failed. Please try again.");
    // Set LED matrix to red
    // ...existing code...
    log_exit("show_rejection_feedback");
}
