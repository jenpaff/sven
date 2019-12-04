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
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "tw_guest"
#define WLAN_PASS       "...your password..."

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "farmer.cloudmqtt.com"
#define AIO_SERVERPORT   12892                    // use 8883 for SSL
#define AIO_USERNAME    "hgjpspdi"
#define AIO_KEY         "...password..."

/************************** Sven Stuff ***************************************/
int input_switch = 2;
int led = 4;
unsigned long timer = 0;
long threshold = 5000;
long interval = threshold*2;
int counter = 1;

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Feed where we will publish a message if the door is open
Adafruit_MQTT_Publish doorFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/door");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));
  
  // Input for door switch
  pinMode (input_switch, INPUT_PULLUP);
  // output for LED
  pinMode (led, OUTPUT);

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

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

}

uint32_t x=0;



void loop() {

  // Read the door switch
  bool switch_state = digitalRead(input_switch);
  
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
    
  bool switch_state = digitalRead(input_switch);

  if (!doorFeed.publish(switch_state)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
  Serial.print("timer = ");
  Serial.println(timer);
 
  if (switch_state) {
    Serial.println("open");
    Serial.println(switch_state);
    
    if ((unsigned long)(millis() - timer) >= threshold) {
      // if timer > 30 seconds, turn on light
      digitalWrite(led, switch_state);
      
      if ((unsigned long)(millis() - timer) >= interval) {
        // after that, send a warning every 30 seconds
        Serial.print("WARNING #");
        //counter=counter+1;
        Serial.println(counter++);
        interval+=threshold;
      }
    } 
  } else {
      Serial.println("closed");

      /*
       * light off and reset values
       */
      digitalWrite(led, switch_state); // light off
      timer = millis(); // "reset" timer
      counter = 1;
      interval = threshold*2;
  }

  delay(1000);
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
