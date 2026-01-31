# Installation Guide

Complete step-by-step installation instructions for the Arduino R4 WiFi Security System.

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Hardware Assembly](#hardware-assembly)
- [Software Installation](#software-installation)
- [Initial Configuration](#initial-configuration)
- [First Run](#first-run)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)

## ✅ Prerequisites

### Required Hardware

| Item | Specification | Where to Buy |
|------|--------------|--------------|
| Arduino UNO R4 WiFi | Official board | Arduino.cc, Amazon, DigiKey |
| PIR Motion Sensor | HC-SR501 (5V) | Amazon, AliExpress, SparkFun |
| LED 3mm | Any color, recommend green | Any electronics supplier |
| LED 5mm | Red recommended | Any electronics supplier |
| Active Buzzer | 5V active type | Amazon, AliExpress |
| Resistors 220Ω | 1/4W, 5% tolerance | Any electronics supplier |
| Jumper Wires | Male-to-male, assorted | Any electronics supplier |
| USB-C Cable | Data cable (not charge-only) | Any electronics supplier |
| Breadboard | 830 tie-points (optional) | Any electronics supplier |

**Total Estimated Cost**: $45-75 USD (including Arduino)

### Required Software

- **Arduino IDE** (version 2.0 or higher)
  - Download: https://www.arduino.cc/en/software
  - Supported: Windows, macOS, Linux
  
- **Web Browser** (any modern browser)
  - Chrome, Firefox, Safari, Edge

### Network Requirements

- **WiFi Network** (2.4GHz required)
  - WPA2 or WPA3 encryption
  - SSID and password available
  - Internet not required (local network only)

### Knowledge Requirements

- Basic electronics (connecting wires)
- Basic Arduino IDE usage
- Access to computer with USB port

## 🔧 Hardware Assembly

### Step 1: Identify Components

Lay out all components and verify you have everything:

```
✓ Arduino R4 WiFi
✓ PIR Sensor (has 3 pins: VCC, OUT, GND)
✓ LED 3mm (longer leg is +, shorter is -)
✓ LED 5mm (longer leg is +, shorter is -)
✓ Buzzer (marked + and -, or red/black wires)
✓ 2x 220Ω Resistors (color bands: red-red-brown)
✓ Jumper wires (at least 11 wires)
```

### Step 2: Prepare Breadboard (Optional but Recommended)

If using a breadboard:
1. Place Arduino alongside breadboard
2. Keep power rails accessible
3. Organize components for clean layout

### Step 3: Connect Power Rails

**⚠️ DO NOT PLUG IN USB YET**

1. Connect Arduino **5V** to breadboard power rail (+)
2. Connect Arduino **GND** to breadboard ground rail (-)

### Step 4: Connect PIR Sensor

PIR sensors typically have 3 pins (some have adjustment pots on back):

```
PIR Sensor          Arduino R4 WiFi
----------          ---------------
VCC (or +)    -->   5V
GND (or -)    -->   GND
OUT           -->   D2
```

**Tips:**
- PIR sensor dome should face outward
- Keep away from heat sources
- Adjustment pots (if present): Set both to middle position initially

### Step 5: Connect Status LED (3mm)

```
Status LED Circuit:
Arduino D8 --> 220Ω Resistor --> LED Anode (+, longer leg)
LED Cathode (-, shorter leg) --> GND
```

**Detailed Steps:**
1. Insert LED into breadboard
2. Connect 220Ω resistor to LED's anode (long leg)
3. Connect resistor's other end to Arduino pin D8
4. Connect LED's cathode (short leg) to GND

### Step 6: Connect Alarm LED (5mm)

```
Alarm LED Circuit:
Arduino D9 --> 220Ω Resistor --> LED Anode (+, longer leg)
LED Cathode (-, shorter leg) --> GND
```

**Detailed Steps:**
1. Insert LED into breadboard
2. Connect 220Ω resistor to LED's anode (long leg)
3. Connect resistor's other end to Arduino pin D9
4. Connect LED's cathode (short leg) to GND

### Step 7: Connect Buzzer

Active buzzers have polarity:

```
Buzzer            Arduino R4 WiFi
------            ---------------
+ (or red wire)   -->   D10
- (or black wire) -->   GND
```

**How to identify polarity:**
- Look for + and - markings on buzzer
- Red wire is usually positive
- Longer pin is usually positive
- Flat side often indicates negative

### Step 8: Verify All Connections

**Complete Connection Summary:**

| Component | Pin/Wire | Arduino Pin |
|-----------|----------|-------------|
| PIR VCC | Power | 5V |
| PIR GND | Ground | GND |
| PIR OUT | Signal | D2 |
| Status LED + | Via 220Ω | D8 |
| Status LED - | Direct | GND |
| Alarm LED + | Via 220Ω | D9 |
| Alarm LED - | Direct | GND |
| Buzzer + | Direct | D10 |
| Buzzer - | Direct | GND |

### Step 9: Visual Inspection

Before powering on:
- [ ] All connections are secure
- [ ] No wires touching each other
- [ ] LEDs inserted with correct polarity
- [ ] Resistors are in series with LEDs
- [ ] Buzzer polarity is correct
- [ ] No loose components
- [ ] Clean workspace (no metal objects near board)

### Optional: Take a Photo

Take a photo of your wiring for reference during troubleshooting.

## 💻 Software Installation

### Step 1: Install Arduino IDE

1. Download Arduino IDE from https://www.arduino.cc/en/software
2. Install for your operating system:
   - **Windows**: Run installer, follow prompts
   - **macOS**: Drag to Applications folder
   - **Linux**: Extract and run `install.sh`
3. Launch Arduino IDE

### Step 2: Install Board Support

1. Open Arduino IDE
2. Go to **Tools → Board → Boards Manager**
3. Search for "**UNO R4 WiFi**"
4. Click "**Arduino UNO R4 Boards**" by Arduino
5. Click **Install** (may take several minutes)
6. Wait for installation to complete
7. Close Boards Manager

### Step 3: Select Board and Port

1. Connect Arduino R4 WiFi to computer via USB-C
2. Wait for drivers to install (Windows may take a moment)
3. In Arduino IDE:
   - Go to **Tools → Board → Arduino UNO R4 Boards → Arduino UNO R4 WiFi**
   - Go to **Tools → Port** → Select your Arduino's port
     - Windows: Usually "COM3", "COM4", etc.
     - macOS: Usually "/dev/cu.usbmodem..."
     - Linux: Usually "/dev/ttyACM0" or "/dev/ttyUSB0"

**Troubleshooting port selection:**
- If no port appears, try different USB cable
- Check Device Manager (Windows) or System Information (macOS)
- Try different USB port on computer
- Restart Arduino IDE

### Step 4: Download Project Files

**Option A: Download ZIP**
1. Visit project GitHub page
2. Click green "**Code**" button
3. Click "**Download ZIP**"
4. Extract ZIP to your Arduino folder

**Option B: Git Clone**
```bash
cd ~/Arduino
git clone https://github.com/yourusername/arduino-security-system.git
```

### Step 5: Open Sketch

1. In Arduino IDE: **File → Open**
2. Navigate to downloaded folder
3. Select `security_system.ino`
4. Click **Open**

## ⚙️ Initial Configuration

### Step 1: Configure WiFi

Open `security_system.ino` and find these lines near the top:

```cpp
// ==================== CONFIGURATION ====================
// Update these with your WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

Replace with your actual WiFi information:

```cpp
const char* ssid = "MyHomeNetwork";     // Your WiFi name
const char* password = "MyWiFiPassword123"; // Your WiFi password
```

**Important Notes:**
- WiFi network MUST be 2.4GHz (not 5GHz)
- SSID and password are case-sensitive
- Use quotes around the text
- Special characters in password may need escaping

### Step 2: Configure PIN (Optional)

Default PIN is **1234**. To change:

```cpp
// Security PIN
const String SECURITY_PIN = "1234";  // Change to your desired PIN
```

Example:
```cpp
const String SECURITY_PIN = "9086";  // Your custom PIN
```

**PIN Requirements:**
- Must be exactly 4 digits
- Numbers only (0-9)
- Avoid obvious PINs (1234, 0000, etc.)

### Step 3: Adjust Settings (Optional)

**Session Timeout** (default 30 minutes):
```cpp
const unsigned long SESSION_TIMEOUT = 1800000;  // milliseconds
```

To change to 60 minutes:
```cpp
const unsigned long SESSION_TIMEOUT = 3600000;  // 60 minutes
```

**Pin Assignments** (only change if you need different pins):
```cpp
const int PIR_PIN = 2;
const int STATUS_LED_PIN = 8;
const int ALARM_LED_PIN = 9;
const int BUZZER_PIN = 10;
```

### Step 4: Save Changes

1. Press **Ctrl+S** (or Cmd+S on macOS)
2. Or: **File → Save**

## 🚀 First Run

### Step 1: Compile and Upload

1. Click the **Verify** button (checkmark ✓) to compile
   - Wait for "Done compiling" message
   - Check for any errors
2. If successful, click **Upload** button (→)
   - Arduino will reset and start uploading
   - TX/RX LEDs on Arduino will blink rapidly
   - Wait for "Done uploading" message

**Common Upload Errors:**
- "Port not found": Check connection and reselect port
- "Access denied": Close Serial Monitor if open
- "Wrong bootloader": Ensure "Arduino UNO R4 WiFi" is selected

### Step 2: Open Serial Monitor

1. Click **Serial Monitor** button (magnifying glass icon)
2. Set baud rate to **115200** (bottom right dropdown)
3. Watch for system startup messages

Expected output:
```
=== Arduino R4 WiFi Security System ===
Hardware initialized - System DISARMED
Connecting to WiFi: YourNetworkName
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

### Step 3: Note the IP Address

**Write down the IP address shown in Serial Monitor:**
```
IP Address: 192.168.1.150  <-- This is YOUR system's address
```

You'll need this to access the web interface.

### Step 4: Test Hardware

With Serial Monitor open:

1. **Status LED Test:**
   - Arduino should boot with all LEDs off
   - System starts in DISARMED state

2. **PIR Sensor Stabilization:**
   - Wait 60 seconds for PIR sensor to stabilize
   - During this time, avoid triggering sensor

## ✅ Verification

### Test 1: Web Interface Access

1. Open web browser on same network
2. Navigate to IP address: `http://192.168.1.150` (use your IP)
3. **Expected Result:** Login page appears with PIN entry
4. **If fails:** See troubleshooting section

### Test 2: Login

1. Enter PIN: **0421** (or your custom PIN)
2. Click **UNLOCK**
3. **Expected Result:** Control dashboard appears
4. **If fails:** Check PIN is correct, check Serial Monitor for errors

### Test 3: System Arm

1. Click **ARM SYSTEM** button
2. **Expected Results:**
   - Status LED (3mm) turns ON
   - Web page shows "ARMED" in yellow
   - Serial Monitor shows "System ARMED via web interface"

### Test 4: Motion Detection

1. Ensure system is ARMED (Status LED on)
2. Wait 10 seconds (ensure PIR stabilized)
3. Wave hand in front of PIR sensor
4. **Expected Results:**
   - Alarm LED (5mm) turns ON
   - Buzzer starts beeping (500ms on/off pattern)
   - Web page shows "⚠️ ALARM TRIGGERED ⚠️" in red
   - Serial Monitor shows "MOTION DETECTED! Alarm triggered."

### Test 5: System Disarm

1. While alarm is active, click **DISARM SYSTEM** button
2. **Expected Results:**
   - All LEDs turn OFF
   - Buzzer stops
   - Web page shows "DISARMED" in green
   - Serial Monitor shows "System DISARMED via web interface"

### Test 6: Session Timeout

1. Leave web page open without interaction
2. Wait 30 minutes (or your configured timeout)
3. Try to ARM or DISARM
4. **Expected Result:** Redirected to login page

## 🔧 Troubleshooting

### WiFi Connection Issues

**Problem:** "WiFi connection failed!" in Serial Monitor

**Solutions:**
1. Verify SSID and password are correct
2. Ensure WiFi is 2.4GHz (not 5GHz)
3. Move Arduino closer to router
4. Check router is powered on
5. Try restarting router
6. Check for special characters in password

**Test WiFi separately:**
```cpp
// Temporarily add this to setup() for testing:
Serial.println(ssid);
Serial.println(password);
```

### Can't Access Web Interface

**Problem:** Browser shows "Can't connect" or timeout

**Solutions:**
1. Verify Arduino is connected to WiFi (check Serial Monitor)
2. Confirm IP address is correct
3. Ensure computer is on same network
4. Try `ping 192.168.1.150` in command prompt/terminal
5. Disable firewall temporarily to test
6. Clear browser cache
7. Try different browser

### PIR Sensor Not Working

**Problem:** No alarm when motion occurs

**Solutions:**
1. Verify system is ARMED (Status LED should be on)
2. Check PIR sensor connections (especially signal wire to D2)
3. Ensure PIR sensor has 5V power
4. Wait 60 seconds for PIR stabilization
5. Adjust PIR sensitivity pot (small screwdriver)
6. Test PIR with Serial Monitor:
   ```cpp
   // Add to loop() temporarily:
   Serial.println(digitalRead(PIR_PIN));
   ```
7. Try different PIR sensor

### LEDs Not Working

**Problem:** LEDs don't light up

**Solutions:**
1. Check LED polarity (long leg = anode = +)
2. Verify resistors are connected
3. Test LED directly:
   - Connect long leg to 5V via 220Ω resistor
   - Connect short leg to GND
   - Should light up
4. Check wire connections to D8 and D9
5. Try different LED

### Buzzer Not Working

**Problem:** No sound from buzzer

**Solutions:**
1. Verify active buzzer (not passive)
2. Check buzzer polarity
3. Test buzzer directly:
   - Connect + to 5V
   - Connect - to GND
   - Should make sound
4. Check wire connection to D10
5. Try different buzzer
6. Verify buzzer isn't loose

### Upload Fails

**Problem:** Can't upload sketch to Arduino

**Solutions:**
1. Close Serial Monitor (it locks the port)
2. Check USB cable (try different one)
3. Verify correct board selected
4. Verify correct port selected
5. Press reset button on Arduino, then upload immediately
6. Restart Arduino IDE
7. Restart computer

### Login Not Working

**Problem:** PIN rejected or login page issues

**Solutions:**
1. Verify PIN is exactly 4 digits
2. Check for typos in PIN definition
3. Clear browser cookies
4. Try private/incognito browser window
5. Check Serial Monitor for "Login attempt with PIN: XXXX"
6. Verify POST data is being received

## 📞 Getting Help

If you're still having issues:

1. **Check Documentation:**
   - README.md
   - This INSTALL.md
   - SECURITY.md

2. **Serial Monitor:**
   - Most issues show error messages here
   - Include Serial output when asking for help

3. **Community Support:**
   - GitHub Issues (for bugs)
   - GitHub Discussions (for questions)
   - Arduino Forums

4. **Include This Info:**
   - Arduino IDE version
   - Operating system
   - Serial Monitor output
   - Photo of wiring
   - Steps to reproduce problem

## 🎓 Next Steps

After successful installation:

1. **Read the full README.md** for detailed usage
2. **Review SECURITY.md** for security best practices
3. **Experiment with the system** in a safe environment
4. **Consider enhancements** from CONTRIBUTING.md
5. **Share your project** with the community!

## 📚 Additional Resources

- [Arduino R4 WiFi Documentation](https://docs.arduino.cc/hardware/uno-r4-wifi/)
- [PIR Sensor Tutorial](https://lastminuteengineers.com/pir-sensor-arduino-tutorial/)
- [WiFiS3 Library Reference](https://www.arduino.cc/reference/en/libraries/wifis3/)
- [Project GitHub Repository](https://github.com/yourusername/arduino-security-system)

---

**Congratulations on completing the installation!** 🎉

*Last Updated: January 30, 2026*

