#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <cstdint>

struct VoiceProfile {
    char keyword[32];
    uint32_t voice_hash;
    float pitch_average;
    float tone_signature[8];
    uint16_t assignment_number;
    uint32_t timestamp;
    bool active;
};

// Storage functions
void store_voice_profile();
bool add_voice_profile(const VoiceProfile& profile);
VoiceProfile* find_voice_profile(const char* keyword, uint32_t voice_hash);
bool deactivate_profile(const char* keyword, uint32_t voice_hash);

#endif // STORAGE_MANAGER_H