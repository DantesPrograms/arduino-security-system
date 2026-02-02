# Arduino R4 WiFi Security System

A network-enabled security system with PIR motion detection, web-based control, and PIN authentication.

![System Status](https://img.shields.io/badge/status-stable-green)
![Arduino](https://img.shields.io/badge/Arduino-R4%20WiFi-blue)
![License](https://img.shields.io/badge/license-MIT-blue)

## 📋 Table of Contents


- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Web Interface](#web-interface)
- [API Documentation](#api-documentation)
- [Troubleshooting](#troubleshooting)
- [Safety Considerations](#safety-considerations)
- [Contributing](#contributing)
- [License](#license)
- [Author](#author)

## 🔍 Overview

This project implements a comprehensive security system using the Arduino R4 WiFi platform. It combines passive infrared (PIR) motion detection with a web-based control interface, providing real-time monitoring and remote system management capabilities.

The system is designed for educational purposes and rapid prototyping, demonstrating embedded systems design principles, state machine architecture, and IoT connectivity.

## ✨ Features

- **Ultrasonic Motion Detection**: HC-SR04 distance-based detection (no heat false alarms!)
- **Smart Distance Monitoring**: Detects actual movement by measuring distance changes
- **Configurable Sensitivity**: Adjustable detection range and movement threshold
- **Visual Indicators**: Dual LED system for status and alarm indication
- **Audible Alerts**: Piezoelectric buzzer with pulsing alarm pattern
- **Web Interface**: Responsive HTML interface for remote monitoring and control
- **PIN Authentication**: Secure 4-digit PIN protection (default: 0421)
- **Session Management**: 30-minute authenticated sessions with timeout
- **REST API**: JSON endpoints for programmatic control
- **State Machine Logic**: Three-state system (DISARMED, ARMED, ALARM_TRIGGERED)
- **Persistent Alarms**: Alarm continues until manually disarmed via web interface
- **Real-time Updates**: AJAX-powered dashboard with no page reloads

## 🛠️ Hardware Requirements

### Components

| Component | Quantity | Specifications |
|-----------|----------|----------------|
| Arduino R4 WiFi | 1 | Main controller with WiFi capability |
| HC-SR04 Ultrasonic Sensor | 1 | Distance-based motion detection |
| LED (3mm) | 1 | Status indicator (any color) |
| LED (5mm) | 1 | Alarm indicator (red recommended) |
| Active Piezo Buzzer | 1 | 5V active buzzer |
| Resistors (220Ω) | 2 | Current limiting for LEDs |
| Jumper Wires | Assorted | Male-to-male for connections |
| Breadboard | 1 | For prototyping (optional) |
| Enclosure | 1 | Project box (optional) |

### Estimated Cost

- **Without Arduino**: $8-22 USD
- **Complete Kit**: $40-50 USD (including Arduino R4 WiFi)

## 📌 Pin Configuration

| Function | Arduino Pin | Component | Notes |
|----------|-------------|-----------|-------|
| Ultrasonic Trigger | D3 | HC-SR04 TRIG | Motion detection sensor |
| Ultrasonic Echo | D4 | HC-SR04 ECHO | Motion detection sensor |
| Status LED | D8 | 3mm LED (+ 220Ω resistor) | Active when ARMED |
| Alarm LED | D9 | 5mm LED (+ 220Ω resistor) | Active during ALARM |
| Buzzer | D10 | Active Piezo Buzzer | Pulsing pattern |
| Power (5V) | 5V | All components | Common power rail |
| Ground | GND | All components | Common ground |

**See `pins.xml` for detailed connection diagram compatible with Fritzing.**

## 🚀 Installation

### Prerequisites

1. **Arduino IDE** (version 2.0 or higher)
   - Download from [arduino.cc](https://www.arduino.cc/en/software)

2. **Arduino R4 WiFi Board Support**
   ```
   Tools → Board → Boards Manager → Search "UNO R4 WiFi" → Install
   ```

3. **Required Libraries**
   - WiFiS3 (included with board package)

### Hardware Setup

1. **Assemble the circuit** according to the pin configuration table
2. **Verify all connections** before powering on
3. **Ensure proper polarity** for LEDs and PIR sensor
4. **Connect Arduino R4 WiFi** to your computer via USB-C cable

### Software Setup

1. **Clone or download** this repository
   ```bash
   git clone https://github.com/yourusername/arduino-security-system.git
   cd arduino-security-system
   ```

2. **Open** `security_system.ino` in Arduino IDE

3. **Configure WiFi credentials** (see Configuration section)

4. **Select board and port**
   ```
   Tools → Board → Arduino UNO R4 WiFi
   Tools → Port → [Select your Arduino's port]
   ```

5. **Upload the sketch**
   - Click the Upload button (→) or press Ctrl+U

## ⚙️ Configuration

### WiFi Settings

Edit the following lines in `security_system.ino`:

```cpp
const char* ssid = "YOUR_WIFI_SSID";        // Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi password
```

### Security PIN

Default PIN is **1234**. To change:

```cpp
const String SECURITY_PIN = "1234";  // Change to your 4-digit PIN
```

### Session Timeout

Default session timeout is 30 minutes. To modify:

```cpp
const unsigned long SESSION_TIMEOUT = 1800000;  // Time in milliseconds
```

## 📱 Usage

### Initial Startup

1. **Power on** the Arduino
2. **Open Serial Monitor** (115200 baud) to view system status
3. **Note the IP address** displayed in the serial output
4. **Navigate to the IP address** in your web browser

Example serial output:
```
=== Arduino R4 WiFi Security System ===
Hardware initialized - System DISARMED
Connecting to WiFi: MyNetwork
..........
WiFi connected!
IP Address: 192.168.1.150
Web server started

=== System Information ===
WiFi Status: Connected
IP Address: 192.168.1.150
Access web interface at: http://192.168.1.150
System State: DISARMED
========================
```

### Web Interface Access

1. Open browser and go to `http://[ARDUINO_IP_ADDRESS]`
2. Enter PIN: **0421** (or your custom PIN)
3. Click **UNLOCK** to access controls

### Operating the System

#### To Arm the System:
1. Login to web interface
2. Click **ARM SYSTEM** button
3. Status LED will illuminate
4. Ultrasonic sensor sets baseline distance
5. System now monitoring for motion

#### When Motion is Detected:
- Distance change detected by ultrasonic sensor
- Alarm LED activates
- Buzzer sounds with pulsing pattern (500ms intervals)
- Alarm continues indefinitely until disarmed
- Web interface shows "ALARM ACTIVE" in red

#### To Disarm the System:
1. Login to web interface (if not already)
2. Click **DISARM SYSTEM** button
3. All outputs deactivate
4. System returns to safe state

#### To Logout:
- Click **LOGOUT** button at bottom of control page

## 🌐 Web Interface

### Login Page
- Clean, secure login interface
- 4-digit PIN entry
- Invalid PIN notification
- Mobile-responsive design

### Control Dashboard
- Real-time system status display
- Color-coded state indicators:
  - 🟢 **Green**: DISARMED (safe)
  - 🟡 **Yellow**: ARMED (monitoring)
  - 🔴 **Red**: ALARM TRIGGERED (alert)
- ARM/DISARM control buttons
- Auto-refresh every 3 seconds
- System information panel
- Logout option

## 🔌 API Documentation

### Endpoints

#### `GET /`
Returns web interface (login or dashboard)
- **Authentication**: Required (redirects to login if not authenticated)

#### `POST /login`
Authenticate with PIN
- **Parameters**: `pin` (4-digit string)
- **Response**: Sets session cookie, redirects to `/`
- **Authentication**: Not required

#### `GET /logout`
Invalidate current session
- **Response**: Clears session cookie, redirects to `/`
- **Authentication**: Not required

#### `GET /status`
Returns current system state
- **Authentication**: Required
- **Response**: JSON
```json
{
  "state": "ARMED",
  "motion": false
}
```

#### `POST /arm`
Arm the security system
- **Authentication**: Required
- **Response**: 303 redirect to `/`

#### `POST /disarm`
Disarm the security system and stop alarm
- **Authentication**: Required
- **Response**: 303 redirect to `/`

### Example API Usage

Using `curl`:

```bash
# Check status
curl http://192.168.1.150/status --cookie "session=YOUR_SESSION_TOKEN"

# Arm system
curl -X POST http://192.168.1.150/arm --cookie "session=YOUR_SESSION_TOKEN"

# Disarm system
curl -X POST http://192.168.1.150/disarm --cookie "session=YOUR_SESSION_TOKEN"
```

## 🔧 Troubleshooting

### WiFi Connection Issues

**Problem**: Arduino fails to connect to WiFi

**Solutions**:
- Verify SSID and password are correct
- Ensure 2.4GHz WiFi (R4 WiFi doesn't support 5GHz)
- Check router is powered on and in range
- Try moving Arduino closer to router
- Check for special characters in WiFi credentials

### Motion Sensor Issues

**Problem**: No alarm when motion occurs

**Solutions for Ultrasonic Sensor**:
- Verify system is ARMED (status LED should be on)
- Check ultrasonic connections (TRIG to D3, ECHO to D4)
- Ensure sensor is pointed toward detection area
- Check Serial Monitor for baseline distance when arming
- Adjust `DETECTION_DISTANCE` in code (default 150cm / 5 feet)
- Adjust `DISTANCE_CHANGE_THRESHOLD` in code (default 30cm)
- Verify 5V power to ultrasonic sensor
- Ensure nothing is blocking the sensor

### Too Many False Alarms

**Problem**: System triggers when no one is there

**Solutions**:
- Increase `DISTANCE_CHANGE_THRESHOLD` (e.g., from 30cm to 50cm)
- Decrease `DETECTION_DISTANCE` to reduce range
- Point sensor away from moving objects (curtains, fans, pets)
- Check for obstacles that might sway or move
- Avoid pointing at windows or doors that may shift
- Ensure sensor is mounted securely (vibrations can cause false triggers)

### Web Interface Not Loading

**Problem**: Cannot access web interface

**Solutions**:
- Verify WiFi connection (check Serial Monitor)
- Confirm IP address from Serial Monitor
- Try accessing from same network as Arduino
- Clear browser cache
- Try different browser
- Check firewall settings

### LEDs Not Working

**Problem**: LEDs don't illuminate

**Solutions**:
- Check LED polarity (longer leg = positive)
- Verify resistor values (220Ω recommended)
- Test LEDs with multimeter
- Check pin connections (D8, D9)
- Verify ground connection

### Buzzer Not Sounding

**Problem**: No alarm sound during ALARM state

**Solutions**:
- Verify active buzzer (not passive)
- Check buzzer polarity
- Test buzzer with direct 5V connection
- Verify D10 connection
- Check if buzzer is damaged

### Session Expires Too Quickly

**Problem**: Need to re-login frequently

**Solution**: Increase `SESSION_TIMEOUT` value
```cpp
const unsigned long SESSION_TIMEOUT = 3600000;  // 60 minutes
```

## ⚠️ Safety Considerations

### Electrical Safety
- Operate only with low-voltage DC power (5V USB)
- Do not connect to mains voltage
- Verify all connections before powering on
- Use appropriate current-limiting resistors for LEDs
- Ensure proper ventilation if using enclosure

### GPIO Protection
- Do not exceed 8mA per GPIO pin (use resistors)
- Maximum 200mA total for all GPIO pins combined
- PIR sensor current should not exceed Arduino's limits

### Security Notes
- This is a demonstration/educational project
- **Not intended for critical security applications**
- Change default PIN immediately
- Use strong WiFi passwords
- Consider HTTPS for production use
- No warranty for security effectiveness

### Privacy
- PIR sensors detect motion only (no cameras)
- No data logging or external transmission
- All processing done locally on Arduino

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. **Open** a Pull Request

### Ideas for Contributions
- Add MQTT support for IoT integration
- Implement multiple user PINs
- Add email/SMS notifications
- Create mobile app interface
- Add event logging to SD card
- Implement zone-based detection
- Add camera integration
- Create Alexa/Google Home integration

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## 👤 Author

**Dante D'Abramo**

- Course: Engineering / Embedded Systems
- Date: January 30, 2026
- Project: Arduino R4 WiFi–Based Security System

## 📚 References

- [Arduino R4 WiFi Documentation](https://docs.arduino.cc/hardware/uno-r4-wifi/)
- [PIR Motion Sensor Guide](https://lastminuteengineers.com/pir-sensor-arduino-tutorial/)
- [WiFiS3 Library Reference](https://www.arduino.cc/reference/en/libraries/wifis3/)

## 🙏 Acknowledgments

- Arduino community for excellent documentation
- Engineering course instructors
- Open-source hardware community

## 📞 Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Check existing documentation
- Review troubleshooting section

---

**⚡ Built with Arduino R4 WiFi | 🔒 Secure by Design | 🌐 IoT Ready**

*Last Updated: Febuary 1st, 2026*
