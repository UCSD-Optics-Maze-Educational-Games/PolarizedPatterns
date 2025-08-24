# Polarized Patterns

## Purpose & Educational Goals

**Polarized Patterns** is an interactive educational game designed to teach people fundamental concepts in optics, specifically **light polarization phenomena**. By combining hands-on filter manipulation with riddle-solving challenges, this game makes abstract optical physics concepts tangible and engaging.

Students learn about:
- Light polarization and polarizing filters
- The relationship between filter orientation and light transmission
- How polarized light interacts with materials at different angles
- Critical thinking and problem-solving through riddle challenges
- Pattern recognition and scientific observation

## Introduction

This project transforms traditional polarization education into an immersive, game-based learning experience. Players must physically manipulate polarizing filters to reveal hidden numbers, then solve written riddles to determine the correct password sequence. The game bridges the gap between theoretical optics knowledge and hands-on experimentation, making learning both intuitive and memorable.

## How to Play

### Step 1: Observe the Polarizing Filters
- The game features **4 polarizing filters** with numbers attached to each filter
- A light source is positioned behind all 4 filters
- Numbers on filters may appear dim, unclear, or completely invisible initially

### Step 2: Rotate Filters for Optimal Visibility
- Rotate each polarizing filter to find the orientation where its number appears clearest
- Observe how light transmission changes as you rotate the filters
- Learn how polarization angle affects visibility and light intensity
- Record the numbers as they become visible through proper filter alignment

### Step 3: Solve Written Riddles
- Use the revealed numbers to solve a series of written riddles
- Each riddle corresponds to a specific 4-digit password
- Apply logical reasoning and pattern recognition to determine the correct sequence
- A series of riddles must be solved in sequence for complete victory

### Step 4: Enter Password Sequence
- Use the keypad to input each 4-digit password solution
- The LCD display shows your progress: "Correct: X of Y"
- Successful entry of all passwords triggers victory sequence
- System tracks completion and provides next game information

## Technical Skills & Components

### Hardware Components
- **ESP32 Microcontroller**: Main processing unit with built-in WiFi capabilities
- **16x2 LCD Display**: Real-time feedback and game progress tracking
- **4x3 Matrix Keypad**: Password input interface for riddle solutions
- **4 Polarizing Filters**: Core optical components with numbered labels
- **Light Source**: Illumination system for polarization demonstration
- **3D Printed Main Container**: Holds the keypad and stores necessary hardwares
- **Physical Filter Housing**: Rotatable mounting system for hands-on manipulation

### Software Features
- **Sequential Password System**: multi-stage progressive challenge structure
- **WiFi Connectivity**: Automatic connection with offline mode fallback
- **Google Apps Script Integration**: Cloud-based progress tracking and analytics
- **Real-time Progress Display**: Visual feedback showing completion status
- **Robust Error Handling**: Graceful offline operation and connection recovery
- **Game State Management**: Automatic reset and continuous play capability

### Technical Skills Demonstrated
- **Embedded Systems Programming**: Arduino/ESP32 development with advanced I/O
- **Network Programming**: HTTP client implementation with error resilience
- **Cloud Integration**: Google Sheets API integration via Apps Script
- **User Interface Design**: Multi-modal feedback and progress indication
- **Educational Game Design**: Progressive difficulty and pedagogical structure

## System Architecture

```
Physical Filter Assembly → ESP32 Microcontroller → LCD/Keypad Interface
                                    ↓
                            WiFi Network Connection
                                    ↓
                            Google Apps Script
                                    ↓
                            Google Sheets Database
```

## Setup & Installation

### Prerequisites
- Arduino IDE with ESP32 board package
- Required libraries:
  - `LiquidCrystal.h`
  - `Keypad.h`
  - `WiFi.h`
  - `HTTPClient.h`

### Hardware Setup
1. Connect LCD display to ESP32 pins (ex. 19, 23, 18, 17, 16, 15)
2. Wire keypad matrix to pins (ex. 14, 27, 26, 25 for rows; 33, 32, 35 for columns)
3. Install polarizing filters in rotatable housing with numbered labels
4. Position light source behind filter assembly
5. Ensure stable physical mounting for filter rotation

### Software Configuration
1. Upload the provided Arduino sketch to ESP32
2. Configure Google Apps Script URL in the code (if using Google Sheet)
3. Set WiFi credentials
4. Customize password sequences and riddle content as needed

## Educational Impact

This game successfully combines:
- **Kinesthetic Learning**: Physical manipulation of optical components
- **Visual Learning**: Direct observation of polarization effects
- **Cognitive Challenge**: Riddle-solving and logical reasoning
- **Progressive Mastery**: Sequential challenges building complexity
- **Technology Integration**: Modern IoT connectivity and data tracking

Students develop both scientific understanding of polarization physics and problem-solving skills while engaging with hands-on optical experimentation.

## Project Contributors

### Kesler Anderson
- **Original Game Design**: Conceptualized educational polarization-based puzzle game combining optics and riddle challenges
- **Hardware Fabrication**: Designed and constructed physical filter housing with rotatable polarizing filter system
- **Core Programming**: Developed foundational embedded systems code for keypad input, LCD display, and sequential game logic
- **Optical Integration**: Integrated and calibrated polarizing filters, light source, and optical measurement systems

### Nick Ji
- **WiFi Connectivity Implementation**: Modified code to add robust WiFi connection management with offline mode fallback capabilities
- **Google Sheets Integration**: Developed HTTP client functionality to send game completion status to Google Sheets via Apps Script for progress tracking
- **Dynamic Game Management**: Implemented system to retrieve and display next game information from Google Sheets database on LCD screen
- **System Maintenance**: Ongoing maintenance and optimization of game functionality and network connectivity
- **Documentation**: Authored this comprehensive README and technical specification

## License

This educational project is designed for academic use and learning purposes. Everyone is more than welcome to replicate and learn! For more details, please check the [MIT License](/LICENSE).
