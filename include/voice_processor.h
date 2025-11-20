#ifndef VOICE_PROCESSOR_H
#define VOICE_PROCESSOR_H

#include <vector>
#include <string>
#include <cstdint>

// Function declarations
void capture_voice();
void process_keyword(const char* keyword);
bool register_user(const char* keyword, uint32_t voice_hash);
int match_user(const char* keyword, uint32_t voice_hash);

// ElevenLabs API functions
std::string elevenlabs_speech_to_text(const std::vector<uint8_t>& audio_data);
std::vector<uint8_t> elevenlabs_text_to_speech(const std::string& text);
std::string get_elevenlabs_api_key();

#endif // VOICE_PROCESSOR_H