#include <DHT.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include "secrets.h"

// Wi-Fi network credentials
char networkSSID[] = SECRET_SSID;     
char networkPassword[] = SECRET_PASS;     

WiFiClient wifiConnection;

// ThingSpeak channel and API key
unsigned long channelID = SECRET_CH_ID;
const char * writeAPIKey = SECRET_WRITE_APIKEY;

// DHT sensor configuration
#define SENSOR_PIN 2        
#define SENSOR_MODEL DHT22  

DHT temperatureSensor(SENSOR_PIN, SENSOR_MODEL);

void setup() {
  Serial.begin(9600);  
  temperatureSensor.begin();  

  // Attempt to connect to the Wi-Fi network
  Serial.print("Attempting to connect to Wi-Fi");
  while (WiFi.begin(networkSSID, networkPassword) != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi!");

  ThingSpeak.begin(wifiConnection);  // Initialize connection to ThingSpeak
}

void loop() {
  // Fetch temperature from the DHT sensor
  float tempReading = temperatureSensor.readTemperature();

  // Verify if the temperature reading is valid
  if (isnan(tempReading)) {
    Serial.println("Error: Unable to read from the DHT sensor.");
    return;
  }

  // Display the temperature on the Serial Monitor
  Serial.print("Current Temperature: ");
  Serial.print(tempReading);
  Serial.println(" °C");

  // Send temperature data to ThingSpeak
  ThingSpeak.setField(1, tempReading);
  int responseCode = ThingSpeak.writeFields(channelID, writeAPIKey);

  // Check if the data was successfully sent
  if (responseCode == 200) {
    Serial.println("Data successfully sent to ThingSpeak.");
  } else {
    Serial.println("Failed to send data. HTTP Response Code: " + String(responseCode));
  }

  // Pause for 60 seconds before the next reading
  delay(60000);
}
