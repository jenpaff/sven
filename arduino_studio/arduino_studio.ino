/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h> // TODO: #include <Wifi.h> instead ?
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "twguest"
#define WLAN_PASS       "....password...."

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "farmer.cloudmqtt.com"
#define AIO_SERVERPORT   12892                    // use 8883 for SSL / 12892 no SSL
#define AIO_USERNAME    "hgjpspdi"
#define AIO_KEY         "....password...."

/************************** Sven Stuff ***************************************/
int input_switch = 0;

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;
// or... use WiFiFlientSecure for SSL
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Feed where we will publish a message if the door is open
Adafruit_MQTT_Publish doorFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/door");


// I would like to test if this works, might not be necessary for final version
Adafruit_MQTT_Subscribe onofflight = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onofflight");


/*************************** Sketch Code ************************************/

// TODO: Can we check if it also works without?
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
//void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Sven is waking up ... "));

  // Input for door switch
  pinMode (input_switch, INPUT_PULLUP);
  // output for LED
  pinMode (LED_BUILTIN, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("Sven is connected to WIFI");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onofflight);

}

uint32_t x=0;
bool led_state = false;

void loop() {

  // Read the door switch
  bool switch_state = digitalRead(input_switch);

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // TODO: do we need to subscribe? for now to turn on / off led light for testing
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onofflight) {
        led_state = !led_state;
        digitalWrite(LED_BUILTIN, led_state);
    }
  }

  // Publish sven's status
  if (!doorFeed.publish(switch_state)) {
    Serial.println(F("Couldn't send sven's status"));
  } else {
    Serial.println(F("OK!"));
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}