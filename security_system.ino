/*
 * Arduino R4 WiFi Security System
 * PIR Motion Detection with Web-Controlled Arm/Disarm
 * 
 * Author: Dante D'Abramo
 * Date: 2026-01-30
 * 
 * Hardware Connections:
 * - PIR Sensor Output: D2
 * - Status LED (3mm): D8
 * - Alarm LED (5mm): D9
 * - Piezo Buzzer: D10
 */

#include <WiFiS3.h>

// ==================== CONFIGURATION ====================
// Update these with your WiFi credentials
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PWORD";

// Security PIN
const String SECURITY_PIN = "0421";

// Pin Definitions
const int PIR_PIN = 2;
const int STATUS_LED_PIN = 8;
const int ALARM_LED_PIN = 9;
const int BUZZER_PIN = 10;

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
  pinMode(PIR_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ALARM_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Ensure all outputs are off at startup
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(ALARM_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
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
  
  delay(100); // Small delay to prevent CPU overload
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
  
  // Check PIR sensor
  int pirValue = digitalRead(PIR_PIN);
  if (pirValue == HIGH) {
    motionDetected = true;
    lastMotionTime = millis();
    currentState = ALARM_TRIGGERED;
    Serial.println("MOTION DETECTED! Alarm triggered.");
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
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin:50px;background:#f0f0f0;}";
  html += "h1{color:#333;}";
  html += ".container{background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);max-width:400px;margin:auto;}";
  html += "input{font-size:18px;padding:12px;width:80%;margin:10px 0;border:2px solid #ddd;border-radius:5px;text-align:center;}";
  html += "button{font-size:18px;padding:15px 30px;margin:10px;border:none;border-radius:5px;cursor:pointer;background:#4CAF50;color:white;width:80%;}";
  html += "button:hover{background:#45a049;}";
  html += ".error{color:#f44336;margin:10px;padding:10px;background:#ffebee;border-radius:5px;}";
  html += ".lock{font-size:60px;margin:20px;}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<div class='lock'>🔒</div>";
  html += "<h1>Security System</h1>";
  html += "<h2>Enter PIN</h2>";
  
  if (errorMsg.length() > 0) {
    html += "<div class='error'>" + errorMsg + "</div>";
  }
  
  html += "<form method='POST' action='/login'>";
  html += "<input type='password' name='pin' maxlength='4' pattern='[0-9]{4}' placeholder='Enter 4-digit PIN' required autofocus>";
  html += "<button type='submit'>UNLOCK</button>";
  html += "</form>";
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
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin:50px;background:#f0f0f0;}";
  html += "h1{color:#333;}";
  html += ".container{background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);max-width:500px;margin:auto;}";
  html += ".status{font-size:24px;margin:20px;padding:15px;border-radius:5px;}";
  html += ".disarmed{background:#90EE90;color:#006400;}";
  html += ".armed{background:#FFD700;color:#8B4500;}";
  html += ".alarm{background:#FF6347;color:#8B0000;animation:blink 1s infinite;}";
  html += "@keyframes blink{0%,50%{opacity:1;}51%,100%{opacity:0.5;}}";
  html += "button{font-size:18px;padding:15px 30px;margin:10px;border:none;border-radius:5px;cursor:pointer;transition:0.3s;}";
  html += ".arm-btn{background:#4CAF50;color:white;}.arm-btn:hover{background:#45a049;}";
  html += ".disarm-btn{background:#f44336;color:white;}.disarm-btn:hover{background:#da190b;}";
  html += ".info{margin-top:30px;padding:15px;background:#e3f2fd;border-radius:5px;font-size:14px;}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>🔒 Security System</h1>";
  html += "<div class='status ";
  
  switch (currentState) {
    case DISARMED:
      html += "disarmed'>DISARMED</div>";
      break;
    case ARMED:
      html += "armed'>ARMED</div>";
      break;
    case ALARM_TRIGGERED:
      html += "alarm'>⚠️ ALARM TRIGGERED ⚠️</div>";
      break;
  }
  
  html += "<form method='POST'>";
  html += "<button class='arm-btn' formaction='/arm'>ARM SYSTEM</button>";
  html += "<button class='disarm-btn' formaction='/disarm'>DISARM SYSTEM</button>";
  html += "</form>";
  html += "<form method='GET' action='/logout'>";
  html += "<button style='background:#757575;color:white;font-size:14px;padding:10px 20px;margin-top:20px;' type='submit'>LOGOUT</button>";
  html += "</form>";
  html += "<div class='info'>";
  html += "<strong>System Info:</strong><br>";
  html += "PIR Sensor: Pin D2<br>";
  html += "Status LED: Pin D8<br>";
  html += "Alarm LED: Pin D9<br>";
  html += "Buzzer: Pin D10";
  html += "</div></div>";
  html += "<script>setTimeout(function(){location.reload();},3000);</script>";
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
  response += "}";
  
  client.print(response);
}

void armSystem(WiFiClient& client) {
  currentState = ARMED;
  Serial.println("System ARMED via web interface");
  
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
