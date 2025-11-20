/*
 * Audio Capture and Playback Module
 * ESP32 I2S Implementation for Voice Processing
 */

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <driver/i2s.h>
#include <vector>
#include <cstdint>

// Audio Configuration
#define SAMPLE_RATE 16000
#define BITS_PER_SAMPLE 16
#define CHANNELS 1
#define AUDIO_BUFFER_SIZE 2048
#define MAX_RECORDING_DURATION 5 // seconds

// I2S Pin Configuration (AtomS3R)
#define I2S_MIC_WS_PIN 5
#define I2S_MIC_SD_PIN 6
#define I2S_MIC_SCK_PIN 4

// I2S Configuration for Speaker (if available)
#define I2S_SPK_BCLK_PIN 12
#define I2S_SPK_LRCK_PIN 0
#define I2S_SPK_DATA_PIN 2

class AudioManager {
private:
    bool mic_initialized;
    bool speaker_initialized;
    int16_t* audio_buffer;
    size_t buffer_size;
    bool recording;
    
public:
    AudioManager();
    ~AudioManager();
    
    // Initialization
    bool initializeMicrophone();
    bool initializeSpeaker();
    void deinitialize();
    
    // Recording functions
    bool startRecording();
    void stopRecording();
    std::vector<uint8_t> getRecordedAudio();
    bool isRecording() { return recording; }
    
    // Playback functions
    bool playAudio(const std::vector<uint8_t>& audio_data);
    void stopPlayback();
    
    // WAV file creation
    std::vector<uint8_t> createWAVFile(const std::vector<int16_t>& pcm_data);
    
    // Utility functions
    void amplifyAudio(std::vector<int16_t>& audio_data, float gain = 2.0f);
    float calculateRMS(const std::vector<int16_t>& audio_data);
};

// Implementation
AudioManager::AudioManager() : mic_initialized(false), speaker_initialized(false), recording(false) {
    audio_buffer = (int16_t*)malloc(AUDIO_BUFFER_SIZE * sizeof(int16_t));
    buffer_size = 0;
}

AudioManager::~AudioManager() {
    deinitialize();
    if (audio_buffer) {
        free(audio_buffer);
    }
}

bool AudioManager::initializeMicrophone() {
    ESP_LOGI("AUDIO", "Initializing I2S microphone...");
    
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_MIC_SCK_PIN,
        .ws_io_num = I2S_MIC_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_MIC_SD_PIN
    };
    
    esp_err_t result = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (result != ESP_OK) {
        ESP_LOGE("AUDIO", "Failed to install I2S driver: %s", esp_err_to_name(result));
        return false;
    }
    
    result = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (result != ESP_OK) {
        ESP_LOGE("AUDIO", "Failed to set I2S pins: %s", esp_err_to_name(result));
        return false;
    }
    
    // Clear DMA buffers
    i2s_zero_dma_buffer(I2S_NUM_0);
    
    mic_initialized = true;
    ESP_LOGI("AUDIO", "I2S microphone initialized successfully");
    return true;
}

bool AudioManager::initializeSpeaker() {
    ESP_LOGI("AUDIO", "Initializing I2S speaker...");
    
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SPK_BCLK_PIN,
        .ws_io_num = I2S_SPK_LRCK_PIN,
        .data_out_num = I2S_SPK_DATA_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    esp_err_t result = i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
    if (result != ESP_OK) {
        ESP_LOGE("AUDIO", "Failed to install I2S speaker driver: %s", esp_err_to_name(result));
        return false;
    }
    
    result = i2s_set_pin(I2S_NUM_1, &pin_config);
    if (result != ESP_OK) {
        ESP_LOGE("AUDIO", "Failed to set I2S speaker pins: %s", esp_err_to_name(result));
        return false;
    }
    
    speaker_initialized = true;
    ESP_LOGI("AUDIO", "I2S speaker initialized successfully");
    return true;
}

void AudioManager::deinitialize() {
    if (mic_initialized) {
        i2s_driver_uninstall(I2S_NUM_0);
        mic_initialized = false;
    }
    
    if (speaker_initialized) {
        i2s_driver_uninstall(I2S_NUM_1);
        speaker_initialized = false;
    }
}

bool AudioManager::startRecording() {
    if (!mic_initialized) {
        ESP_LOGE("AUDIO", "Microphone not initialized");
        return false;
    }
    
    ESP_LOGI("AUDIO", "Starting audio recording...");
    recording = true;
    buffer_size = 0;
    
    return true;
}

void AudioManager::stopRecording() {
    recording = false;
    ESP_LOGI("AUDIO", "Recording stopped. Captured %d samples", buffer_size);
}

std::vector<uint8_t> AudioManager::getRecordedAudio() {
    if (!mic_initialized || !recording) {
        return std::vector<uint8_t>();
    }
    
    std::vector<int16_t> pcm_data;
    size_t bytes_read = 0;
    uint32_t start_time = millis();
    
    // Record for specified duration
    while (recording && (millis() - start_time) < (MAX_RECORDING_DURATION * 1000)) {
        int16_t samples[AUDIO_BUFFER_SIZE];
        
        esp_err_t result = i2s_read(I2S_NUM_0, samples, sizeof(samples), &bytes_read, portMAX_DELAY);
        if (result == ESP_OK && bytes_read > 0) {
            size_t samples_read = bytes_read / sizeof(int16_t);
            
            // Apply gain and filtering
            for (size_t i = 0; i < samples_read; i++) {
                int32_t amplified = samples[i] * 4; // 4x amplification
                samples[i] = constrain(amplified, -32768, 32767);
            }
            
            pcm_data.insert(pcm_data.end(), samples, samples + samples_read);
        }
        
        delay(10); // Small delay to prevent watchdog
    }
    
    recording = false;
    
    // Convert to WAV format
    return createWAVFile(pcm_data);
}

bool AudioManager::playAudio(const std::vector<uint8_t>& audio_data) {
    if (!speaker_initialized) {
        ESP_LOGE("AUDIO", "Speaker not initialized");
        return false;
    }
    
    ESP_LOGI("AUDIO", "Playing audio data (%d bytes)", audio_data.size());
    
    // Skip WAV header if present (44 bytes)
    size_t data_offset = 0;
    if (audio_data.size() > 44 && 
        audio_data[0] == 'R' && audio_data[1] == 'I' && 
        audio_data[2] == 'F' && audio_data[3] == 'F') {
        data_offset = 44;
    }
    
    size_t bytes_written = 0;
    size_t total_bytes = audio_data.size() - data_offset;
    const uint8_t* data_ptr = audio_data.data() + data_offset;
    
    while (bytes_written < total_bytes) {
        size_t chunk_size = min(1024UL, total_bytes - bytes_written);
        size_t written = 0;
        
        esp_err_t result = i2s_write(I2S_NUM_1, data_ptr + bytes_written, chunk_size, &written, portMAX_DELAY);
        if (result != ESP_OK) {
            ESP_LOGE("AUDIO", "I2S write failed: %s", esp_err_to_name(result));
            return false;
        }
        
        bytes_written += written;
        delay(1); // Prevent watchdog
    }
    
    ESP_LOGI("AUDIO", "Audio playback completed");
    return true;
}

void AudioManager::stopPlayback() {
    if (speaker_initialized) {
        i2s_zero_dma_buffer(I2S_NUM_1);
    }
}

std::vector<uint8_t> AudioManager::createWAVFile(const std::vector<int16_t>& pcm_data) {
    std::vector<uint8_t> wav_file;
    
    // WAV header
    uint32_t file_size = pcm_data.size() * 2 + 36;
    uint32_t data_size = pcm_data.size() * 2;
    
    // RIFF header
    wav_file.insert(wav_file.end(), {'R', 'I', 'F', 'F'});
    wav_file.push_back(file_size & 0xFF);
    wav_file.push_back((file_size >> 8) & 0xFF);
    wav_file.push_back((file_size >> 16) & 0xFF);
    wav_file.push_back((file_size >> 24) & 0xFF);
    wav_file.insert(wav_file.end(), {'W', 'A', 'V', 'E'});
    
    // Format chunk
    wav_file.insert(wav_file.end(), {'f', 'm', 't', ' '});
    wav_file.insert(wav_file.end(), {16, 0, 0, 0}); // Chunk size
    wav_file.insert(wav_file.end(), {1, 0}); // PCM format
    wav_file.insert(wav_file.end(), {1, 0}); // Mono
    
    // Sample rate
    wav_file.push_back(SAMPLE_RATE & 0xFF);
    wav_file.push_back((SAMPLE_RATE >> 8) & 0xFF);
    wav_file.push_back((SAMPLE_RATE >> 16) & 0xFF);
    wav_file.push_back((SAMPLE_RATE >> 24) & 0xFF);
    
    // Byte rate
    uint32_t byte_rate = SAMPLE_RATE * 2;
    wav_file.push_back(byte_rate & 0xFF);
    wav_file.push_back((byte_rate >> 8) & 0xFF);
    wav_file.push_back((byte_rate >> 16) & 0xFF);
    wav_file.push_back((byte_rate >> 24) & 0xFF);
    
    wav_file.insert(wav_file.end(), {2, 0}); // Block align
    wav_file.insert(wav_file.end(), {16, 0}); // Bits per sample
    
    // Data chunk
    wav_file.insert(wav_file.end(), {'d', 'a', 't', 'a'});
    wav_file.push_back(data_size & 0xFF);
    wav_file.push_back((data_size >> 8) & 0xFF);
    wav_file.push_back((data_size >> 16) & 0xFF);
    wav_file.push_back((data_size >> 24) & 0xFF);
    
    // PCM data
    for (int16_t sample : pcm_data) {
        wav_file.push_back(sample & 0xFF);
        wav_file.push_back((sample >> 8) & 0xFF);
    }
    
    return wav_file;
}

void AudioManager::amplifyAudio(std::vector<int16_t>& audio_data, float gain) {
    for (int16_t& sample : audio_data) {
        int32_t amplified = (int32_t)(sample * gain);
        sample = constrain(amplified, -32768, 32767);
    }
}

float AudioManager::calculateRMS(const std::vector<int16_t>& audio_data) {
    if (audio_data.empty()) return 0.0f;
    
    float sum_squares = 0.0f;
    for (int16_t sample : audio_data) {
        sum_squares += (float)(sample * sample);
    }
    
    return sqrt(sum_squares / audio_data.size());
}

#endif // AUDIO_MANAGER_H