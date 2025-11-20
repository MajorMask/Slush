/*
 * HACKATHON READY VERSION
 * Voice-Activated Cloakroom System
 * Copy this to main.cpp for AtomS3R
 */

#include <M5Unified.h>
#include "secrets.h"

// Simple demo without APIs - works immediately!
uint16_t demo_numbers[] = {42, 123, 456, 789, 101};
int current_demo = 0;

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
    
    Serial.println("Cloakroom Voice ID System Ready!");
    Serial.println("Button A = Register | Button B = Retrieve");
}

void loop() {
    M5.update();
    
    // REGISTRATION DEMO
    if (M5.BtnA.wasPressed()) {
        Serial.println("=== REGISTRATION MODE ===");
        
        // Recording simulation
        M5.Display.fillScreen(BLUE);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("RECORDING...", 10, 20);
        M5.Display.drawString("Say keyword", 10, 45);
        Serial.println("Simulating: User says 'Helsinki winter'");
        delay(2000);
        
        // Processing simulation
        M5.Display.fillScreen(YELLOW);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("PROCESSING...", 10, 30);
        Serial.println("Processing voice biometrics...");
        delay(1500);
        
        // Assignment
        uint16_t assigned = demo_numbers[current_demo];
        current_demo = (current_demo + 1) % 5;
        
        M5.Display.fillScreen(GREEN);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("ASSIGNED:", 10, 15);
        M5.Display.drawString(String(assigned).c_str(), 10, 40);
        
        Serial.printf("USER REGISTERED: Number %d\n", assigned);
        Serial.println("Voice biometric stored securely");
        
        delay(3000);
        
        // Return to ready
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("READY", 10, 30);
    }
    
    // RETRIEVAL DEMO  
    if (M5.BtnB.wasPressed()) {
        Serial.println("=== RETRIEVAL MODE ===");
        
        // Listening simulation
        M5.Display.fillScreen(CYAN);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("LISTENING...", 10, 20);
        M5.Display.drawString("Say keyword", 10, 45);
        Serial.println("Simulating: User says 'Helsinki winter'");
        delay(2000);
        
        // Voice matching simulation
        M5.Display.fillScreen(PURPLE);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("MATCHING...", 10, 30);
        Serial.println("Analyzing voice biometrics...");
        delay(1000);
        
        // Show result (last assigned number)
        uint16_t found_number = demo_numbers[(current_demo - 1 + 5) % 5];
        
        M5.Display.fillScreen(GREEN);
        M5.Display.setTextColor(BLACK);
        M5.Display.drawString("FOUND:", 10, 15);
        M5.Display.drawString(String(found_number).c_str(), 10, 40);
        
        Serial.printf("MATCH FOUND: Your items are number %d\n", found_number);
        Serial.println("Voice biometric authenticated successfully");
        
        delay(3000);
        
        // Return to ready
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(WHITE);
        M5.Display.drawString("READY", 10, 30);
    }
    
    delay(100);
}