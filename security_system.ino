/*
 * Arduino R4 WiFi Security System
 * PIR Motion Detection with Web-Controlled Arm/Disarm
 * 
 * Author: Dante D'Abramo
 * Date: 2026-01-30
 * 
 * Hardware Connections:
 * - PIR Sensor Output: D2 - Not used
 * - ECHO Pin: 4 * - TRIG Pin: 3
 * - Status LED (3mm): D8 * - Alarm LED (5mm): D9
 * - Piezo Buzzer: D10
 * - Doorbell Button: D11
 * - Doorbell Buzzer: D12
 */

#include <WiFiS3.h>

// ==================== CONFIGURATION ====================
// Update these with your WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Security PIN
const String SECURITY_PIN = "0421";

// Pin Definitions
// const int PIR_PIN = 2;  // Not used - ultrasonic only
const int STATUS_LED_PIN = 8;
const int ALARM_LED_PIN = 9;
const int BUZZER_PIN = 10;

// Doorbell Pins
const int DOORBELL_BUTTON_PIN = 11;  // Button input
const int DOORBELL_BUZZER_PIN = 12;  // Passive buzzer for doorbell

// Ultrasonic Sensor Pins (HC-SR04)
const int TRIG_PIN = 3;  // Trigger pin
const int ECHO_PIN = 4;  // Echo pin

// Ultrasonic Detection Settings
const int DETECTION_DISTANCE = 150;  // Trigger if something within 150cm (1.5m)
const int DISTANCE_CHANGE_THRESHOLD = 30; // 30cm movement threshold
int baselineDistance = 0;  // Store baseline distance when armed
unsigned long lastDistanceCheck = 0;
const unsigned long DISTANCE_CHECK_INTERVAL = 500; // Check every 500ms

// ==================== SYSTEM STATES ====================
enum SystemState {
  DISARMED,
  ARMED,
  ALARM_TRIGGERED
};

SystemState currentState = DISARMED;

// ==================== GLOBALS ====================
WiFiServer server(80);
bool motionDetected = false;
unsigned long lastMotionTime = 0;

// Doorbell globals
bool doorbellPressed = false;
unsigned long doorbellPressTime = 0;
const unsigned long DOORBELL_MESSAGE_DURATION = 120000; // 2 minutes in milliseconds
bool doorbellButtonState = HIGH;  // Debounced button state (HIGH = not pressed with INPUT_PULLUP)
bool lastButtonState = HIGH;  // Raw reading for debounce timing (HIGH = not pressed with INPUT_PULLUP)
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce

// Session management
String authenticatedSession = "";
unsigned long sessionStartTime = 0;
const unsigned long SESSION_TIMEOUT = 1800000; // 30 minutes in milliseconds

// ==================== SETUP ====================
void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait up to 3 seconds for serial
  
  Serial.println("\n=== Arduino R4 WiFi Security System ===");
  
  // Initialize GPIO pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ALARM_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Doorbell pins
  pinMode(DOORBELL_BUTTON_PIN, INPUT_PULLUP);  // Internal pull-up: reads HIGH when open, LOW when button connects D11 to GND
  pinMode(DOORBELL_BUZZER_PIN, OUTPUT);
  
  // Ensure all outputs are off at startup
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(ALARM_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(DOORBELL_BUZZER_PIN, LOW);
  
  Serial.println("Hardware initialized - System DISARMED");
  
  // Connect to WiFi
  connectToWiFi();
  
  // Start web server
  server.begin();
  Serial.println("Web server started");
  printSystemInfo();
}

// ==================== MAIN LOOP ====================
void loop() {
  // Handle web client requests
  handleWebClient();
  
  // Process system logic based on current state
  switch (currentState) {
    case DISARMED:
      handleDisarmedState();
      break;
      
    case ARMED:
      handleArmedState();
      break;
      
    case ALARM_TRIGGERED:
      handleAlarmState();
      break;
  }
  
  // Check doorbell button (works in all states)
  checkDoorbellButton();
  
  // Clear doorbell message after 2 minutes
  if (doorbellPressed && (millis() - doorbellPressTime > DOORBELL_MESSAGE_DURATION)) {
    doorbellPressed = false;
    Serial.println("Doorbell message cleared");
  }
  
  delay(100); // Small delay to prevent CPU overload
}

// ==================== DOORBELL FUNCTIONS ====================
void checkDoorbellButton() {
  int reading = digitalRead(DOORBELL_BUTTON_PIN);
  
  // Debounce the button: reset timer whenever the raw reading changes
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // Reading has been stable long enough - update debounced state
    if (reading != doorbellButtonState) {
      doorbellButtonState = reading;
      
      // Trigger on falling edge (button pressed pulls to GND with INPUT_PULLUP)
      if (doorbellButtonState == LOW) {
        Serial.println("DOORBELL: Button pressed!");
        playDoorbellSound();
        doorbellPressed = true;
        doorbellPressTime = millis();
      }
    }
  }
  
  lastButtonState = reading;
}

void playDoorbellSound() {
  // Ding-Dong doorbell melody
  // "Ding" - higher note
  tone(DOORBELL_BUZZER_PIN, 1000, 200);  // 1000 Hz for 200ms
  delay(200);
  noTone(DOORBELL_BUZZER_PIN);
  delay(50);
  
  // "Dong" - lower note
  tone(DOORBELL_BUZZER_PIN, 800, 300);   // 800 Hz for 300ms
  delay(300);
  noTone(DOORBELL_BUZZER_PIN);
  
  Serial.println("DOORBELL: Ding-dong sound played");
}

// ==================== ULTRASONIC SENSOR FUNCTIONS ====================
int getDistance() {
  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read echo pulse
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  
  // Calculate distance in cm (duration/2 because sound travels there and back)
  int distance = duration * 0.034 / 2;
  
  // Return 0 if no echo or out of range
  if (distance == 0 || distance > 400) {
    return 0;
  }
  
  return distance;
}

bool checkUltrasonicMotion() {
  unsigned long currentTime = millis();
  
  // Only check at intervals to avoid constant pinging
  if (currentTime - lastDistanceCheck < DISTANCE_CHECK_INTERVAL) {
    return false;
  }
  
  lastDistanceCheck = currentTime;
  int currentDistance = getDistance();
  
  // Skip if invalid reading
  if (currentDistance == 0) {
    return false;
  }
  
  // Check if something is within detection range
  if (currentDistance < DETECTION_DISTANCE) {
    // Check if distance changed significantly from baseline
    int distanceChange = abs(currentDistance - baselineDistance);
    
    if (distanceChange > DISTANCE_CHANGE_THRESHOLD) {
      Serial.print("ULTRASONIC: Movement detected! Distance change: ");
      Serial.print(distanceChange);
      Serial.print("cm (baseline: ");
      Serial.print(baselineDistance);
      Serial.print("cm, current: ");
      Serial.print(currentDistance);
      Serial.println("cm)");
      return true;
    }
  }
  
  return false;
}

// ==================== STATE HANDLERS ====================
void handleDisarmedState() {
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(ALARM_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void handleArmedState() {
  digitalWrite(STATUS_LED_PIN, HIGH);
  digitalWrite(ALARM_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  unsigned long currentTime = millis();
  
  // Check ultrasonic sensor for motion
  if (checkUltrasonicMotion()) {
    motionDetected = true;
    lastMotionTime = currentTime;
    currentState = ALARM_TRIGGERED;
    Serial.println("========================================");
    Serial.println("ALARM TRIGGERED - MOTION DETECTED!");
    Serial.println("========================================");
  }
}

void handleAlarmState() {
  digitalWrite(STATUS_LED_PIN, HIGH);
  digitalWrite(ALARM_LED_PIN, HIGH);
  
  // Buzzer pattern - beep every 500ms
  unsigned long currentTime = millis();
  if ((currentTime / 500) % 2 == 0) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  // Alarm continues until system is disarmed via web interface
  // No automatic timeout
}

// ==================== WIFI FUNCTIONS ====================
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    
    // Wait for valid IP address (not 0.0.0.0)
    attempts = 0;
    while (WiFi.localIP().toString() == "0.0.0.0" && attempts < 20) {
      delay(500);
      Serial.print("Waiting for IP");
      Serial.print(".");
      attempts++;
    }
    Serial.println();
    
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    if (WiFi.localIP().toString() == "0.0.0.0") {
      Serial.println("WARNING: Failed to obtain IP address from DHCP");
      Serial.println("Check router DHCP settings");
    }
  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.println("System will operate without network features.");
  }
}

void printSystemInfo() {
  Serial.println("\n=== System Information ===");
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Access web interface at: http://");
    Serial.println(WiFi.localIP());
  }
  Serial.print("System State: ");
  Serial.println(getStateString(currentState));
  Serial.println("========================\n");
}

// ==================== WEB SERVER ====================
void handleWebClient() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client connected");
    String currentLine = "";
    String requestType = "";
    String requestPath = "";
    String cookieHeader = "";
    String postData = "";
    int contentLength = 0;
    bool isPostRequest = false;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Read POST data if present
            if (isPostRequest && contentLength > 0) {
              for (int i = 0; i < contentLength && client.available(); i++) {
                postData += (char)client.read();
              }
            }
            
            // End of HTTP request - process and respond
            handleHTTPRequest(client, requestType, requestPath, cookieHeader, postData);
            break;
          } else {
            // Parse request line
            if (currentLine.startsWith("GET") || currentLine.startsWith("POST")) {
              int firstSpace = currentLine.indexOf(' ');
              int secondSpace = currentLine.indexOf(' ', firstSpace + 1);
              requestType = currentLine.substring(0, firstSpace);
              requestPath = currentLine.substring(firstSpace + 1, secondSpace);
              isPostRequest = (requestType == "POST");
            }
            // Parse Cookie header
            else if (currentLine.startsWith("Cookie:")) {
              cookieHeader = currentLine.substring(8);
              cookieHeader.trim();
            }
            // Parse Content-Length
            else if (currentLine.startsWith("Content-Length:")) {
              contentLength = currentLine.substring(16).toInt();
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    delay(10);
    client.stop();
    Serial.println("Client disconnected");
  }
}

void handleHTTPRequest(WiFiClient& client, String method, String path, String cookie, String postData) {
  Serial.print("Request: ");
  Serial.print(method);
  Serial.print(" ");
  Serial.println(path);
  
  // Check authentication
  bool isAuthenticated = checkAuthentication(cookie);
  
  // Route handling
  if (path == "/" || path.startsWith("/?")) {
    if (isAuthenticated) {
      serveWebPage(client);
    } else {
      serveLoginPage(client, "");
    }
  } else if (path == "/login" && method == "POST") {
    handleLogin(client, postData);
  } else if (path == "/logout") {
    handleLogout(client);
  } else if (path == "/status") {
    if (isAuthenticated) {
      serveStatus(client);
    } else {
      serve401(client);
    }
  } else if (path == "/arm" && method == "POST") {
    if (isAuthenticated) {
      armSystem(client);
    } else {
      serve401(client);
    }
  } else if (path == "/disarm" && method == "POST") {
    if (isAuthenticated) {
      disarmSystem(client);
    } else {
      serve401(client);
    }
  } else {
    serve404(client);
  }
}

// ==================== AUTHENTICATION ====================
bool checkAuthentication(String cookie) {
  // Check if session is valid and not expired
  if (authenticatedSession.length() > 0 && cookie.indexOf("session=" + authenticatedSession) >= 0) {
    if (millis() - sessionStartTime < SESSION_TIMEOUT) {
      return true;
    } else {
      // Session expired
      authenticatedSession = "";
      Serial.println("Session expired");
      return false;
    }
  }
  return false;
}

String generateSessionToken() {
  // Simple session token based on millis and random
  String token = String(millis()) + String(random(10000, 99999));
  return token;
}

void serveLoginPage(WiFiClient& client, String errorMsg) {
  String html = "HTTP/1.1 200 OK\r\n";
  html += "Content-Type: text/html\r\n";
  html += "Connection: close\r\n\r\n";
  html += "<!DOCTYPE HTML><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<link href='https://fonts.googleapis.com/css2?family=DM+Sans:wght@400;500;700&display=swap' rel='stylesheet'>";
  html += "<style>";
  html += "*{margin:0;padding:0;box-sizing:border-box;}";
  html += "body{font-family:'DM Sans',sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px;}";
  html += ".container{background:rgba(255,255,255,0.95);backdrop-filter:blur(20px);padding:50px 40px;border-radius:24px;box-shadow:0 20px 60px rgba(0,0,0,0.3);max-width:420px;width:100%;animation:slideUp 0.6s ease;}";
  html += "@keyframes slideUp{from{opacity:0;transform:translateY(30px);}to{opacity:1;transform:translateY(0);}}";
  html += ".logo{width:80px;height:80px;background:linear-gradient(135deg,#667eea,#764ba2);border-radius:20px;display:flex;align-items:center;justify-content:center;margin:0 auto 30px;box-shadow:0 10px 30px rgba(102,126,234,0.4);}";
  html += ".logo svg{width:45px;height:45px;}";
  html += "h1{font-size:28px;font-weight:700;color:#1a202c;text-align:center;margin-bottom:10px;}";
  html += ".subtitle{font-size:15px;color:#64748b;text-align:center;margin-bottom:40px;font-weight:400;}";
  html += "form{display:flex;flex-direction:column;gap:20px;}";
  html += ".input-group{position:relative;}";
  html += "input{font-family:'DM Sans',sans-serif;font-size:16px;padding:16px 20px;width:100%;border:2px solid #e2e8f0;border-radius:12px;background:#f8fafc;transition:all 0.3s ease;font-weight:500;}";
  html += "input:focus{outline:none;border-color:#667eea;background:#fff;box-shadow:0 0 0 4px rgba(102,126,234,0.1);}";
  html += "input::placeholder{color:#94a3b8;font-weight:400;}";
  html += "button{font-family:'DM Sans',sans-serif;font-size:16px;font-weight:600;padding:16px;border:none;border-radius:12px;background:linear-gradient(135deg,#667eea,#764ba2);color:white;cursor:pointer;transition:all 0.3s ease;box-shadow:0 4px 15px rgba(102,126,234,0.4);}";
  html += "button:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(102,126,234,0.5);}";
  html += "button:active{transform:translateY(0);}";
  html += ".error{background:#fee;border:2px solid #fcc;color:#c33;padding:14px 18px;border-radius:12px;margin-bottom:20px;font-size:14px;font-weight:500;animation:shake 0.4s ease;}";
  html += "@keyframes shake{0%,100%{transform:translateX(0);}25%{transform:translateX(-10px);}75%{transform:translateX(10px);}}";
  html += ".footer{text-align:center;margin-top:30px;font-size:13px;color:#94a3b8;}";
  html += "@media(max-width:480px){.container{padding:40px 30px;}h1{font-size:24px;}}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<div class='logo'><svg viewBox='0 0 24 24' fill='none' stroke='white' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'><rect x='3' y='11' width='18' height='11' rx='2' ry='2'/><path d='M7 11V7a5 5 0 0 1 10 0v4'/></svg></div>";
  html += "<h1>Security System</h1>";
  html += "<div class='subtitle'>Enter your PIN to continue</div>";
  
  if (errorMsg.length() > 0) {
    html += "<div class='error'>" + errorMsg + "</div>";
  }
  
  html += "<form method='POST' action='/login'>";
  html += "<div class='input-group'><input type='password' name='pin' maxlength='4' pattern='[0-9]{4}' placeholder='Enter 4-digit PIN' required autofocus></div>";
  html += "<button type='submit'>Unlock System</button>";
  html += "</form>";
  html += "<div class='footer'>Arduino R4 WiFi Security v1.0</div>";
  html += "</div></body></html>";
  
  client.print(html);
}

void handleLogin(WiFiClient& client, String postData) {
  // Parse PIN from POST data
  String pin = "";
  int pinIndex = postData.indexOf("pin=");
  if (pinIndex >= 0) {
    pin = postData.substring(pinIndex + 4);
    int ampIndex = pin.indexOf('&');
    if (ampIndex >= 0) {
      pin = pin.substring(0, ampIndex);
    }
  }
  
  Serial.print("Login attempt with PIN: ");
  Serial.println(pin);
  
  if (pin == SECURITY_PIN) {
    // Successful login
    authenticatedSession = generateSessionToken();
    sessionStartTime = millis();
    Serial.println("Login successful - session created");
    
    String response = "HTTP/1.1 303 See Other\r\n";
    response += "Location: /\r\n";
    response += "Set-Cookie: session=" + authenticatedSession + "; Path=/; Max-Age=1800\r\n";
    response += "Connection: close\r\n\r\n";
    
    client.print(response);
  } else {
    // Failed login
    Serial.println("Login failed - incorrect PIN");
    serveLoginPage(client, "Incorrect PIN. Try again.");
  }
}

void handleLogout(WiFiClient& client) {
  authenticatedSession = "";
  Serial.println("User logged out");
  
  String response = "HTTP/1.1 303 See Other\r\n";
  response += "Location: /\r\n";
  response += "Set-Cookie: session=; Path=/; Max-Age=0\r\n";
  response += "Connection: close\r\n\r\n";
  
  client.print(response);
}

void serve401(WiFiClient& client) {
  String response = "HTTP/1.1 401 Unauthorized\r\n";
  response += "Content-Type: text/html\r\n";
  response += "Connection: close\r\n\r\n";
  response += "<html><body><h1>401 - Unauthorized</h1><p><a href='/'>Login</a></p></body></html>";
  
  client.print(response);
}

void serveWebPage(WiFiClient& client) {
  String html = "HTTP/1.1 200 OK\r\n";
  html += "Content-Type: text/html\r\n";
  html += "Connection: close\r\n\r\n";
  html += "<!DOCTYPE HTML><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<link href='https://fonts.googleapis.com/css2?family=DM+Sans:wght@400;500;700&display=swap' rel='stylesheet'>";
  html += "<style>";
  html += "*{margin:0;padding:0;box-sizing:border-box;}";
  html += "body{font-family:'DM Sans',sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px;}";
  html += ".container{max-width:800px;margin:0 auto;}";
  html += ".header{text-align:center;color:white;margin-bottom:30px;animation:fadeIn 0.6s ease;}";
  html += "@keyframes fadeIn{from{opacity:0;transform:translateY(-20px);}to{opacity:1;transform:translateY(0);}}";
  html += ".header h1{font-size:32px;font-weight:700;margin-bottom:8px;text-shadow:0 2px 10px rgba(0,0,0,0.2);}";
  html += ".header p{font-size:16px;font-weight:400;opacity:0.9;}";
  html += ".card{background:rgba(255,255,255,0.95);backdrop-filter:blur(20px);border-radius:24px;padding:40px;box-shadow:0 20px 60px rgba(0,0,0,0.3);margin-bottom:20px;animation:slideUp 0.6s ease;}";
  html += "@keyframes slideUp{from{opacity:0;transform:translateY(30px);}to{opacity:1;transform:translateY(0);}}";
  html += ".status-badge{display:inline-block;padding:20px 40px;border-radius:16px;font-size:24px;font-weight:700;margin:20px 0;text-align:center;width:100%;transition:all 0.4s ease;}";
  html += ".status-disarmed{background:linear-gradient(135deg,#10b981,#059669);color:white;box-shadow:0 8px 24px rgba(16,185,129,0.4);}";
  html += ".status-armed{background:linear-gradient(135deg,#f59e0b,#d97706);color:white;box-shadow:0 8px 24px rgba(245,158,11,0.4);}";
  html += ".status-alarm{background:linear-gradient(135deg,#ef4444,#dc2626);color:white;box-shadow:0 8px 24px rgba(239,68,68,0.4);animation:pulse 1.5s infinite;}";
  html += "@keyframes pulse{0%,100%{transform:scale(1);}50%{transform:scale(1.02);}}";
  html += ".status-message{text-align:center;color:#64748b;margin-top:10px;font-size:14px;transition:all 0.3s ease;}";
  html += ".controls{display:grid;grid-template-columns:1fr 1fr;gap:15px;margin:30px 0;}";
  html += ".btn{font-family:'DM Sans',sans-serif;padding:18px 24px;border:none;border-radius:14px;font-size:16px;font-weight:600;cursor:pointer;transition:all 0.3s ease;text-align:center;position:relative;overflow:hidden;}";
  html += ".btn:disabled{opacity:0.5;cursor:not-allowed;}";
  html += ".btn-arm{background:linear-gradient(135deg,#3b82f6,#2563eb);color:white;box-shadow:0 4px 15px rgba(59,130,246,0.4);}";
  html += ".btn-arm:hover:not(:disabled){transform:translateY(-2px);box-shadow:0 6px 20px rgba(59,130,246,0.5);}";
  html += ".btn-disarm{background:linear-gradient(135deg,#10b981,#059669);color:white;box-shadow:0 4px 15px rgba(16,185,129,0.4);}";
  html += ".btn-disarm:hover:not(:disabled){transform:translateY(-2px);box-shadow:0 6px 20px rgba(16,185,129,0.5);}";
  html += ".btn:active:not(:disabled){transform:translateY(0);}";
  html += ".btn.loading::after{content:'';position:absolute;top:50%;left:50%;width:20px;height:20px;margin:-10px 0 0 -10px;border:3px solid rgba(255,255,255,0.3);border-top-color:white;border-radius:50%;animation:spin 0.8s linear infinite;}";
  html += "@keyframes spin{to{transform:rotate(360deg);}}";
  html += ".info-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:15px;margin:25px 0;padding:25px;background:rgba(100,116,139,0.08);border-radius:16px;}";
  html += ".info-item{text-align:center;}";
  html += ".info-label{font-size:12px;color:#64748b;font-weight:500;text-transform:uppercase;letter-spacing:0.5px;margin-bottom:6px;}";
  html += ".info-value{font-size:18px;color:#1e293b;font-weight:700;}";
  html += ".logout-btn{width:100%;padding:14px;margin-top:15px;background:rgba(100,116,139,0.1);color:#64748b;border:2px solid rgba(100,116,139,0.2);border-radius:12px;font-size:14px;font-weight:600;cursor:pointer;transition:all 0.3s ease;}";
  html += ".logout-btn:hover{background:rgba(100,116,139,0.15);border-color:rgba(100,116,139,0.3);}";
  html += ".divider{height:2px;background:linear-gradient(90deg,transparent,rgba(100,116,139,0.2),transparent);margin:25px 0;}";
  html += ".update-indicator{display:inline-block;width:8px;height:8px;background:#10b981;border-radius:50%;margin-left:8px;animation:blink 2s infinite;}";
  html += "@keyframes blink{0%,100%{opacity:1;}50%{opacity:0.3;}}";
  html += ".doorbell-notification{background:linear-gradient(135deg,#8b5cf6,#7c3aed);color:white;padding:20px;border-radius:16px;margin-bottom:20px;text-align:center;font-size:18px;font-weight:600;box-shadow:0 8px 24px rgba(139,92,246,0.4);animation:slideDown 0.5s ease,pulse 2s infinite;}";
  html += "@keyframes slideDown{from{opacity:0;transform:translateY(-20px);}to{opacity:1;transform:translateY(0);}}";
  html += ".doorbell-icon{font-size:32px;margin-bottom:8px;}";
  html += "@media(max-width:640px){.controls{grid-template-columns:1fr;}.info-grid{grid-template-columns:1fr;}}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>Security System</h1>";
  html += "<p>Real-time monitoring and control<span class='update-indicator'></span></p>";
  html += "</div>";
  
  // Doorbell notification (shown via AJAX when doorbell pressed)
  html += "<div id='doorbellNotification' style='display:none;'></div>";
  
  html += "<div class='card'>";
  
  // Status badge - will be updated via AJAX
  html += "<div id='statusBadge' class='status-badge'></div>";
  html += "<p id='statusMessage' class='status-message'></p>";
  
  html += "<div class='divider'></div>";
  
  // Control buttons
  html += "<div class='controls'>";
  html += "<button class='btn btn-arm' id='armBtn' onclick='controlSystem(\"arm\")'>Arm System</button>";
  html += "<button class='btn btn-disarm' id='disarmBtn' onclick='controlSystem(\"disarm\")'>Disarm System</button>";
  html += "</div>";
  
  // System info grid
  html += "<div class='info-grid'>";
  html += "<div class='info-item'><div class='info-label'>Ultrasonic TRIG</div><div class='info-value'>Pin D3</div></div>";
  html += "<div class='info-item'><div class='info-label'>Ultrasonic ECHO</div><div class='info-value'>Pin D4</div></div>";
  html += "<div class='info-item'><div class='info-label'>Status LED</div><div class='info-value'>Pin D8</div></div>";
  html += "<div class='info-item'><div class='info-label'>Alarm LED</div><div class='info-value'>Pin D9</div></div>";
  html += "<div class='info-item'><div class='info-label'>Buzzer</div><div class='info-value'>Pin D10</div></div>";
  html += "<div class='info-item'><div class='info-label'>Detection</div><div class='info-value'>Distance</div></div>";
  html += "</div>";
  
  html += "<div class='divider'></div>";
  
  // Logout button
  html += "<form method='GET' action='/logout'>";
  html += "<button class='logout-btn' type='submit'>Logout</button>";
  html += "</form>";
  
  html += "</div></div>";
  
  // JavaScript for AJAX updates
  html += "<script>";
  html += "let isUpdating=false;";
  
  // Function to update status display
  html += "function updateStatus(){";
  html += "fetch('/status').then(r=>r.json()).then(data=>{";
  html += "const badge=document.getElementById('statusBadge');";
  html += "const msg=document.getElementById('statusMessage');";
  html += "const doorbell=document.getElementById('doorbellNotification');";
  html += "badge.className='status-badge';";
  html += "if(data.state==='DISARMED'){";
  html += "badge.className+=' status-disarmed';";
  html += "badge.textContent='SYSTEM DISARMED';";
  html += "msg.textContent='System is inactive and not monitoring';";
  html += "msg.style.color='#64748b';";
  html += "}else if(data.state==='ARMED'){";
  html += "badge.className+=' status-armed';";
  html += "badge.textContent='SYSTEM ARMED';";
  html += "msg.textContent='Monitoring for motion activity';";
  html += "msg.style.color='#64748b';";
  html += "}else if(data.state==='ALARM_TRIGGERED'){";
  html += "badge.className+=' status-alarm';";
  html += "badge.textContent='ALARM ACTIVE';";
  html += "msg.textContent='Motion detected! Disarm to stop alarm';";
  html += "msg.style.color='#dc2626';";
  html += "msg.style.fontWeight='600';";
  html += "}";
  html += "if(data.doorbell){";
  html += "doorbell.innerHTML=\"<div class='doorbell-notification'><div class='doorbell-icon'>&#128276;</div><div>Someone is at your door!</div></div>\";";
  html += "doorbell.style.display='block';";
  html += "}else{";
  html += "doorbell.style.display='none';";
  html += "}";
  html += "}).catch(e=>console.error('Update failed:',e));";
  html += "}";
  
  // Function to control system (arm/disarm)
  html += "function controlSystem(action){";
  html += "if(isUpdating)return;";
  html += "isUpdating=true;";
  html += "const btn=document.getElementById(action+'Btn');";
  html += "const originalText=btn.textContent;";
  html += "btn.classList.add('loading');";
  html += "btn.disabled=true;";
  html += "btn.textContent='';";
  html += "fetch('/'+action,{method:'POST'})";
  html += ".then(()=>{";
  html += "setTimeout(()=>{updateStatus();isUpdating=false;btn.classList.remove('loading');btn.disabled=false;btn.textContent=originalText;},500);";
  html += "})";
  html += ".catch(e=>{console.error('Control failed:',e);isUpdating=false;btn.classList.remove('loading');btn.disabled=false;btn.textContent=originalText;});";
  html += "}";
  
  // Initial update and periodic refresh
  html += "updateStatus();";
  html += "setInterval(updateStatus,2000);";
  html += "</script>";
  
  html += "</body></html>";
  
  client.print(html);
}

void serveStatus(WiFiClient& client) {
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: application/json\r\n";
  response += "Connection: close\r\n\r\n";
  response += "{\"state\":\"";
  response += getStateString(currentState);
  response += "\",\"motion\":";
  response += motionDetected ? "true" : "false";
  response += ",\"doorbell\":";
  response += doorbellPressed ? "true" : "false";
  response += "}";
  
  client.print(response);
}

void armSystem(WiFiClient& client) {
  currentState = ARMED;
  
  // Set baseline distance for ultrasonic sensor
  delay(100); // Small delay to let sensor stabilize
  baselineDistance = getDistance();
  if (baselineDistance == 0) {
    baselineDistance = 200; // Default baseline if no reading
  }
  
  Serial.print("System ARMED via web interface - Baseline distance: ");
  Serial.print(baselineDistance);
  Serial.println("cm");
  
  String response = "HTTP/1.1 303 See Other\r\n";
  response += "Location: /\r\n";
  response += "Connection: close\r\n\r\n";
  
  client.print(response);
}

void disarmSystem(WiFiClient& client) {
  currentState = DISARMED;
  motionDetected = false;
  Serial.println("System DISARMED via web interface");
  
  String response = "HTTP/1.1 303 See Other\r\n";
  response += "Location: /\r\n";
  response += "Connection: close\r\n\r\n";
  
  client.print(response);
}

void serve404(WiFiClient& client) {
  String response = "HTTP/1.1 404 Not Found\r\n";
  response += "Content-Type: text/html\r\n";
  response += "Connection: close\r\n\r\n";
  response += "<html><body><h1>404 - Not Found</h1></body></html>";
  
  client.print(response);
}

// ==================== UTILITY FUNCTIONS ====================
String getStateString(SystemState state) {
  switch (state) {
    case DISARMED: return "DISARMED";
    case ARMED: return "ARMED";
    case ALARM_TRIGGERED: return "ALARM_TRIGGERED";
    default: return "UNKNOWN";
  }
}
