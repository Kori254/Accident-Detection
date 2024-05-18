# Accident-Detection
This project is designed to detect car accidents using an ultrasonic sensor and check for gas leaks using an MQ-2 gas sensor. Upon detecting an accident, the system also retrieves the car's location using an IP-based geolocation API. The project uses an ESP32 microcontroller.
Components

    *ESP32 microcontroller
    *Ultrasonic sensor (e.g., HC-SR04)
    *MQ-2 gas sensor
    *LED (for alert indication)
    *WiFi connection
Libraries

    *NewPing
    *WiFi
    *WiFiClientSecure
    *ArduinoJson
  Setup and Installation

    Install Arduino IDE:
        Download and install the Arduino IDE from Arduino's official website.

    Install ESP32 Board:
        In Arduino IDE, go to File > Preferences and add the following URL to Additional Board Manager URLs:
        ```
        https://dl.espressif.com/dl/package_esp32_index.json
        ```
    Go to Tools > Board > Board Manager and search for "ESP32". Install the esp32 package by Espressif Systems.
    Install Required Libraries:

    Open Arduino IDE and go to Sketch > Include Library > Manage Libraries and install the following libraries:
        NewPing
        ArduinoJson

Upload Code to ESP32:

    Connect your ESP32 to your computer via USB.
    Open the provided code in Arduino IDE.
    Replace SSID and PSK with your WiFi credentials.
    Select the correct board and port under Tools > Board and Tools > Port.
    Click the Upload button to upload the code to the ESP32.
    Setup

The setup initializes the serial communication, connects to WiFi, and sets the pin modes for the sensors and the LED.
```C++
void setup() {
  Serial.begin(115200);
  pinMode(RedAlert, OUTPUT);
  pinMode(Smoke_sensor, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}```

Main Loop

The loop function constantly checks the distance using the ultrasonic sensor. If an accident is detected (distance between 0 and 10 cm), it turns on the LED, checks for gas leaks, and retrieves the location.
``` C++
void loop() {
  delay(50);
  unsigned int distance = sonar.ping_cm();

  if(distance > 0 && distance < 10) {
    Serial.println("Accident has Occured");
    digitalWrite(RedAlert, HIGH);
    detectGas();
    getLocation();
  } else {
    digitalWrite(RedAlert, LOW);
  }
}```
Gas Detection

The detectGas() function reads the digital value from the MQ-2 sensor and prints whether gas is detected or not.
```C++
void detectGas() {
  int digitalValue = digitalRead(Smoke_sensor);
  Serial.print("Digital Value: ");
  
  if (digitalValue == HIGH) {
    Serial.println("Gas detected!");
  } else {
    Serial.println("No gas detected.");
  }
  
  delay(1000);
}```

Location Detection

The getLocation() function connects to the ipinfo.io API and retrieves the location based on the IP address.
```C++
bool getLocation() {
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Error: Connection to ipinfo.io failed!");
    return false;
  }

  Serial.println("Connected to ipinfo.io!");

  String url = "/json";
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

  String line = client.readStringUntil('\r');
  Serial.print("Response: ");
  Serial.println(line);

  if (!client.find("\r\n\r\n")) {
    Serial.println("Invalid response");
    return false;
  }

  const size_t capacity = JSON_OBJECT_SIZE(10) + 310;
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }

  const char* ip = doc["ip"];
  const char* city = doc["city"];
  const char* region = doc["region"];
  const char* country = doc["country"];
  const char* loc = doc["loc"];
  const char* org = doc["org"];
  const char* postal = doc["postal"];
  const char* timezone = doc["timezone"];

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
  Serial.println(loc);
  Serial.print("Organization: ");
  Serial.println(org);
  Serial.print("Postal: ");
  Serial.println(postal);
  Serial.print("Timezone: ");
  Serial.println(timezone);
  Serial.println("#################");

  return true;
}
```
Notes

    Ensure the ESP32 board is properly connected and the correct board and port are selected in the Arduino IDE.
    For more accurate location tracking, consider integrating a GPS module.

Troubleshooting

    If the WiFi connection fails, check your SSID and password.
    Ensure the ultrasonic and MQ-2 sensors are correctly wired.
    Verify that the libraries are correctly installed in the Arduino IDE.

Future Improvements
Implement more sophisticated accident detection algorithms using accelerometers or gyroscopes.
