#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

// Input handling functions
void handle_gamepad_input();
void navigate_menu(int direction);
void handle_button_action(int button_id);
void adjust_volume(int value);
void adjust_sensitivity(int value);
bool detect_hand_wave();

// Analytics functions
void log_peak_usage_time();
void log_average_retrieval_time();
void log_voice_recognition_accuracy();
void log_hardware_health();

#endif // INPUT_HANDLER_H