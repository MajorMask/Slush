#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuration - UPDATE THESE!
#define WIFI_SSID "YOUR_HACKATHON_WIFI"
#define WIFI_PASSWORD "PASSWORD"
#define ELEVENLABS_API_KEY "YOUR_API_KEY"

// Simple demo system
uint16_t demo_numbers[] = {42, 123, 456, 789};
int current_demo_index = 0;

void setup() {
    Serial.begin(115200);
    
    // Initialize M5
    M5.begin();
    M5.Display.setTextSize(2);
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE);
    M5.Display.drawString("CLOAKROOM", 10, 10);
    M5.Display.drawString("VOICE ID", 10, 35);
    M5.Display.drawString("READY", 10, 60);
    
    // Connect WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    
    M5.Display.drawString("WIFI OK", 10, 85);
}

void loop() {
    M5.update();
    
    if (M5.BtnA.wasPressed()) {
        // Registration Demo
        M5.Display.fillScreen(BLUE);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("RECORDING...", 10, 20);
        
        delay(2000); // Simulate recording
        
        M5.Display.fillScreen(YELLOW);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("PROCESSING...", 10, 20);
        
        delay(1000); // Simulate processing
        
        uint16_t assigned_number = demo_numbers[current_demo_index];
        current_demo_index = (current_demo_index + 1) % 4;
        
        M5.Display.fillScreen(GREEN);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("NUMBER:", 10, 10);
        M5.Display.drawString(String(assigned_number).c_str(), 10, 35);
        
        delay(3000);
        
        // Return to ready
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("READY", 10, 30);
    }
    
    if (M5.BtnB.wasPressed()) {
        // Retrieval Demo
        M5.Display.fillScreen(CYAN);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("RETRIEVAL", 10, 20);
        
        delay(2000); // Simulate listening
        
        // Show last assigned number
        M5.Display.fillScreen(GREEN);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("FOUND:", 10, 10);
        M5.Display.drawString(String(demo_numbers[current_demo_index-1]).c_str(), 10, 35);
        
        delay(3000);
        
        // Return to ready
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("READY", 10, 30);
    }
    
    delay(100);
}