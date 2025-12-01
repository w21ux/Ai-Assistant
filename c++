#include <Arduino.h>
#include <ESP32Servo.h>
#include "AudioTools.h"

// ------------------- Servo Setup -------------------
Servo headServo;
Servo tailServo;
const int HEAD_PIN = 14;
const int TAIL_PIN = 18;

// ------------------- I2S/MAX98357A Setup -------------------
#define I2S_BCLK_PIN  27
#define I2S_WS_PIN    26 // LRC (Word Select)
#define I2S_DATA_PIN  25 // DIN

const int SAMPLE_RATE = 11025;   // Reduced for smooth UART streaming
const int BITS_PER_SAMPLE = 16;
const int CHANNELS = 1; // Mono

// Audio objects
I2SStream i2sStream;
I2SConfig cfg;

// ------------------- Serial Protocol -------------------
enum State { WAITING_FOR_COMMAND, RECEIVING_SERVO, RECEIVING_AUDIO_SIZE, RECEIVING_AUDIO_DATA };
State currentState = WAITING_FOR_COMMAND;
uint32_t audioDataToReceive = 0;
const size_t AUDIO_CHUNK_SIZE = 64; // Match Python chunk size


// ------------------- Servo Animation -------------------
unsigned long lastServoMove = 0;
int headDirection = 1;
void animateServos() {
    if (millis() - lastServoMove > 150) {
        lastServoMove = millis();
        int currentAngle = headServo.read();
       
        if (currentAngle >= 100) headDirection = -1;
        else if (currentAngle <= 80) headDirection = 1;
        headServo.write(currentAngle + (headDirection * 5));
        tailServo.write(180 - headServo.read());
    }
}


void setup() {
    Serial.begin(921600); // Faster baud rate


    // --- Servo Setup ---
    headServo.attach(HEAD_PIN);
    tailServo.attach(TAIL_PIN);
    headServo.write(90);
    tailServo.write(90);


    // --- I2S Setup ---
    cfg.pin_bck = I2S_BCLK_PIN;
    cfg.pin_ws = I2S_WS_PIN;
    cfg.pin_data = I2S_DATA_PIN;
    cfg.sample_rate = SAMPLE_RATE;
    cfg.bits_per_sample = BITS_PER_SAMPLE;
    cfg.channels = CHANNELS;
    cfg.i2s_format = I2S_STD_FORMAT;


    i2sStream.begin(cfg);
}


void processServoCommand() {
    char buf[6];
    Serial.readBytes(buf, 6);
    char headStr[4] = {buf[0], buf[1], buf[2], 0};
    char tailStr[4] = {buf[3], buf[4], buf[5], 0};
    headServo.write(atoi(headStr));
    tailServo.write(atoi(tailStr));
    currentState = WAITING_FOR_COMMAND;
}


void processAudioData() {
    byte audioBuffer[AUDIO_CHUNK_SIZE];
   
    if (audioDataToReceive > 0) {
        size_t bytesRead = Serial.readBytes(audioBuffer, AUDIO_CHUNK_SIZE);


        if (bytesRead > 0) {
            i2sStream.write(audioBuffer, bytesRead);
            audioDataToReceive -= bytesRead;
            Serial.write('A'); // ACK
        }
    }


    if (audioDataToReceive <= 0) {
        i2sStream.flush();
        headServo.write(90);
        tailServo.write(90);
        currentState = WAITING_FOR_COMMAND;
    }
}

void loop() {
    if (currentState == RECEIVING_AUDIO_DATA) {
        animateServos();
        processAudioData();
    }


    if (Serial.available()) {
        switch (currentState) {
            case WAITING_FOR_COMMAND: {
                char command = Serial.read();
                if (command == '$') {
                    while (!Serial.available()) {}
                    char type = Serial.read();
                    if (type == 'S') {
                        currentState = RECEIVING_AUDIO_SIZE;
                    } else {
                        currentState = RECEIVING_SERVO;
                    }
                }
                break;
            }
            case RECEIVING_SERVO:
                if (Serial.available() >= 6) processServoCommand();
                break;
            case RECEIVING_AUDIO_SIZE: {
                byte size_bytes[4];
                Serial.readBytes(size_bytes, 4);
                audioDataToReceive = ((uint32_t)size_bytes[0] << 24) |
                                     ((uint32_t)size_bytes[1] << 16) |
                                     ((uint32_t)size_bytes[2] << 8) |
                                     (uint32_t)size_bytes[3];

                // Skip WAV header
                byte header_dummy[44];
                if (audioDataToReceive >= 44) {
                    Serial.readBytes(header_dummy, 44);
                    audioDataToReceive -= 44;
                }
               
                if (audioDataToReceive > 0) {
                    Serial.write('A'); // ACK
                    currentState = RECEIVING_AUDIO_DATA;
                } else {
                    currentState = WAITING_FOR_COMMAND;
                }
                break;
            }
            case RECEIVING_AUDIO_DATA:
                break; // handled above
        }
    }
}
