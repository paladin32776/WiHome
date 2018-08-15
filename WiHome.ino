#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiHome_Support.h"
#include "WiHome_Config.h"
#include "NoBounceButtons.h"


// Setup Wifi and MQTT Clients
// Create an ESP8266 WiFiClient object to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

// Web server:
ConfigWebServer* cws;

// Setup button debouncing object:
NoBounceButtons nbb;
// and global variables to hold button IDs:
int button1;
int button2;

// Create objects for EnoughTimePassed class:
EnoughTimePassed etp_MQTT_KeepAlive(MQTT_KEEPALIVE);
EnoughTimePassed etp_softAP_mode(600000);
EnoughTimePassed etp_led_blink(500);

/****************************** MQTT Feeds ***************************************/
// MQTT topics: MDNS_CLIENT_NAME/.../...

// Setup a feed called 'photocell' for publishing.
Adafruit_MQTT_Publish button_feed = Adafruit_MQTT_Publish(&mqtt, MDNS_CLIENT_NAME "/button");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe led_feed = Adafruit_MQTT_Subscribe(&mqtt, MDNS_CLIENT_NAME "/led");

// Global variable to indicate soft AP mode:
bool is_softAP = false;


void setup() 
{
  Serial.begin(115200);
  delay(10);
  
  // Turn on Wifi and MDNS
  Wifi_connect(WLAN_SSID, WLAN_PASS, MDNS_CLIENT_NAME);
  // Setup MQTT subscription for command feed
  mqtt.subscribe(&led_feed);
  
  // Configure LED pin:
  pinMode(PIN_OUTPUT, OUTPUT);

  // Configure main button:
  button1 = nbb.create(PIN_INPUT);
  button2 = nbb.create(SOFT_AP_BUTTON);
}


void loop_normal() 
{
  // Ensure the connection to the MQTT server is alive 
  // (will make the first connection and automatically reconnect when disconnected)
  MQTT_connect(&mqtt);

  //cws.handleClient();
  
  // Check buttons:
  nbb.check();
  
  // Check subscriptions:
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10))) // Wait for 5s for a subscription message
  {
    if (subscription == &led_feed) 
    {
      String command = (char*)led_feed.lastread;
      Serial.print(F("Received: "));
      Serial.println(command);
      if (command.compareTo("on")==0)
      {
          Serial.println(F("\nTurning LED on"));
          digitalWrite(PIN_OUTPUT,LOW);
      }
      if (command.compareTo("off")==0)
      {
          Serial.println(F("\nTurning LED off"));
          digitalWrite(PIN_OUTPUT,HIGH);
      }
      if (command.compareTo("toggle")==0)
      {
          Serial.println(F("\Toggling LED"));
          digitalWrite(PIN_OUTPUT,!digitalRead(PIN_OUTPUT));
      }
    }
  }

  // Publish if required:
  if (nbb.action(button1))
  {
    Serial.print(F("\nSending toggle signal "));
    Serial.print(F("..."));
    if (! button_feed.publish("toggle"))
      Serial.println(F("Failed"));
    else 
      Serial.println(F("OK!"));
    nbb.reset(button1);
  }

  if (nbb.action(button2))
  {
    Serial.println("Going into SoftAP mode ...");
    is_softAP = true;
    nbb.reset(button2);
  }
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  if (etp_MQTT_KeepAlive.enough_time())
    if(! mqtt.ping()) 
      mqtt.disconnect();
}


void loop_softAP()
{
  if (etp_softAP_mode.enough_time())
  {
    Wifi_softAPmode("WiHome_Config_AP"); 
    cws = new ConfigWebServer(80);
  }
  // Blink led in Soft AP mode
  if (etp_led_blink.enough_time())
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  // Handle webserver and dnsserver events
  cws->handleClient();
  // Check buttons
  nbb.check();
  if (nbb.action(button2))
  {
    Serial.println("Going back to Infrastructure mode ...");
    is_softAP = false;
    nbb.reset(button2);
  }
}


void loop()
{
  if (is_softAP)
    loop_softAP();
  else
    loop_normal();
}


