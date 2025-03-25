#include <WiFi.h>
#include <HTTPClient.h>

// Pin Definitions for three FSR sensors
#define FSR1_PIN 32   // ADC1 channel: GPIO32
#define FSR2_PIN 33   // ADC1 channel: GPIO33
#define FSR3_PIN 34   // ADC1 channel: GPIO34

// WiFi Credentials
const char* ssid = " David";       // Replace with your WiFi SSID (ensure no extra spaces)
const char* password = "David128"; // Replace with your WiFi password

// Server URL - adjust if necessary
const char* serverURL = "https://testit-theta.vercel.app/api/readings";

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Connect to WiFi
  connectToWiFi();

  Serial.println("Setup complete. Starting sensor readings.");
}

void connectToWiFi() {
  Serial.printf("Connecting to %s ...\n", ssid);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Read an individual FSR sensor value
int readFSR(int pin) {
  return analogRead(pin);
}

// Send the three FSR sensor values to your API endpoint
void sendDataToServer(int fsr1, int fsr2, int fsr3) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Build JSON payload with fsr1, fsr2, and fsr3 fields
    String jsonPayload = "{ \"fsr1\": " + String(fsr1) +
                         ", \"fsr2\": " + String(fsr2) +
                         ", \"fsr3\": " + String(fsr3) + " }";

    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void loop() {
  // Read sensor values from each FSR
  int fsrValue1 = readFSR(FSR1_PIN);
  int fsrValue2 = readFSR(FSR2_PIN);
  int fsrValue3 = readFSR(FSR3_PIN);

  Serial.printf("FSR Reading on pin %d: %d\n", FSR1_PIN, fsrValue1);
  Serial.printf("FSR Reading on pin %d: %d\n", FSR2_PIN, fsrValue2);
  Serial.printf("FSR Reading on pin %d: %d\n", FSR3_PIN, fsrValue3);

  // Optionally, provide status messages for each sensor
  // (You can adjust these thresholds based on your calibration needs)
  if (fsrValue1 < 50)
    Serial.println("FSR1 Status: No pressure");
  else if (fsrValue1 < 500)
    Serial.println("FSR1 Status: Light touch");
  else if (fsrValue1 < 1500)
    Serial.println("FSR1 Status: Light squeeze");
  else if (fsrValue1 < 2500)
    Serial.println("FSR1 Status: Medium squeeze");
  else
    Serial.println("FSR1 Status: Big squeeze");

  // Repeat similar messaging if desired for fsrValue2 and fsrValue3

  // Send the sensor readings to the backend API
  sendDataToServer(fsrValue1, fsrValue2, fsrValue3);

  // Wait 1 second before the next reading cycle
  delay(1000);
}
