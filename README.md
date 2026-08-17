# Smart Parking System using ESP32

A two-slot smart parking prototype built with an ESP32, two HC-SR04 ultrasonic sensors, an SSD1306 OLED display, status LEDs, and a buzzer. The complete system was developed and functionally verified using the Wokwi virtual simulator.

## Project Overview

The system automatically detects whether each parking slot is **FREE** or **OCCUPIED** by measuring distance with an HC-SR04 ultrasonic sensor.

- Distance ≤ 35 cm → **OCCUPIED**
- Distance > 35 cm → **FREE**
- Five readings are averaged for more stable measurements.
- Two consecutive matching decisions are required before changing the confirmed slot state.
- The system calculates the number of available slots.
- Red/green LEDs indicate each slot's state.
- An SSD1306 OLED displays live parking information.
- A buzzer activates when all monitored slots are occupied.
- Serial Monitor provides diagnostic information.

## Features

- ESP32-based embedded system
- 2 independent parking slots
- 2 × HC-SR04 ultrasonic sensors
- 35 cm configurable occupancy threshold
- Distance averaging
- State-confirmation logic
- 4 status LEDs
- SSD1306 128×64 I2C OLED
- Full-parking buzzer alert
- Serial Monitor diagnostics
- Wokwi virtual simulation

## Hardware / Components

| Component | Quantity |
|---|---:|
| ESP32 DevKit | 1 |
| HC-SR04 Ultrasonic Sensor | 2 |
| SSD1306 128×64 OLED | 1 |
| Green LED | 2 |
| Red LED | 2 |
| Buzzer | 1 |
| Resistors for LEDs | 4 |
| Jumper wires / breadboard | As required |

## Pin Configuration

| Device | Signal | ESP32 GPIO |
|---|---|---:|
| HC-SR04 Slot 1 | TRIG | GPIO 5 |
| HC-SR04 Slot 1 | ECHO | GPIO 18 |
| HC-SR04 Slot 2 | TRIG | GPIO 19 |
| HC-SR04 Slot 2 | ECHO | GPIO 23 |
| OLED | SDA | GPIO 21 |
| OLED | SCL | GPIO 22 |
| Slot 1 Green LED | Signal | GPIO 13 |
| Slot 1 Red LED | Signal | GPIO 14 |
| Slot 2 Green LED | Signal | GPIO 25 |
| Slot 2 Red LED | Signal | GPIO 26 |
| Buzzer | Signal | GPIO 27 |

## Software

- Arduino-compatible C/C++
- ESP32 Arduino core
- Wokwi
- Adafruit GFX Library
- Adafruit SSD1306 Library

## How It Works

1. The ESP32 triggers the first HC-SR04 sensor.
2. The Echo pulse duration is measured.
3. Distance is calculated using:

   `Distance = Echo Time × 0.0343 / 2`

4. Five valid measurements are averaged.
5. The averaged distance is compared with the 35 cm threshold.
6. The same process is performed for Slot 2.
7. Confirmed slot states are used to calculate available spaces.
8. LEDs, OLED, buzzer, and Serial Monitor are updated.
9. The process repeats continuously.

## Test Results

The implementation was verified under the following simulated conditions:

| Slot 1 | Slot 2 | Result |
|---|---|---|
| ~50 cm | ~50 cm | Both FREE, 2/2 available |
| ~20 cm | ~50 cm | Slot 1 OCCUPIED, 1/2 available |
| ~50 cm | ~20 cm | Slot 2 OCCUPIED, 1/2 available |
| ~20 cm | ~20 cm | Both OCCUPIED, 0/2, PARKING FULL |

The final parking-full test correctly produced:

```text
Slot 1: OCCUPIED
Slot 2: OCCUPIED
Available Slots: 0 / 2
STATUS: PARKING FULL
```

## Repository Structure

```text
smart-parking-esp32/
├── sketch.ino
├── diagram.json
├── libraries.txt
└── README.md
```

Documentation and simulation evidence can be maintained in the project Drive folder.

## Simulation

Wokwi Project:

https://wokwi.com/projects/472531313410758657

## Limitations

This is a two-slot prototype validated through virtual simulation. Physical deployment would require sensor mounting calibration, environmental testing, and appropriate electrical protection. In a physical ESP32 build, the HC-SR04 Echo signal should be reduced to a safe 3.3 V logic level using an appropriate resistor divider or level shifter.

## Future Scope

- More parking slots
- ESP32 Wi-Fi dashboard
- MQTT / cloud monitoring
- Mobile notifications
- RFID access control
- Automatic parking barrier
- Vehicle identification
- Parking reservation and payment
- Occupancy analytics

## Project Status

**Core implementation:** Complete  
**Wokwi simulation:** Functionally verified  
**GitHub repository:** In progress / being documented
