# TrafficBot

An Arduino-based intelligent traffic management system consisting of a line-following robot and smart traffic lights. Built on Boe-Bot platform, it simulates real-world intersection control with autonomous navigation and sensor-based traffic light switching.

---

## Description

TrafficBot processes intersection traffic control through two independent Arduino systems that communicate visually. The system:

- Follows black lines using QTI sensors with autonomous navigation
- Detects and responds to red traffic lights using color sensor technology  
- Controls two-way intersection traffic with ultrasonic proximity detection
- Manages traffic flow with configurable timing and cooldown periods
- Supports obstacle avoidance and manual power control

---

## Requirements

### Hardware Components

- **2x Arduino Uno** (Boe-Bot platform for robot)
- **2x HC-SR04** ultrasonic sensors  
- **TCS3200** color sensor
- **3x QTI** line sensors
- **2x Servo motors** (continuous rotation)
- **6x LEDs** (2 red, 2 yellow, 2 green)
- **Push button**
- **Breadboards and jumper wires**

### Software Libraries

- Arduino IDE
- **Servo.h**: <https://www.arduino.cc/reference/en/libraries/servo/>

---

## Installation

1. Download Arduino IDE: <https://www.arduino.cc/en/software>
2. Clone or download this repository
3. Upload `Robot.ino` to robot's Arduino Uno
4. Upload `TrafficLight.ino` to traffic light Arduino Uno

---

## Required Files

The system requires the following Arduino sketch files:

```text
TrafficBot/
├── Robot/
│   └── Robot.ino           # Line-following robot code
├── TrafficLight/        
│   └── TrafficLight.ino    # Intersection traffic control 
└── Schematics/             # Connection schematics
    ├── Robot.svg
    └── TrafficLight.svg
```

---

## Configuration Variables

### Robot Calibration Settings

- **QTI_THRESHOLD**: Line detection sensitivity (default: 75)
- **Color thresholds**: Red light detection parameters in `WhatColor()`  
- **Servo pulse widths**: Movement speed calibration (1300-1700 range)
- **Ultrasonic timeout**: Obstacle detection distance (default: 10cm)

### Traffic Light Timing

- **MEASUREINTERVAL**: Side road activation cooldown (default: 72000ms)
- **Green light duration**: Side road green time (default: 10 seconds)
- **Detection range**: Robot proximity trigger distance (< 10cm)

---

## Usage

### 3D printable extras  

1. Print for LED mounting: <https://www.thingiverse.com/thing:7135281>
2. Print for wire management: <https://www.thingiverse.com/thing:7135273>

### System Operation

Power both Arduino controllers and position robot on track:

```bash
# Robot starts in standby mode - press button to activate
# Traffic light initializes: main road green, side road red
```

**Robot Behavior:**

- **Line Following**: Autonomous navigation with LED direction indicators  
- **Red Light Detection**: Complete stop with left LED status indicator
- **Obstacle Avoidance**: Pause with right LED warning indicator
- **Manual Control**: Power button toggle with debounce protection

**Traffic Light Logic:**  

- **Default State**: Main road maintains green light priority
- **Side Road Activation**: Green light triggered by robot proximity (< 10cm)
- **Timing Control**: Maximum 10-second green duration with 72-second cooldown
- **Visual Communication**: Robot reads light colors directly (no wireless communication)

---

## Error Handling

**Common Issues:**

- **Line Detection Failure**: Calibrate QTI_THRESHOLD for track surface and lighting
- **Color Sensor Inaccuracy**: Adjust RGB thresholds in WhatColor() function for ambient light
- **Servo Calibration**: Modify pulse width values for proper wheel alignment
- **Ultrasonic Interference**: Check sensor mounting and detection range settings

**Troubleshooting:**

- Monitor Serial output for sensor readings and calibration data
- Verify all connections match provided wiring diagrams  
- Test individual components before full system integration
- Use GUI Serial Monitor for real-time debugging and threshold adjustment

---

## Tested Systems

- **Arduino Uno** (fully tested)
- **Arduino IDE 1.8+** (compatible with 2.x)
