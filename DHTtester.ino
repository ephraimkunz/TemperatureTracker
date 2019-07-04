#include "config.h"

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

#define TIME_BETWEEN 10000
#define DHTTYPE DHT11
#define NUM_DHTS 4

int pins[NUM_DHTS] = {5, 4, 0, 2};

DHT dhts[NUM_DHTS] = {
{pins[0], DHTTYPE},
{pins[1], DHTTYPE},
{pins[2], DHTTYPE},
{pins[3], DHTTYPE},
};

unsigned long prev_millis = 0;

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup feeds
Adafruit_MQTT_Publish feeds[NUM_DHTS] = {
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor1"), 
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor2"), 
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor3"), 
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor4")
};

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting!"));

   // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();

  for (int i = 0; i < NUM_DHTS; ++i) {
    dhts[i].begin();
  }
}

void loop() {
 // Wait a few seconds between measurements.
 if ((millis() - prev_millis) < TIME_BETWEEN) {
  return;
 }

 // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

 prev_millis = millis();
  for (int i = 0; i < NUM_DHTS; ++i) {
    yield();
    DHT dht = dhts[i];
    Adafruit_MQTT_Publish feed = feeds[i];

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      continue;
    }
  
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Sensor: "));
    Serial.print(i + 1);
    Serial.print(F(" Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));

    feed.publish(f);
  }

  Serial.println();
}

// Connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
