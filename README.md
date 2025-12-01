# Ai-Assistant

**Hardware Requirements:**

**Microcontroller:** An ESP32 development board is required to serve as the central processing unit. Its integrated Wi-Fi is critical for internet connectivity to cloud services.

**Audio Input:** An INMP441 I2S Microphone is used to capture the user's voice with high fidelity and low noise.

**Audio Output:** A MAX98357A I2S Amplifier connected to a small speaker (e.g., 3W, 4-8 Ohm) is used to generate the assistant's voice.

**Actuators:** Two or more SG90 or MG90S micro servo motors are required to provide head and tail movement.

**Chassis & Assembly:** Cardboard, hot glue, and basic crafting tools are needed for prototyping the robot's body.

**Power Supply:** A stable 5V DC power supply (minimum 2A) capable of powering the ESP32 and the servo motors simultaneously.




**Software Requirements**

**Development Environment:** The Arduino IDE, configured with the ESP32 board manager.

**Programming Language:** The core logic is written in C++ within the Arduino framework.

**Key Libraries:**

WiFi/HTTP Client library for connecting to APIs.
ArduinoJson library for parsing API responses.
I2S driver library for handling the microphone and speaker.
Servo control library for managing the animatronics.



**WIRING/CIRCUIT DIAGRAMS:**

**WIRING IN SERVO MOTORS:**

<img width="794" height="403" alt="image" src="https://github.com/user-attachments/assets/002d3438-c070-470f-b32b-caf0fbc6b18d" />


**WIRING IN MAX98357A : **

<img width="914" height="338" alt="image" src="https://github.com/user-attachments/assets/aaf12569-c36c-4a6c-bae5-11df243c953d" />


**WIRING IN INMP441 :**

<img width="926" height="566" alt="image" src="https://github.com/user-attachments/assets/d494d794-1b17-450d-a49a-85ddb70bb435" />


**!!CHANGE THE PINS ARRANGEMENTS IF NEEDED!!**



