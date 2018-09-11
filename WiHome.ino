#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiHome_Support.h"
#include "WiHome_Config.h"

// Setup Wifi and MQTT Clients
// Create an ESP8266 WiFiClient object to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client* mqtt;

// Web server:
ConfigWebServer* cws;

// User data class:
UserData ud;

// Setup button debouncing object:
NoBounceButtons nbb;
// and global variables to hold button IDs:
int button1;
int button2;

// Create objects for EnoughTimePassed class:
EnoughTimePassed etp_MQTT_KeepAlive(MQTT_KEEPALIVE);
EnoughTimePassed etp_softAP_mode(600000);
EnoughTimePassed etp_led_blink(500);

// Pointers for publishing and subscribe MQTT objects:
Adafruit_MQTT_Publish* button_feed;
Adafruit_MQTT_Subscribe* led_feed;
Adafruit_MQTT_Subscribe* config_feed;

// Global variable to indicate soft AP mode:
bool is_softAP = false;


void setup()
{
  Serial.begin(115200);
  delay(2000);
  // Load user data (ssid, password, mdsn name, mqtt broker):
  ud.load();
  ud.show();

  // Configure buttons:
  button1 = nbb.create(PIN_INPUT);
  button2 = nbb.create(SOFT_AP_BUTTON);

  // Configure LED pin:
  pinMode(PIN_OUTPUT, OUTPUT);

  // Turn on Wifi and MDNS
  if (Wifi_connect(ud.wlan_ssid, ud.wlan_pass, ud.mdns_client_name, &nbb, button2)==false)
    is_softAP = true;
  else
  {
    // Setup MQTT subscription for command feed
    mqtt = new Adafruit_MQTT_Client(&client, ud.mqtt_broker, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);
    button_feed = Adafruit_MQTT_Publish_3A(mqtt, ud.mdns_client_name, "/button");
    led_feed = Adafruit_MQTT_Subscribe_3A(mqtt, ud.mdns_client_name, "/led");
    config_feed = Adafruit_MQTT_Subscribe_3A(mqtt, ud.mdns_client_name, "/config");
    mqtt->subscribe(led_feed);
    mqtt->subscribe(config_feed);
  }
}


void loop_normal()
{
  // Ensure the connection to the MQTT server is alive
  // (will make the first connection and automatically reconnect when disconnected)
  MQTT_connect(mqtt);

  //cws.handleClient();

  // Check buttons:
  nbb.check();

  // Check subscriptions:
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt->readSubscription(10))) // Wait for 10ms for a subscription message
  {
    if (subscription == led_feed)
    {
      String command = (char*)led_feed->lastread;
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
    if (subscription == config_feed)
    {
      String command = (char*)led_feed->lastread;
      Serial.print(F("Received: "));
      Serial.println(command);
      if (command.compareTo("direct")==0)
      {
          Serial.println(F("\nConfiguring for direct button to led connection"));
      }
      if (command.compareTo("indirect")==0)
      {
          Serial.println(F("\nConfiguring for indirect button to ledelay connection"));
      }
    }
  }

  // Publish if required:
  if (nbb.action(button1))
  {
    Serial.print(F("\nSending toggle signal "));
    Serial.print(F("..."));
    if (! button_feed->publish("toggle"))
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
    if(! mqtt->ping())
      mqtt->disconnect();
}


void loop_softAP()
{
  // TODO: NEED TO PREVENT DOUBLE START OF cws .....!!!
  if (etp_softAP_mode.enough_time())
  {
    Wifi_softAPmode("WiHome_Config_AP");
    cws = new ConfigWebServer(80,&ud);
  }
  // Blink led in Soft AP mode
  if (etp_led_blink.enough_time())
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  // Handle webserver and dnsserver events
  cws->handleClient();
  // Check buttonsß
  nbb.check();
  if (nbb.action(button2))
  {
    Serial.println("Going back to Infrastructure mode ...");
    is_softAP = false;
    nbb.reset(button2);
    setup();
  }
}


void loop()
{
  if (is_softAP)
    loop_softAP();
  else
    loop_normal();
}
