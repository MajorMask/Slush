#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#endif
#include <functional>

// Voice processing and ElevenLabs integration
#include "voice_processor.h"
#include "error_handler.h"
#include <cstring>

struct VoiceProfile {
    char keyword[32];
    uint32_t voice_hash;
    float pitch_average;
    float tone_signature[8];
    uint16_t assignment_number;
    uint32_t timestamp;
    bool active;
};

std::string get_elevenlabs_api_key() {
    const char* key = std::getenv("ELEVENLABS_API_KEY");
    if (key) return std::string(key);
    log_error(0x05, "ElevenLabs API key not found in environment");
    return "";
}

// Demo profile for "Helsinki winter"
VoiceProfile demo_profile = {"Helsinki winter", 0xABCDEF01, 220.0f, {0.1f,0.2f,0.3f,0.4f,0.5f,0.6f,0.7f,0.8f}, 42, 1700000000, true};

void capture_voice() {
    log_entry("capture_voice");
    // ...voice capture logic...
    log_exit("capture_voice");
}

void process_keyword(const char* keyword) {
    log_entry("process_keyword");
    // ...keyword processing logic...
    log_exit("process_keyword");
}

// Register a new user (demo: only one profile)
bool register_user(const char* keyword, uint32_t voice_hash) {
    log_entry("register_user");
    if (strcmp(keyword, "Helsinki winter") == 0 && voice_hash == demo_profile.voice_hash) {
        // Already registered
        log_exit("register_user");
        return false;
    }
    // In real system: store new profile
    log_exit("register_user");
    return true;
}

// Match user for retrieval
int match_user(const char* keyword, uint32_t voice_hash) {
    log_entry("match_user");
    if (strcmp(keyword, demo_profile.keyword) == 0 && voice_hash == demo_profile.voice_hash) {
        // Match found
        log_exit("match_user");
        return demo_profile.assignment_number;
    }
    // No match
    log_exit("match_user");
    return -1;
}


// ElevenLabs API implementation for speech-to-text
std::string elevenlabs_speech_to_text(const std::vector<uint8_t>& audio_data) {
    log_entry("elevenlabs_speech_to_text");
    
    std::string api_key = get_elevenlabs_api_key();
    if (api_key.empty()) {
        // For hackathon: hardcode your key here
        api_key = "YOUR_ELEVENLABS_API_KEY_HERE";
    }

#ifdef ESP32
    HTTPClient http;
    http.begin("https://api.elevenlabs.io/v1/speech-to-text");
    http.addHeader("Accept", "application/json");
    http.addHeader("xi-api-key", api_key.c_str());
    http.addHeader("Content-Type", "audio/wav");
    
    int httpResponseCode = http.POST((uint8_t*)audio_data.data(), audio_data.size());
    
    std::string result = "";
    if (httpResponseCode == 200) {
        String response = http.getString();
        
        // Simple JSON parsing
        int text_start = response.indexOf("\"text\":\"") + 8;
        int text_end = response.indexOf("\"", text_start);
        if (text_start > 7 && text_end > text_start) {
            result = response.substring(text_start, text_end).c_str();
            log_performance("STT_success", 1.0f);
        }
    } else {
        log_error(0x01, ("STT API failed: " + std::to_string(httpResponseCode)).c_str());
    }
    http.end();
#else
    // Desktop simulation for testing
    result = "Helsinki winter"; // Your demo keyword
    log_performance("STT_simulation", 1.0f);
#endif

    log_exit("elevenlabs_speech_to_text");
    return result;
}

std::vector<uint8_t> elevenlabs_text_to_speech(const std::string& text) {
    log_entry("elevenlabs_text_to_speech");
    
    std::string api_key = get_elevenlabs_api_key();
    if (api_key.empty()) {
        api_key = "YOUR_ELEVENLABS_API_KEY_HERE"; // Hardcode for hackathon
    }
    
    std::vector<uint8_t> audio_data;

#ifdef ESP32
    std::string voice_id = "21m00Tcm4TlvDq8ikWAM"; // Rachel voice
    std::string url = "https://api.elevenlabs.io/v1/text-to-speech/" + voice_id;
    
    HTTPClient http;
    http.begin(url.c_str());
    http.addHeader("Accept", "audio/mpeg");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("xi-api-key", api_key.c_str());
    
    // Create JSON payload
    String json_payload = "{\"text\":\"" + String(text.c_str()) + 
                         "\",\"model_id\":\"eleven_monolingual_v1\"}";
    
    int httpResponseCode = http.POST(json_payload);
    
    if (httpResponseCode == 200) {
        WiFiClient* stream = http.getStreamPtr();
        
        // Read audio data
        uint8_t buffer[512];
        while (http.connected() && stream->available()) {
            size_t bytes_read = stream->readBytes(buffer, sizeof(buffer));
            audio_data.insert(audio_data.end(), buffer, buffer + bytes_read);
        }
        
        log_performance("TTS_success", 1.0f);
    } else {
        log_error(0x01, ("TTS API failed: " + std::to_string(httpResponseCode)).c_str());
    }
    http.end();
#else
    // Desktop simulation
    audio_data.resize(1000, 0x80); // Dummy audio
    log_performance("TTS_simulation", 1.0f);
#endif

    log_exit("elevenlabs_text_to_speech");
    return audio_data;
}