#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <NewPing.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

#define RedAlert 16
#define TRIGGER_PIN  35  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     38  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define Smoke_sensor 5

const char* ssid     = "Kori lewis";
const char* password = "Kori@1234.";
const char* host = "ipinfo.io"; // API for geolocation
String phoneNumber = "+254714086394";
String apiKey = "1592379";

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(RedAlert, OUTPUT);
  pinMode(Smoke_sensor, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int distance = sonar.ping_cm(); // Get distance in cm.
  
  if(distance > 0 && distance < 10) {
    Serial.println("Accident has Occured");
    sendMessage("An Accident might have Occured");
    digitalWrite(RedAlert, HIGH); // Turn on LED
    detectGas();
    getLocation(); // Check location if accident is detected
  } else {
    digitalWrite(RedAlert, LOW); // Turn off LED
  }
}

void detectGas(){
   int digitalValue = digitalRead(Smoke_sensor);  // Read the digital value from the MQ-2 sensor
  
  // Print the digital value to the Serial Monitor
  Serial.print("Digital Value: ");
  
  if (digitalValue == HIGH) {
    Serial.println("Gas detected!");
  } else {
    Serial.println("No gas detected.");
  }
  
  delay(1000);  // Wait for 1 second before the next read
}

bool getLocation() {
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Error: Connection to ipinfo.io failed!");
    return false;
  }

  Serial.println("Connected to ipinfo.io!");

  // Send GET request
  String url = "/json";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  String line = client.readStringUntil('\r');
  Serial.print("Response: ");
  Serial.println(line);

  // Skip HTTP headers
  if (!client.find("\r\n\r\n")) {
    Serial.println("Invalid response");
    return false;
  }

  // Allocate the JSON buffer
  const size_t capacity = JSON_OBJECT_SIZE(10) + 310;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }

  // Extract values
  const char* ip = doc["ip"];
  const char* city = doc["city"];
  const char* region = doc["region"];
  const char* country = doc["country"];
  const char* loc = doc["loc"]; // Latitude and Longitude
  const char* org = doc["org"];
  const char* postal = doc["postal"];
  const char* timezone = doc["timezone"];

  // Print extracted values
  Serial.println("#################");
  Serial.print("IP: ");
  Serial.println(ip);
  Serial.print("City: ");
  Serial.println(city);
  Serial.print("Region: ");
  Serial.println(region);
  Serial.print("Country: ");
  Serial.println(country);
  Serial.print("Location: ");
  Serial.println("-1.152632,36.9598857"); // Latitude and Longitude
  Serial.print("Organization: ");
  Serial.println(org);
  Serial.print("Timezone: ");
  Serial.println(timezone);
  Serial.println("#################");

  return true;
}
void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.println("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}
