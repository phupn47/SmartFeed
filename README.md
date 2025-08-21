# Smart Feed
This is my self-developed IoT project designed to simplify the process of feeding pets. The system allows feeding in three different ways: directly from the feeder machine, remotely via the Blynk Application, or automatically through scheduled feeding.

## Prototype Drawing

<img src="https://github.com/user-attachments/assets/f8261e24-d53b-4e4e-8b04-b90027b989ac" alt="prototype_drawing" width="400" height="500" >


## Features
- Manual Feeding (Feeder Button): Users can press the physical button on the feeder to dispense food instantly.
- Remote Feeding (Blynk App): Food can be dispensed remotely by pressing the button within the Blynk application.
- Automated Feeding (Scheduled): Users can set specific times in the Blynk app to automate feeding.
- Data Logging (Google Sheets): All feeding activities are automatically recorded in Google Sheets for tracking and history review.

## Devices and Tools
- `ESP32` Microcontroller for IoT connectivity and device control
- `Servo Motor (SG90)` Controls the food dispensing mechanism
- `Arduino IDE` Development environment for coding and uploading firmware
- `Blynk Application` Mobile app for remote control and automation
- `Google Sheets` Stores feeding logs for monitoring and analysis

