#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define dot 15
#define dash 4
#define enter 0

const uint8_t dreb_delay = 150;
uint32_t last_click = 0;
String currentMorse = "";
String translatedText = "";

void updateDisplay() {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(F("=== MORSE TERMINAL ==="));

    display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

    display.setCursor(0, 18);
    display.print(F("Input:"));
    
    display.setTextSize(2);
    display.setCursor(0, 30);
    if (currentMorse.length() == 0) {
        display.print(F("_"));
    } else {
        display.print(currentMorse);
    }

    display.setTextSize(1);
    display.setCursor(0, 55);
    display.print(F("Text: "));
    display.print(translatedText);

    display.display();
}

void click() {
    uint32_t current_time = millis();

    if (digitalRead(dot) == LOW) {
        if (current_time - last_click > dreb_delay) {
            last_click = current_time;
            currentMorse += ".";
            Serial.println(currentMorse);
            updateDisplay();
        }
    }
    if (digitalRead(dash) == LOW) {
        if (current_time - last_click > dreb_delay) {
            last_click = current_time;
            currentMorse += "-";
            Serial.println(currentMorse);
            updateDisplay();
        }
    }
    if (digitalRead(enter) == LOW) {
        if (current_time - last_click > dreb_delay) {
            last_click = current_time;
            currentMorse += " | ";
            Serial.println(currentMorse);
            updateDisplay();
        }
    }
}

void setup() {
    Serial.begin(9600);
    
    pinMode(dot, INPUT_PULLUP);
    pinMode(dash, INPUT_PULLUP);
    pinMode(enter, INPUT_PULLUP);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.clearDisplay();
    updateDisplay();
}

void loop() {
    click();
}