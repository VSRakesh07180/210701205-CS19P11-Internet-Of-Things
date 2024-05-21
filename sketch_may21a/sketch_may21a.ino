#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>

#define DHTPIN D2     // Pin connected to DHT11 data pin
#define DHTTYPE DHT11  // DHT 11
#define BUZZER_PIN D6 // Pin connected to the buzzer
#define GAS_THRESHOLD 535 // Adjust this value according to your requirements


DHT dht(DHTPIN, DHTTYPE);

// Replace with your network credentials
const char* ssid = "VSRAKESH";
const char* password = "rakeshvivo";

// ThingSpeak settings
unsigned long myChannelNumber = 2555488;
const char * myWriteAPIKey = "VOM7T6OCM2Q2HHVB";

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  ThingSpeak.begin(client);

  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Reading temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Reading gas level
  int gasValue = analogRead(A0);
  
  // Print values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Gas Level: ");
  Serial.println(gasValue);

  // Update ThingSpeak channel with new data
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, gasValue);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Buzzer control
  if (gasValue > GAS_THRESHOLD) {
    // Turn on the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    // Wait for some time
    delay(3000);
    // Turn off the buzzer
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Wait 3 seconds before sending another update
  delay(3000);
}
