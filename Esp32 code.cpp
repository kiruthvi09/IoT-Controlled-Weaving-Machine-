#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <GoogleSheets.h>

#define IR_SENSOR_PIN 13
#define VIBRATION_SENSOR_PIN 14

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

#define GOOGLE_SHEET_ID "YourGoogleSheetID"
#define GOOGLE_SHEET_NAME "Sheet1"

const char* GSCRIPT_ID = "YourGoogleAppsScriptWebAppURL";

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
GoogleSheets GSheets;

int count = 0;
float temperature = 0.0;
int vibrationValue = 0;

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_PIN, INPUT);

  mlx.begin();
  connectToWiFi();
  GSheets.begin(GSCRIPT_ID);
}

void loop() {
  temperature = mlx.readObjectTempC();
  count = digitalRead(IR_SENSOR_PIN); // Assuming IR sensor gives digital output
  vibrationValue = analogRead(VIBRATION_SENSOR_PIN);

  // Send data to Google Sheets
  sendDataToGoogleSheets();

  delay(5000); // Delay 5 seconds before next reading
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected");
}

void sendDataToGoogleSheets() {
  String data = String(temperature) + "," + String(count) + "," + String(vibrationValue);
  GSheets.appendRow(GOOGLE_SHEET_ID, GOOGLE_SHEET_NAME, data);
}
