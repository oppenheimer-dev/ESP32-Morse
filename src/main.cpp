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
#define backspace 23
#define enter 0
#define buz_pin 18
#define led_pin 2   

const uint8_t dreb_delay = 230;
uint32_t last_click = 0;
uint32_t last_sound = 0;
uint16_t sound_duration = 0;
String currentMorse = "";
String translatedText = "";

struct MorseCode {
    const char* pattern; 
    char letter;         
};

// 2. Таблица всего алфавита и цифр)
const MorseCode morseTable[] = {
    {".-", 'A'},   {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'},
    {".", 'E'},    {"..-.", 'F'}, {"--.", 'G'},  {"....", 'H'},
    {"..", 'I'},   {".---", 'J'}, {"-.-", 'K'},  {".-..", 'L'},
    {"--", 'M'},   {"-.", 'N'},   {"---", 'O'},  {".--.", 'P'},
    {"--.-", 'Q'}, {".-.", 'R'},  {"...", 'S'},  {"-", 'T'},
    {"..-", 'U'},  {"...-", 'V'}, {".--", 'W'},  {"-..-", 'X'},
    {"-.--", 'Y'}, {"--..", 'Z'},
    {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'},
    {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'},
    {"---..", '8'}, {"----.", '9'}
};

const int tableSize = sizeof(morseTable) / sizeof(morseTable[0]);


char decodeMorse(String morse) {
    for (int i = 0; i < tableSize; i++) {
        if (morse == morseTable[i].pattern) {
            return morseTable[i].letter; 
        }
    }
    return '?'; 
}

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

void physical_view(int ch = -1) {
    uint32_t current_time = millis();

    //on sound
    if (ch == 0) {
        tone(buz_pin, 750);
        GPIO.out_w1ts=(1<<led_pin);
        last_sound = current_time; 
        sound_duration = 100;
    } 
    else if (ch == 1) {
        tone(buz_pin, 750);
        GPIO.out_w1ts=(1<<led_pin);
        last_sound = current_time; 
        sound_duration = 300;
    }

    //off sound 
    if (sound_duration > 0) {
        if (current_time - last_sound >= sound_duration) {
            noTone(buz_pin);
            GPIO.out_w1tc=(1<<led_pin);
            sound_duration = 0;
        }
    }
}

void click() {
    uint32_t current_time = millis();

    if (digitalRead(dot) == LOW) {
        if (current_time - last_click > dreb_delay) {
            last_click = current_time;
            currentMorse += ".";
            Serial.println(currentMorse);
            physical_view(0);
            updateDisplay();
        }
    }
    if (digitalRead(dash) == LOW) {
        if (current_time - last_click > dreb_delay) {
            last_click = current_time;
            currentMorse += "-";
            Serial.println(currentMorse);
            physical_view(1);
            updateDisplay();
        }
    }
    if (digitalRead(enter) == LOW) {
        if (current_time - last_click > dreb_delay) {
            last_click = current_time;
            if (currentMorse.length() > 0) {
            char decodedChar = decodeMorse(currentMorse);

            translatedText += decodedChar;
            currentMorse = ""; 
            
            Serial.print(F("Decoded: "));
            Serial.println(decodedChar);
        } else {
            translatedText += " ";
        }
            Serial.println(currentMorse);
            updateDisplay();
        }
    }
    if(digitalRead(backspace) == LOW){
        if(current_time -  last_click > dreb_delay){
            last_click = current_time;
            if(currentMorse.length() >0){
                currentMorse.remove(currentMorse.length() - 1);
                Serial.println(currentMorse);
                updateDisplay();
            }
        }
    }
}

void setup() {
    Serial.begin(9600);
    GPIO.enable_w1ts=(1<<led_pin);
    pinMode(dot, INPUT_PULLUP);
    pinMode(dash, INPUT_PULLUP);
    pinMode(enter, INPUT_PULLUP);
    pinMode(backspace, INPUT_PULLUP);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.clearDisplay();
    updateDisplay();
}

void loop() {
    click();
    physical_view();
}