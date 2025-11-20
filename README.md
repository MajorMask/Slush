# Slush - Voice-Activated Cloakroom System 🎤🔢

A sophisticated voice biometric-based item check-in/check-out system designed for cloakrooms, coat checks, and secure storage facilities. The system uses unique voice signatures combined with keywords to provide a touchless, secure, and user-friendly experience.

## 🎯 Overview

Slush eliminates traditional ticket systems by using voice biometrics as the authentication method. Users register with a unique keyword, and the system captures their voice characteristics to create a secure profile. Upon return, users simply speak their keyword, and the system instantly identifies them and retrieves their assigned number.

### Key Features

- **Voice Biometric Authentication**: Combines voice characteristics with keyword matching
- **Touchless Operation**: No physical tickets or cards needed
- **Instant Recognition**: Fast retrieval with >85% confidence matching
- **Dual Hardware Architecture**: AtomS3R for voice processing + Feather ESP32-S3 for display
- **Staff-Friendly Interface**: Gamepad controls and visual status indicators
- **Graceful Degradation**: Works with or without internet connectivity

## 🎬 Demo Flow

### Registration Example
1. User approaches cloakroom station
2. Staff presses **Button A** on gamepad to enter registration mode
3. User speaks: *"Helsinki winter"*
4. System processes voice + keyword
5. **LED Matrix displays: #042**
6. TFT screen confirms: *"Your code is 042"*
7. Audio feedback: *"Your items are stored as number 42"*

### Retrieval Example
1. User returns and speaks: *"Helsinki winter"*
2. System analyzes voice biometrics
3. **Instant match found**
4. LED displays: **#042**
5. Audio confirmation: *"Your items are number 42"*

### Security Test
- **Scenario**: Different person tries same keyword *"Helsinki winter"*
- **Result**: System rejects (voice biometric mismatch)
- **Outcome**: Original person's items remain secure

## 🔧 Hardware Requirements

### Primary Device: M5Stack AtomS3R
- **Purpose**: Voice capture and processing
- **Features**:
  - Built-in microphone for voice input
  - LED matrix for number display (001-999)
  - 3 programmable buttons (A, B, C)
  - Built-in accelerometer (item placement detection)
  - I2C communication to Feather

### Secondary Device: Adafruit Feather ESP32-S3 Reverse TFT
- **Purpose**: Display and gamepad interface
- **Features**:
  - Built-in TFT screen for detailed information
  - I2C receiver for data from AtomS3R
  - Gamepad input support
  - Potentiometer for settings adjustment

### Additional Components
- **Gamepad Controller**: Navigation and control interface
- **Potentiometer**: Volume and sensitivity adjustment
- **Status LEDs**:
  - 🟢 Green: System ready
  - 🔵 Blue: Recording voice
  - 🟡 Yellow: Processing
  - 🔴 Red: Error or failed match

## 🏗️ System Architecture

```
┌─────────────────────┐         I2C          ┌──────────────────────┐
│   AtomS3R Device    │◄─────────────────────►│  Feather ESP32-S3    │
│                     │                        │                      │
│ • Voice Capture     │                        │ • TFT Display        │
│ • Audio Processing  │                        │ • Gamepad Input      │
│ • LED Matrix        │                        │ • Staff Interface    │
│ • Accelerometer     │                        │ • Settings Control   │
│ • Voice Biometrics  │                        │ • Visual Feedback    │
└─────────────────────┘                        └──────────────────────┘
         ▲                                              ▲
         │                                              │
         │                                              │
    User Voice                                    Staff Controls
    Input/Output                                  (Gamepad/Pot)
```

### Communication Protocol

The two devices communicate via I2C:

```c
struct I2CMessage {
    uint8_t command;           // Operation type
    uint8_t data[64];          // Payload (number, status, etc.)
    uint8_t checksum;          // Data integrity check
};
```

**Message Flow**:
1. AtomS3R captures voice → Processes biometrics
2. Generates/retrieves assignment number
3. Sends number via I2C to Feather
4. Feather displays on TFT + handles gamepad input
5. Feather can send control commands back to AtomS3R

## 📝 Registration Process (Check-in)

### Step-by-Step Workflow

1. **Initiation**
   - User approaches cloakroom station
   - Staff activates registration mode via **Gamepad Button A**
   - LED turns **BLUE** (recording mode)

2. **Voice Capture**
   - System prompts: *"Please state your unique keyword and hold for 3 seconds"*
   - User speaks their chosen keyword (e.g., *"blue mountain sunrise"*)
   - AtomS3R microphone captures 3 seconds of audio

3. **Biometric Processing**
   - LED turns **YELLOW** (processing)
   - System analyzes:
     - Voice pitch and tone
     - Speaking rhythm and cadence
     - Phonetic signature of keyword
     - Unique voice characteristics
   - Generates composite hash:
     ```
     hash = voice_biometric_signature XOR keyword_phonetic_hash XOR timestamp
     ```

4. **Profile Creation**
   ```c
   VoiceProfile {
       keyword: "blue mountain sunrise"
       voice_hash: 0xABCDEF01
       pitch_average: 185.5 Hz
       tone_signature: [float array]
       assignment_number: 042
       timestamp: 1700000000
       active: true
   }
   ```

5. **Number Assignment**
   - System assigns unique number (001-999)
   - LED Matrix displays: **#042**
   - TFT screen shows: *"Your code is 042. Keyword: [masked]"*
   - Audio feedback: *"Your items are stored as number 42"*

6. **Physical Confirmation**
   - Accelerometer detects when item is placed on counter
   - System confirms storage is complete

### Registration Security Features

- **Duplicate Detection**: Checks if keyword+voice combination already exists
- **Voice Uniqueness**: Different person saying same keyword = different profile
- **Time-Stamped**: Each registration includes timestamp for session management
- **Masked Display**: Keyword is partially masked on TFT for privacy

## 🔍 Retrieval Process (Check-out)

### Step-by-Step Workflow

1. **Voice Input**
   - User approaches station
   - System auto-listens or staff presses **Button B**
   - LED turns **CYAN** (listening mode)
   - User speaks their keyword

2. **Voice Analysis**
   - LED turns **PURPLE** (matching)
   - System captures voice sample
   - Generates temporary voice hash
   - Compares against stored profiles

3. **Matching Algorithm**
   ```
   For each stored profile:
       keyword_match = compare_phonetic(spoken, stored.keyword)
       voice_match = compare_biometric(spoken_hash, stored.voice_hash)
       confidence = (keyword_match * 0.4) + (voice_match * 0.6)
       
       if confidence > 0.85:
           return profile.assignment_number
   ```

4. **Success Path** (Match Found >85% confidence)
   - LED turns **GREEN**
   - LED Matrix displays: **#042**
   - TFT shows: *"Your items are number 42"*
   - Optional: Display location map on TFT
   - Audio confirmation: *"Your items are number 42"*

5. **Failure Path** (No Match or Low Confidence)
   - LED turns **RED**
   - System prompts: *"Not recognized. Please try again."*
   - After 3 failed attempts:
     - Alert staff via TFT notification
     - Offer manual override option
     - Log incident for security review

### Retrieval Features

- **Multi-Attempt Support**: Up to 3 tries before staff alert
- **Confidence Scoring**: Transparent matching threshold
- **Manual Override**: Staff can assist if needed
- **Location Mapping**: Optional TFT map showing item location
- **Session Logging**: All retrieval attempts logged for analytics

## 🎮 Staff Interface Features

### Gamepad Controls

| Button/Direction | Function |
|------------------|----------|
| **Up/Down** | Navigate menu options |
| **Left/Right** | Adjust sensitivity settings |
| **Button A** | Enter registration mode |
| **Button B** | Enter retrieval mode |
| **Button C** | View system status |
| **Start** | Confirm action |
| **Select** | Cancel/Go back |
| **L/R Triggers** | Emergency override |

### Potentiometer Functions

1. **Volume Control**
   - Adjusts audio feedback volume
   - Range: 0-100%
   - Real-time adjustment during operation

2. **Sensitivity Adjustment**
   - Controls voice matching threshold
   - Default: 85% confidence
   - Range: 70-95%
   - Lower = More lenient matching
   - Higher = Stricter security

### Visual Status Indicators

| LED Color | Status | Description |
|-----------|--------|-------------|
| 🟢 **Green** | Ready | System operational, awaiting input |
| 🔵 **Blue** | Recording | Capturing voice input |
| 🟡 **Yellow** | Processing | Analyzing voice biometrics |
| 🟣 **Purple** | Matching | Comparing against database |
| 🔴 **Red** | Error | Failed match or system error |
| 🟠 **Orange** | Warning | Already registered / Low confidence |

### TFT Screen Information

**Normal Mode Display**:
```
╔════════════════════════╗
║   CLOAKROOM SYSTEM    ║
║      VOICE ID         ║
║                       ║
║   Status: READY       ║
║   Registered: 42      ║
║   Available: 958      ║
╚════════════════════════╝
```

**Registration Display**:
```
╔════════════════════════╗
║   REGISTRATION        ║
║                       ║
║   Recording...        ║
║   Speak your keyword  ║
║   and hold 3 seconds  ║
╚════════════════════════╝
```

**Assignment Confirmation**:
```
╔════════════════════════╗
║   ✓ REGISTERED        ║
║                       ║
║   YOUR NUMBER:        ║
║        042            ║
║                       ║
║   Keyword: He***er    ║
╚════════════════════════╝
```

## 💻 Software Architecture

### Core Components

#### 1. Voice Processor (`voice_processor.h`)
```c
- capture_voice()              // Capture audio from microphone
- process_keyword()            // Extract keyword phonetics
- register_user()              // Create new voice profile
- match_user()                 // Find matching profile
- elevenlabs_speech_to_text()  // API integration for STT
- elevenlabs_text_to_speech()  // API integration for TTS
```

#### 2. Storage Manager (`storage_manager.h`)
```c
- add_voice_profile()          // Store new profile
- find_voice_profile()         // Search database
- deactivate_profile()         // Remove/disable profile
```

#### 3. Display Manager (`display_manager.h`)
```c
- show_assignment_number()     // Display assigned number
- show_recognition_success()   // Show match result
- show_rejection_feedback()    // Display error/retry
```

#### 4. I2C Communication (`i2c_communication.h`)
```c
- send_message()               // AtomS3R → Feather
- receive_message()            // Feather ← AtomS3R
- calc_checksum()              // Data integrity
```

#### 5. Input Handler (`input_handler.h`)
```c
- handle_gamepad_input()       // Process gamepad events
- adjust_volume()              // Control audio levels
- adjust_sensitivity()         // Tune matching threshold
```

#### 6. Error Handler (`error_handler.h`)
```c
- log_error()                  // Error logging
- retry_network_operation()    // Network resilience
- log_performance()            // Analytics tracking
```

## 🚀 Installation & Setup

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- M5Stack AtomS3R connected via USB
- Adafruit Feather ESP32-S3 connected via USB
- WiFi credentials (optional, for ElevenLabs API)
- ElevenLabs API key (optional, for production features)

### Step 1: Clone Repository

```bash
git clone https://github.com/MajorMask/Slush.git
cd Slush
```

### Step 2: Configure Secrets

Create `include/secrets.h`:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

// WiFi Configuration (Optional)
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ElevenLabs API (Optional)
#define ELEVENLABS_API_KEY "your_api_key_here"

#endif
```

**Note**: System works in simulation mode without WiFi/API. Perfect for testing!

### Step 3: Flash AtomS3R

```bash
# Option 1: Hackathon Demo Version (Recommended for quick start)
pio run -e atoms3 -t upload

# Option 2: Production Version (Full features)
# Uncomment production_main.cpp in platformio.ini
pio run -e atoms3 -t upload
```

### Step 4: Flash Feather ESP32-S3

```bash
pio run -e feather -t upload
```

### Step 5: Connect I2C

Wire the two devices:
```
AtomS3R       Feather
SDA (GPIO 2) → SDA (GPIO 3)
SCL (GPIO 1) → SCL (GPIO 4)
GND          → GND
```

### Step 6: Test System

1. Open serial monitor for AtomS3R:
   ```bash
   pio device monitor -e atoms3
   ```

2. Press **Button A** on AtomS3R
3. Speak a keyword (simulated in demo mode)
4. Verify number appears on LED matrix

## 📱 Usage Guide

### For End Users

#### Registering (Check-in)
1. Approach the cloakroom station
2. Wait for staff to activate registration mode
3. When prompted, speak your unique keyword clearly
4. Hold position for 3 seconds while recording
5. Remember your assigned number (displayed on screen)
6. Hand your items to staff

#### Retrieving (Check-out)
1. Approach the station
2. Speak your keyword when prompted
3. Wait for voice authentication (1-2 seconds)
4. Note your number on the display
5. Receive your items from staff

### For Staff

#### Daily Setup
1. Power on both devices
2. Wait for **GREEN** LED (system ready)
3. Check TFT screen for system status
4. Verify WiFi connection (if using API features)

#### Handling Check-in
1. Press **Gamepad Button A** (registration mode)
2. Instruct user to speak keyword
3. Wait for **YELLOW** LED (processing)
4. Note assigned number on TFT screen
5. Store items with corresponding number
6. Confirm accelerometer detected item placement

#### Handling Check-out
1. Press **Gamepad Button B** (or auto-trigger)
2. User speaks their keyword
3. Wait for **PURPLE** LED (matching)
4. If **GREEN**: Retrieve items with displayed number
5. If **RED**: Allow retry (up to 3 attempts)
6. If 3 failures: Use manual override (L+R triggers)

#### Adjusting Settings
- **Volume**: Rotate potentiometer (left knob)
- **Sensitivity**: Use gamepad Left/Right buttons
- **View Stats**: Press **Button C** on AtomS3R

#### Troubleshooting
| Issue | LED Color | Solution |
|-------|-----------|----------|
| No voice detected | 🔵 Blue stays on | Check microphone, ask user to speak louder |
| Match fails repeatedly | 🔴 Red | Use manual override, verify user is original registrant |
| System frozen | No LED | Restart devices (press reset button) |
| WiFi disconnected | 🟠 Orange | System continues in offline mode |

## 🧪 Development Modes

### 1. Hackathon/Demo Mode
**File**: `src/hackathon_main.cpp`

- No WiFi required
- Simulated voice processing
- Instant feedback
- Perfect for demonstrations
- Uses pre-defined demo numbers: [42, 123, 456, 789, 101]

**Features**:
- Button A = Registration demo
- Button B = Retrieval demo
- Simulated 2-second recording
- Immediate number assignment

### 2. Production Mode
**File**: `src/production_main.cpp`

- Full WiFi integration
- ElevenLabs API support
- Real voice biometrics
- Persistent storage
- Advanced error handling
- Graceful degradation (works offline if needed)

**Features**:
- Actual voice capture and processing
- Database storage of profiles
- Network retry logic
- Performance analytics
- Staff override controls

### 3. Modular Development Mode
**File**: `src/main_modular.cpp`

- Uses all header files
- Component-based architecture
- Easy to test individual modules
- Ideal for feature development

## 🔐 Security Considerations

### Voice Biometric Security

1. **Multi-Factor Authentication**
   - Keyword (something you know)
   - Voice biometric (something you are)
   - Combined confidence scoring

2. **Privacy Protection**
   - Keywords are masked on display
   - Voice data hashed, not stored raw
   - Automatic profile expiration (configurable)

3. **Anti-Spoofing**
   - Voice characteristics hard to replicate
   - Multiple biometric markers analyzed
   - Timestamp-based session validation

### Data Security

- Voice profiles stored locally (no cloud dependency)
- I2C communication uses checksums
- Failed attempts logged for security audit
- Staff override requires physical button combination

## 📊 Analytics & Monitoring

The system tracks:

- **Peak Usage Times**: Identify busy periods
- **Average Retrieval Time**: System performance metric
- **Voice Recognition Accuracy**: Match success rate
- **Hardware Health**: Memory, uptime, error counts

Access logs via:
```bash
pio device monitor -e atoms3
```

Example output:
```
📊 === SYSTEM STATUS ===
Registered Users: 42
Next Number: 234
Free Memory: 245678 bytes
Uptime: 3600 seconds
Average Retrieval Time: 2.3s
Recognition Accuracy: 94.5%
```

## 🛠️ API Integration

### ElevenLabs Speech-to-Text

```cpp
std::string elevenlabs_speech_to_text(const std::vector<uint8_t>& audio_data) {
    // Sends audio to ElevenLabs API
    // Returns transcribed keyword
}
```

### ElevenLabs Text-to-Speech

```cpp
std::vector<uint8_t> elevenlabs_text_to_speech(const std::string& text) {
    // Converts text to audio
    // Returns audio data for playback
    // Used for confirmations: "Your items are number 42"
}
```

**Note**: API integration is optional. System works in simulation mode without API keys.

## 🎨 Customization

### Changing Number Range

Edit in `src/production_main.cpp`:
```cpp
uint16_t demo_numbers[] = {42, 123, 456, 789, 101, 234, 567, 890};
// Supports 001-999
```

### Adjusting Match Confidence

Edit in `src/voice_processor.cpp`:
```cpp
#define MATCH_THRESHOLD 0.85  // Default: 85%
// Range: 0.70 - 0.95
```

### Customizing Display Messages

Edit in `src/display_manager.cpp`:
```cpp
void show_assignment_number(uint16_t number) {
    M5.Display.drawString("YOUR NUMBER:", 10, 10);
    M5.Display.drawString(String(number).c_str(), 10, 40);
    // Customize text, colors, positions
}
```

## 🐛 Troubleshooting

### Common Issues

1. **Compilation Errors**
   ```bash
   # Missing secrets.h
   cp include/secrets.h.example include/secrets.h
   # Edit with your credentials
   ```

2. **Upload Fails**
   ```bash
   # Check USB connection
   pio device list
   
   # Erase flash and retry
   pio run -e atoms3 -t erase
   pio run -e atoms3 -t upload
   ```

3. **I2C Communication Issues**
   - Verify wiring (SDA, SCL, GND)
   - Check pull-up resistors (4.7kΩ recommended)
   - Ensure both devices powered

4. **Voice Not Recognized**
   - Increase microphone sensitivity
   - Check serial monitor for debug logs
   - Lower match threshold temporarily
   - Verify microphone is not obstructed

5. **System Freezes**
   - Check memory usage (serial monitor)
   - Reduce stored profiles if memory low
   - Reset devices and clear storage

## 📚 Additional Resources

- [M5Stack AtomS3R Documentation](https://docs.m5stack.com/en/core/AtomS3R)
- [Adafruit Feather ESP32-S3 Guide](https://learn.adafruit.com/adafruit-esp32-s3-reverse-tft-feather)
- [ElevenLabs API Documentation](https://elevenlabs.io/docs)
- [PlatformIO Documentation](https://docs.platformio.org/)

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on hardware
5. Submit a pull request

## 📄 License

This project is open source. See LICENSE file for details.

## 👥 Authors

Developed for hardware hackathon as a practical solution to cloakroom management.

## 🙏 Acknowledgments

- M5Stack for excellent hardware and libraries
- Adafruit for the Feather platform
- ElevenLabs for voice AI API
- PlatformIO for the development framework

---

**Need Help?** Open an issue on GitHub or check the serial monitor for debug logs.

**Happy Checking In/Out!** 🎤✨🔢
