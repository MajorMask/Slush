/*
 * COMPLETE Voice-Activated Cloakroom System
 * Integrated with Real Audio Processing
 * Hardware: M5Stack AtomS3R with I2S Microphone
 */

#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Include your existing modules
#include "voice_processor.h"
#include "error_handler.h"

// Include audio manager for real voice processing
#include "audio_manager.h"

// Configuration - UPDATE THESE FOR HACKATHON!
#include "secrets.h"

// Enable real audio processing (set to false for simulation)
#define USE_REAL_AUDIO true

// Global objects
AudioManager audio_manager;

// Demo data
uint16_t demo_numbers[] = {42, 123, 456, 789, 101, 234, 567, 890};
int current_demo_index = 0;
bool wifi_connected = false;
bool api_enabled = false;
bool audio_ready = false;

// Voice profiles for demo
struct VoiceProfileDemo {
    String keyword;
    uint32_t voice_hash;
    uint16_t number;
    bool active;
};

VoiceProfileDemo registered_users[10];
int registered_count = 0;

// Function prototypes
void initializeSystem();
void handleRegistration();
void handleRetrieval();
void updateDisplay(const char* status, int color = WHITE, const char* extra = "");
String processVoiceInput(bool use_real_audio = USE_REAL_AUDIO);
uint32_t calculateVoiceHash(const String& keyword, const std::vector<uint8_t>& audio_data = {});
bool findMatchingUser(const String& keyword, uint32_t voice_hash, uint16_t& found_number);
void provideAudioFeedback(const String& message);

void setup() {
    Serial.begin(115200);
    Serial.println("\n🎤 === VOICE-ACTIVATED CLOAKROOM SYSTEM ===");
    Serial.println("Complete Audio Integration Edition");
    
    initializeSystem();
    
    Serial.println("\n📊 System Status:");
    Serial.printf("- WiFi: %s\n", wifi_connected ? "✅ Connected" : "⚠️  Offline Mode");
    Serial.printf("- ElevenLabs API: %s\n", api_enabled ? "✅ Ready" : "⚠️  Simulation Mode");
    Serial.printf("- Audio Hardware: %s\n", audio_ready ? "✅ I2S Ready" : "⚠️  Simulation Mode");
    Serial.printf("- Real Audio Processing: %s\n", USE_REAL_AUDIO ? "✅ Enabled" : "🔄 Simulation");
    
    Serial.println("\n🎮 Controls:");
    Serial.println("  Button A = REGISTER (record voice + keyword)");
    Serial.println("  Button B = RETRIEVE (voice authentication)");
    Serial.println("  Button C = System Info");
    
    updateDisplay("READY", GREEN);
}

void loop() {
    M5.update();
    
    if (M5.BtnA.wasPressed()) {
        Serial.println("\n🆕 === REGISTRATION MODE ===");
        handleRegistration();
    }
    
    if (M5.BtnB.wasPressed()) {
        Serial.println("\n🔍 === RETRIEVAL MODE ===");
        handleRetrieval();
    }
    
    if (M5.BtnC.wasPressed()) {
        Serial.println("\n📊 === SYSTEM INFO ===");
        Serial.printf("Registered Users: %d\n", registered_count);
        Serial.printf("Next Number: %d\n", demo_numbers[current_demo_index]);
        Serial.printf("Free Memory: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
        Serial.printf("Audio Buffer Size: %d bytes\n", AUDIO_BUFFER_SIZE * 2);
        
        updateDisplay("INFO", CYAN, String(registered_count).c_str());
        delay(2000);
        updateDisplay("READY", GREEN);
    }
    
    delay(50);
}

void initializeSystem() {
    log_entry("initializeSystem");
    
    // Initialize M5 system
    auto cfg = M5.config();
    cfg.clear_display = true;
    M5.begin(cfg);
    
    updateDisplay("STARTING...", YELLOW);
    
    // Initialize audio system
    if (USE_REAL_AUDIO) {
        Serial.println("🎙️  Initializing audio hardware...");
        
        if (audio_manager.initializeMicrophone()) {
            Serial.println("✅ Microphone initialized");
            audio_ready = true;
            
            if (audio_manager.initializeSpeaker()) {
                Serial.println("✅ Speaker initialized");
            } else {
                Serial.println("⚠️  Speaker init failed - TTS will be limited");
            }
        } else {
            Serial.println("⚠️  Microphone init failed - using simulation");
            audio_ready = false;
        }
    } else {
        Serial.println("🔄 Audio simulation mode enabled");
    }
    
    // Connect WiFi (non-blocking)
    if (strlen(WIFI_SSID) > 3 && strcmp(WIFI_SSID, "YOUR_HACKATHON_WIFI") != 0) {
        Serial.printf("📶 Connecting to WiFi: %s\n", WIFI_SSID);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 15) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            wifi_connected = true;
            Serial.printf("\n✅ WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
            
            // Test ElevenLabs API
            if (strlen(ELEVENLABS_API_KEY) > 10 && strcmp(ELEVENLABS_API_KEY, "YOUR_API_KEY_HERE") != 0) {
                api_enabled = true;
                Serial.println("✅ ElevenLabs API ready");
            } else {
                Serial.println("⚠️  ElevenLabs API key not configured");
            }
        } else {
            Serial.println("\n⚠️  WiFi connection failed");
        }
    } else {
        Serial.println("⚠️  WiFi credentials not configured");
    }
    
    Serial.println("✅ System initialization complete");
    log_exit("initializeSystem");
}

void handleRegistration() {
    log_entry("handleRegistration");
    
    updateDisplay("RECORDING", BLUE, "Speak now");
    
    // Capture voice input
    String recognized_keyword = processVoiceInput(USE_REAL_AUDIO && audio_ready);
    
    if (recognized_keyword.length() == 0) {
        Serial.println("❌ Voice capture failed");
        updateDisplay("ERROR", RED, "Try again");
        delay(2000);
        updateDisplay("READY", GREEN);
        log_exit("handleRegistration");
        return;
    }
    
    Serial.printf("🎯 Recognized: '%s'\n", recognized_keyword.c_str());
    
    updateDisplay("PROCESSING", YELLOW, "AI working");
    
    // Calculate voice hash for biometric
    uint32_t voice_hash = calculateVoiceHash(recognized_keyword);
    
    // Check if already registered
    uint16_t existing_number;
    if (findMatchingUser(recognized_keyword, voice_hash, existing_number)) {
        Serial.printf("👤 User already registered with number: %d\n", existing_number);
        updateDisplay("EXISTING", ORANGE, String(existing_number).c_str());
        
        provideAudioFeedback("You are already registered as number " + String(existing_number));
        delay(3000);
        updateDisplay("READY", GREEN);
        log_exit("handleRegistration");
        return;
    }
    
    // Register new user
    if (registered_count < 10) {
        uint16_t assigned_number = demo_numbers[current_demo_index];
        current_demo_index = (current_demo_index + 1) % 8;
        
        registered_users[registered_count] = {
            recognized_keyword,
            voice_hash, 
            assigned_number,
            true
        };
        registered_count++;
        
        Serial.printf("✅ NEW USER REGISTERED:\n");
        Serial.printf("   Keyword: %s\n", recognized_keyword.c_str());
        Serial.printf("   Voice Hash: 0x%08X\n", voice_hash);
        Serial.printf("   Number: %d\n", assigned_number);
        
        updateDisplay("ASSIGNED", GREEN, String(assigned_number).c_str());
        
        provideAudioFeedback("Your items are stored as number " + String(assigned_number));
        
        // Log performance metrics
        log_performance("registration_success", 1.0f);
        log_performance("total_users", (float)registered_count);
        
        delay(4000);
    } else {
        Serial.println("❌ Registration full");
        updateDisplay("FULL", RED, "Storage");
        delay(2000);
    }
    
    updateDisplay("READY", GREEN);
    log_exit("handleRegistration");
}

void handleRetrieval() {
    log_entry("handleRetrieval");
    
    updateDisplay("LISTENING", CYAN, "Speak now");
    
    // Capture voice for authentication
    String spoken_keyword = processVoiceInput(USE_REAL_AUDIO && audio_ready);
    
    if (spoken_keyword.length() == 0) {
        Serial.println("❌ Voice capture failed");
        updateDisplay("ERROR", RED, "Try again");
        delay(2000);
        updateDisplay("READY", GREEN);
        log_exit("handleRetrieval");
        return;
    }
    
    Serial.printf("🎯 Heard: '%s'\n", spoken_keyword.c_str());
    
    updateDisplay("MATCHING", PURPLE, "Verifying");
    
    // Calculate voice hash for matching
    uint32_t spoken_hash = calculateVoiceHash(spoken_keyword);
    
    // Find matching profile
    uint16_t found_number;
    if (findMatchingUser(spoken_keyword, spoken_hash, found_number)) {
        Serial.printf("✅ AUTHENTICATION SUCCESS:\n");
        Serial.printf("   Voice verified for number: %d\n", found_number);
        
        updateDisplay("FOUND", GREEN, String(found_number).c_str());
        
        provideAudioFeedback("Your items are number " + String(found_number));
        
        log_performance("retrieval_success", 1.0f);
        delay(4000);
    } else {
        Serial.println("❌ AUTHENTICATION FAILED:");
        Serial.println("   Voice not recognized or user not registered");
        
        updateDisplay("NOT FOUND", RED, "Register?");
        
        log_performance("retrieval_failure", 1.0f);
        delay(3000);
    }
    
    updateDisplay("READY", GREEN);
    log_exit("handleRetrieval");
}

String processVoiceInput(bool use_real_audio) {
    log_entry("processVoiceInput");
    
    String result = "";
    std::vector<uint8_t> audio_data;
    
    if (use_real_audio && audio_ready) {
        Serial.println("🎙️  Recording real audio...");
        
        // Start recording
        if (audio_manager.startRecording()) {
            delay(3000); // Record for 3 seconds
            audio_manager.stopRecording();
            
            // Get recorded audio
            audio_data = audio_manager.getRecordedAudio();
            
            if (audio_data.size() > 0) {
                Serial.printf("📊 Captured %d bytes of audio\n", audio_data.size());
                
                if (api_enabled && wifi_connected) {
                    // Process with ElevenLabs
                    Serial.println("🤖 Processing with ElevenLabs STT...");
                    result = elevenlabs_speech_to_text(audio_data);
                } else {
                    // Simulate recognition for demo
                    result = "Helsinki winter";
                    Serial.println("🔄 Simulated recognition (no API)");
                }
            } else {
                Serial.println("❌ No audio data captured");
            }
        } else {
            Serial.println("❌ Failed to start recording");
        }
    } else {
        // Simulation mode
        Serial.println("🔄 Voice simulation mode");
        delay(2000); // Simulate recording time
        result = "Helsinki winter"; // Demo keyword
    }
    
    log_exit("processVoiceInput");
    return result;
}

uint32_t calculateVoiceHash(const String& keyword, const std::vector<uint8_t>& audio_data) {
    log_entry("calculateVoiceHash");
    
    uint32_t hash = 0;
    
    // Hash the keyword
    for (int i = 0; i < keyword.length(); i++) {
        hash = hash * 31 + keyword[i];
    }
    
    // If we have audio data, incorporate voice characteristics
    if (audio_data.size() > 0) {
        // Simple voice biometric simulation - in production this would be much more sophisticated
        size_t step = audio_data.size() / 16; // Sample 16 points
        for (size_t i = 0; i < audio_data.size(); i += step) {
            hash ^= (audio_data[i] << (i % 24));
        }
    } else {
        // Simulation mode - use timestamp for uniqueness
        hash ^= (millis() & 0xFFFF0000);
    }
    
    log_exit("calculateVoiceHash");
    return hash;
}

bool findMatchingUser(const String& keyword, uint32_t voice_hash, uint16_t& found_number) {
    log_entry("findMatchingUser");
    
    for (int i = 0; i < registered_count; i++) {
        if (registered_users[i].active && 
            registered_users[i].keyword.equals(keyword) && 
            registered_users[i].voice_hash == voice_hash) {
            
            found_number = registered_users[i].number;
            log_exit("findMatchingUser");
            return true;
        }
    }
    
    log_exit("findMatchingUser");
    return false;
}

void provideAudioFeedback(const String& message) {
    log_entry("provideAudioFeedback");
    
    Serial.printf("🔊 Audio Feedback: '%s'\n", message.c_str());
    
    if (api_enabled && wifi_connected) {
        // Generate TTS with ElevenLabs
        std::vector<uint8_t> tts_audio = elevenlabs_text_to_speech(message.c_str());
        
        if (tts_audio.size() > 0 && audio_ready) {
            Serial.println("🎵 Playing TTS audio...");
            audio_manager.playAudio(tts_audio);
        } else {
            Serial.println("🔄 TTS generated but no speaker available");
        }
    } else {
        Serial.println("🔄 TTS simulation mode (no API)");
    }
    
    log_exit("provideAudioFeedback");
}

void updateDisplay(const char* status, int color, const char* extra) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(color);
    M5.Display.setTextSize(2);
    
    M5.Display.setCursor(5, 8);
    M5.Display.print("CLOAKROOM");
    
    M5.Display.setCursor(5, 32);
    M5.Display.print("VOICE ID");
    
    M5.Display.setCursor(5, 60);
    M5.Display.print(status);
    
    if (extra && strlen(extra) > 0) {
        M5.Display.setCursor(5, 88);
        M5.Display.setTextSize(3);
        M5.Display.print(extra);
    }
}