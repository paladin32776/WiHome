#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiHome_Support.h"
#include "WiHome_Config.h"

// Setup Wifi and MQTT Clients
// Create an ESP8266 WiFiClient object to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

/****************************** MQTT Feeds ***************************************/
// MQTT topics: MDNS_CLIENT_NAME/.../...

// Setup a feed called 'photocell' for publishing.
Adafruit_MQTT_Publish status_feed = Adafruit_MQTT_Publish(&mqtt, MDNS_CLIENT_NAME "/led/status");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe command_feed = Adafruit_MQTT_Subscribe(&mqtt, MDNS_CLIENT_NAME "/led/command");


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

  if (!MDNS.begin(MDNS_CLIENT_NAME)) 
  {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  MDNS.addService("esp", "tcp", 8080); // Announce esp tcp service on port 8080
  

  pinMode(LED_BUILTIN, OUTPUT);

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&command_feed);
}

uint32_t x=0;


void loop() 
{
  // Ensure the connection to the MQTT server is alive 
  // (will make the first connection and automatically reconnect when disconnected)
  MQTT_connect(&mqtt);

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
      switch (command.toInt())
      {
        case 1:
          Serial.println(F("\nTurning LED on"));
          digitalWrite(LED_BUILTIN,HIGH);
          break;
        case 0:
          Serial.println(F("\nTurning LED off"));
          digitalWrite(LED_BUILTIN,LOW);
          break;
      }
    }
  }

  // Publish LED status:
  uint32_t led_status = digitalRead(LED_BUILTIN);
  Serial.print(F("\nSending LED status "));
  Serial.print(led_status);
  Serial.print(F("..."));
  if (! status_feed.publish(led_status))
    Serial.println(F("Failed"));
  else 
    Serial.println(F("OK!"));
  

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}


