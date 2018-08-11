#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi Settings 
#define WLAN_SSID       "ExtremeG"
#define WLAN_PASS       "rideldibixwpa2"

// MQTT Server Settings 
#define MQTT_SERVER      "cncmill.local"
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""

// Setup Wifi and MQTT Clients

// Create an ESP8266 WiFiClient object to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish status_feed = Adafruit_MQTT_Publish(&mqtt, "/led/status");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe command_feed = Adafruit_MQTT_Subscribe(&mqtt, "/led/command");

/*************************** Sketch Code ************************************/

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() 
{
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

void setup() 
{
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&command_feed);
}

uint32_t x=0;

void loop() 
{
  // Ensure the connection to the MQTT server is alive 
  // (will make the first connection and automatically reconnect when disconnected)
  MQTT_connect();

  // this is our "wait for incoming subscription packets" busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) // Wait for 5s for a subscription message
  {
    if (subscription == &command_feed) 
    {
      String command = (char*)command_feed.lastread;
      Serial.print(F("Got: "));
      Serial.println(command);
      
      
    }
  }

  // Now we can publish stuff!
  Serial.print(F("\nSending photocell val "));
  Serial.print(x);
  Serial.print("...");
  if (! status_feed.publish(x++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}


