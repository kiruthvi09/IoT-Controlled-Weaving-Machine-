#include <WiFi.h>
#include <HTTPClient.h>

#define HALL_PIN       35    // Digital pin connected to the Hall effect sensor
#define VIBRATION_PIN  34    // Digital pin connected to the vibration sensor
#define BUZZER_PIN     32    // Digital pin connected to the buzzer
#define SEND_INTERVAL  5000  // Interval for sending data (milliseconds)

// WiFi credentials
const char *ssid = "wifi name";
const char *password = "wifi password";

// Google Script ID
const char *GOOGLE_SCRIPT_ID = "your script ID";

volatile int magnetCount = 0;
bool vibrationDetected = false;

void IRAM_ATTR hallSensorISR() {
  magnetCount++;  // Increment magnet count on each interrupt
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Initialize Hall effect sensor
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallSensorISR, RISING);

  // Initialize vibration sensor
  pinMode(VIBRATION_PIN, INPUT);

  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Check for vibration
  if (digitalRead(VIBRATION_PIN) == HIGH) {
    vibrationDetected = true;
    Serial.println("Vibration detected");
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
  } else {
    vibrationDetected = false;
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
  }

  delay(SEND_INTERVAL);  // Wait for the send interval

  // Send data to Google Spreadsheet
  sendData("tag=magnet_count&value=" + String(magnetCount));
  sendData("tag=vibration_status&value=" + String(vibrationDetected ? "vibration detected" : "no vibration"));
}

void sendData(String params) {
  HTTPClient http;

  // Construct the URL with the Google Script ID and parameters
  String url = "https://script.google.com/macros/s/" + String(GOOGLE_SCRIPT_ID) + "/exec?" + params;

  // Send HTTP GET request to the Google Apps Script web app
  http.begin(url); // Specify the URL
  int httpCode = http.GET();
  http.end();

  // Print the result of the HTTP request
  Serial.print("HTTP response code: ");
  Serial.println(httpCode);
}
