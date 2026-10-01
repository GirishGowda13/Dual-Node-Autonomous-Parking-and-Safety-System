# Dual-Node Autonomous Parking and Safety System

A distributed embedded system that combines **autonomous parking** with an independent **vehicle safety-monitoring node** using two PIC16F877A microcontrollers and UART communication.

---

##  Overview

The **Dual-Node Autonomous Parking and Safety System** is designed as a two-node embedded architecture.

**Node 1** is responsible for autonomous parking, obstacle detection, vehicle movement, steering control, and user indication.

**Node 2** operates independently as a safety-monitoring unit. It monitors environmental and distance-related conditions, determines the current safety state, and communicates that state to Node 1 through UART.

The system uses three safety states:

```text
SAFE → WARNING → CRITICAL
```

When a critical condition is detected, Node 2 sends a critical status to Node 1, allowing Node 1 to stop vehicle movement.

---

## Key Features

* Two-node distributed embedded architecture
* PIC16F877A-based controllers
* Autonomous parking operation
* Multi-directional ultrasonic obstacle detection
* Servo-based steering control
* Independent safety-monitoring node
* LDR-based environmental monitoring
* Rain sensing
* SAFE / WARNING / CRITICAL safety states
* UART communication between nodes
* LCD status display
* Directional/status LEDs
* Buzzer-based warning indication
* Critical-condition vehicle stopping

---

## System Architecture

The system consists of two independent embedded nodes.

```text
┌──────────────────────────────┐
│            NODE 2            │
│      Safety Monitoring       │
│                              │
│     Ultrasonic Sensors       │
│            LDR               │
│        Rain Sensor           │
│             │                │
│             ▼                │
│       Risk Assessment        │
│             │                │
│  SAFE / WARNING / CRITICAL   │
│                              │
└──────────────┬───────────────┘
               │
              UART
               │
               ▼
┌──────────────────────────────┐
│            NODE 1            │
│      Autonomous Parking      │
│                              │
│      Ultrasonic Sensors      │
│              │               │
│              ▼               │
│      Parking Controller      │
│              │               │
│         ┌────┴────┐          │
│         ▼         ▼          │
│       Motors     Servo       │
│                              │
│      LCD / LEDs / Buzzer     │
└──────────────────────────────┘
```

### Block Diagram

![System Architecture](images/block_diagram.png)

---

## 🚗 Node 1 — Autonomous Parking

Node 1 acts as the primary autonomous parking controller.

### Responsibilities

* Detect obstacles around the vehicle
* Measure distances using ultrasonic sensors
* Identify a suitable parking space
* Control vehicle movement
* Control steering using a servo motor
* Provide directional indication
* Display system information on the LCD
* Respond to safety information received from Node 2

### Main Outputs

* DC motors
* Servo motor
* Directional LEDs
* Buzzer
* LCD

---

##  Node 2 — Safety Monitoring

Node 2 operates as an independent safety-monitoring controller.

### Inputs

* Two ultrasonic sensors
* LDR
* Rain sensor

### Responsibilities

* Monitor surrounding conditions
* Evaluate sensor information
* Determine the current safety state
* Provide visual and audible indication
* Send the safety state to Node 1 through UART

### Safety States

| State    | Code | Meaning                                   |
| -------- | ---- | ----------------------------------------- |
| SAFE     | `S`  | Normal operating condition                |
| WARNING  | `W`  | Caution condition                         |
| CRITICAL | `C`  | Critical condition requiring vehicle stop |

---

##  UART Communication

The two PIC16F877A nodes communicate using UART.

### Connection

```text
Node 1 RC6 (TX) ─────────→ Node 2 RC7 (RX)

Node 1 RC7 (RX) ←───────── Node 2 RC6 (TX)

Node 1 GND      ────────── Node 2 GND
```

### Safety Message Protocol

Node 2 sends a single character representing its current safety state.

```text
'S' → SAFE
'W' → WARNING
'C' → CRITICAL
```

The primary safety-status path is:

```text
Node 2
   │
   │ Safety assessment
   ▼
'S' / 'W' / 'C'
   │
   │ UART
   ▼
Node 1
   │
   ▼
Vehicle response
```

---

##  Working Principle

### Normal Operation

When the system is operating normally:

```text
Node 2 → SAFE
          ↓
Node 1 receives 'S'
          ↓
Normal autonomous parking
```

### Warning Condition

When Node 2 detects a warning condition:

```text
Node 2 → WARNING
          ↓
Node 2 sends 'W'
          ↓
Node 1 receives warning state
          ↓
Caution indication
```

### Critical Condition

When a critical condition is detected:

```text
Node 2 → CRITICAL
          ↓
Node 2 sends 'C'
          ↓
Node 1 receives critical state
          ↓
Vehicle movement stopped
```

This provides an independent safety-monitoring layer for the autonomous parking controller.

---

##  Hardware Components

### Controllers

* PIC16F877A × 2

### Node 1

* Ultrasonic sensors
* DC motors
* Motor driver
* Servo motor
* LCD
* LEDs
* Buzzer

### Node 2

* Ultrasonic sensors
* LDR
* Rain sensor
* LCD
* Status LEDs
* Buzzer

### Communication

* UART interface between Node 1 and Node 2

---

##  Hardware Documentation

Detailed hardware information is available in the `hardware/` directory.

```text
hardware/
│
├── node1/
│   └── pinout.md
│
├── node2/
│   └── pinout.md
│
└── wiring/
    └── connections.md
```

### Hardware Connection Diagram

![Hardware Connections](images/hardware_connections.png)

---

##  Firmware

The firmware is divided into two independent node implementations.

```text
firmware/
│
├── node1/
│   └── main.c
│
└── node2/
    └── main.c
```

### Node 1 Firmware

Responsible for:

* Ultrasonic sensing
* Parking-space detection
* Vehicle movement
* Servo control
* LEDs
* Buzzer
* LCD
* UART communication

### Node 2 Firmware

Responsible for:

* Sensor acquisition
* Safety-state determination
* LEDs
* Buzzer
* LCD
* UART communication

---

## Testing and Validation

The prototype was tested at both individual-node and integrated-system levels.

### Node 1

* Ultrasonic obstacle detection
* Parking-space detection
* Servo movement
* Motor control
* Directional indication
* Buzzer operation
* LCD operation

### Node 2

* Ultrasonic sensing
* LDR sensing
* Rain sensing
* Safety-state indication
* LCD operation
* Buzzer operation

### Integrated System

* UART communication
* SAFE state transmission
* WARNING state transmission
* CRITICAL state transmission
* Critical-condition vehicle stopping
* Autonomous parking operation

Detailed testing information is available in:

[`docs/testing.md`](docs/testing.md)

---

##  Working Demonstration

A working demonstration of the complete system is included in the project.

The demonstration covers:

1. Hardware setup
2. Node 1 operation
3. Node 2 safety monitoring
4. Autonomous parking
5. Safety-state transitions
6. UART communication
7. Critical-condition vehicle stopping

Demo video:

```text
videos/system_demo.mp4
```

---

##  Documentation

Detailed technical documentation:

| Document                                                      | Description                            |
| ------------------------------------------------------------- | -------------------------------------- |
| [`architecture.md`](docs/architecture.md)                     | Overall system architecture            |
| [`communication_protocol.md`](docs/communication_protocol.md) | UART communication and safety protocol |
| [`testing.md`](docs/testing.md)                               | Testing and validation                 |
| [`node1/pinout.md`](hardware/node1/pinout.md)                 | Node 1 pin assignments                 |
| [`node2/pinout.md`](hardware/node2/pinout.md)                 | Node 2 pin assignments                 |
| [`wiring/connections.md`](hardware/wiring/connections.md)     | Hardware wiring information            |

---

##  Project Structure

```text
Dual-Node-Autonomous-Parking-and-Safety-System/
│
├── README.md
├── .gitignore
├── LICENSE
│
├── firmware/
│   ├── node1/
│   │   └── main.c
│   └── node2/
│       └── main.c
│
├── hardware/
│   ├── node1/
│   │   └── pinout.md
│   ├── node2/
│   │   └── pinout.md
│   └── wiring/
│       └── connections.md
│
├── docs/
│   ├── architecture.md
│   ├── communication_protocol.md
│   └── testing.md
│
├── images/
│   ├── hardware_connections.png
│   ├── block_diagram.png
│   ├── node1.jpg
│   ├── node2.jpg
│   └── complete_system.jpg
│
└── videos/
    └── system_demo.mp4
```

---

##  Future Improvements

Possible future development areas include:

* CAN-based communication between nodes
* More advanced sensor fusion
* Improved parking-space detection
* Closed-loop motor control
* PCB implementation
* Wireless monitoring
* More robust fault detection
* Data logging and analysis
* Integration with additional vehicle safety systems

---

## Project

**Project:** Dual-Node Autonomous Parking and Safety System

**Platform:** PIC16F877A

**Architecture:** Distributed Embedded System

**Communication:** UART

**Application Areas:**

* Autonomous parking
* Embedded vehicle control
* Safety monitoring
* Distributed embedded systems
* Sensor-based decision making
