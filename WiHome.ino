#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiHome_Support.h"
#include "WiHome_Config.h"
#include "MQTT_topic.h"

// Setup Wifi and MQTT Clients
// Create an ESP8266 WiFiClient object to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client* mqtt;

// User data class:
UserData ud;

// Setup button debouncing object:
NoBounceButtons nbb;
// and global variables to hold button IDs:
int button1;

// Setup led and relay:
SignalLED led1(PIN_LED, SLED_BLINK_FAST_1, true);
SignalLED relay1(PIN_RELAY, SLED_OFF, false);

// Create objects for EnoughTimePassed class:
EnoughTimePassed etp_MQTT_KeepAlive(MQTT_KEEPALIVE);

// Pointers to Topics for MQTT publish and subscribe:
MQTT_topic* t_stat_relay_feed;
MQTT_topic* t_cmd_relay_feed;

// Pointers for publishing and subscribe MQTT objects:
Adafruit_MQTT_Publish* stat_relay_feed;
Adafruit_MQTT_Subscribe* cmd_relay_feed;

// Global variable to indicate soft AP mode:
bool is_softAP = false;

bool wlan_ok = false;
bool mqtt_ok = false;

void MQTT_create_feeds()
{
  // Setup MQTT topics for feeds:
  t_stat_relay_feed = new MQTT_topic(ud.mdns_client_name,"/stat/relay");
  t_cmd_relay_feed = new MQTT_topic(ud.mdns_client_name, "/cmd/relay");
  // Setup MQTT client:
  // mqtt = new Adafruit_MQTT_Client(&client, ud.mqtt_broker, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);
  mqtt = new Adafruit_MQTT_Client(&client, ud.mqtt_broker, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);
  // Setup MQTT subscriptions and publications
  stat_relay_feed = new Adafruit_MQTT_Publish(mqtt, t_stat_relay_feed->topic);
  cmd_relay_feed = new Adafruit_MQTT_Subscribe(mqtt, t_cmd_relay_feed->topic);
  mqtt->subscribe(cmd_relay_feed);
}


void MQTT_destroy_feeds()
{
  mqtt->unsubscribe(cmd_relay_feed);
  delete stat_relay_feed;
  delete cmd_relay_feed;
  delete mqtt;
  delete t_stat_relay_feed;
  delete t_cmd_relay_feed;
}


void setup()
{
  Serial.begin(115200);
  delay(2000);
  Serial.printf("WiHome v0.9\n===========\nAuthor:\nGernot\nFattinger\n");
  // Configure buttons:
  button1 = nbb.create(PIN_BUTTON);
  // Load user data (ssid, password, mdsn name, mqtt broker):
  ud.load();
  ud.show();
  // Create MQTT feeds:
  MQTT_create_feeds();
}


void loop_normal()
{
  // Ensure that WiFi Station connection is alive:
  // (will make the first connection and automatically reconnect when disconnected)
  wlan_ok = ConnectStation(ud.wlan_ssid, ud.wlan_pass, ud.mdns_client_name);

  // Ensure the connection to the MQTT server is alive (also checks wlan connection)
  // (will make the first connection and automatically reconnect when disconnected)
  mqtt_ok = MQTT_connect(mqtt);

  if (mqtt_ok)
  {
    // Check subscriptions:
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt->readSubscription(10))) // Wait for 10ms for a subscription message
    {
      if (subscription == cmd_relay_feed)
      {
        String command = (char*)cmd_relay_feed->lastread;
        Serial.print(F("Received: "));
        Serial.println(command);
        if (command.compareTo("on")==0)
        {
            Serial.println(F("Turning relay on"));
            relay1.set(SLED_ON);
        }
        if (command.compareTo("off")==0)
        {
            Serial.println(F("Turning relay off"));
            relay1.set(SLED_OFF);
        }
        if (command.compareTo("toggle")==0)
        {
            Serial.println(F("Toggling relay"));
            relay1.invert();
            bool result=false;
            if (relay1.get()==SLED_ON)
              result = stat_relay_feed->publish("on");
            else if (relay1.get()==SLED_OFF)
              result = stat_relay_feed->publish("off");
            if (result)
              Serial.println(F("Ok!"));
            else
              Serial.println(F("Failed."));
        }
      }
    }
  }

  // Logic for LED status display:
  if (mqtt_ok)
  {
    led1.set(relay1.get());
  }
  else if (wlan_ok)
  {
    led1.set(SLED_BLINK_FAST_3);
  }
  else
  {
    led1.set(SLED_BLINK_FAST_1);
  }

  // LED and RELAY checks:
  led1.check();
  relay1.check();
  // Check buttons:
  nbb.check();

  // Publish if required:
  if (nbb.action(button1)==1)
  {
    Serial.print(F("\nButton1 pressed ..."));
    relay1.invert();
    bool result=false;
    if (relay1.get()==SLED_ON)
      result = stat_relay_feed->publish("on");
    else if (relay1.get()==SLED_OFF)
      result = stat_relay_feed->publish("off");
    if (result)
      Serial.println(F("Ok!"));
    else
      Serial.println(F("Failed."));
    nbb.reset(button1);
  }

  if (nbb.action(button1)==2)
  {
    Serial.println("Going into SoftAP mode ...");
    is_softAP = true;
    MQTT_destroy_feeds();
    led1.set(SLED_BLINK_SLOW); // Blink led slow in SoftAP mode
    nbb.reset(button1);
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  if (etp_MQTT_KeepAlive.enough_time() && WiFi.isConnected())
    if(! mqtt->ping())
      mqtt->disconnect();
}


void loop_softAP()
{
  ConnectSoftAP("WiHome_Config_AP", &ud);

  // LED checks:
  led1.check();
  // Check buttons
  nbb.check();

  if (nbb.action(button1)==1)
  {
    Serial.println("Going back to Station mode ...");
    is_softAP = false;
    MQTT_create_feeds();
    led1.set(SLED_OFF); // Turn off led when going to station mode
    nbb.reset(button1);
  }
}


void loop()
{
  if (is_softAP)
    loop_softAP();
  else
    loop_normal();
}
