#include "WiHome_Support.h"

void Wifi_connect(char* ssid, char* passwd, char* mdns_client_name)
{
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MDNS responder:
  if (!MDNS.begin(mdns_client_name)) 
  {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  MDNS.addService("esp", "tcp", 8080); // Announce esp tcp service on port 8080
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect(Adafruit_MQTT_Client* mqtt)
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt->connected()) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt->connect()) != 0) // connect will return 0 for connected
  { 
       Serial.println(mqtt->connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt->disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}


void Wifi_softAPmode(char* ssid)
{
  
  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("Stopping infrastructure mode.");
    WiFi.disconnect();
  }
    
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(ssid);
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
  
}


// Class EnoughTimePassed - Methods
EnoughTimePassed::EnoughTimePassed(unsigned long desired_intervall) // setup object with desired intervall
{
  intervall = desired_intervall;
  last_time = 0;
  once_called = false;  
}

bool EnoughTimePassed::enough_time()      // create a new button
{
  unsigned long t = millis();
  if ( (t-last_time>intervall)  || (once_called==false) )
    {
      last_time = t;
      once_called = true;
      return true;
    }
  return false;
}

void EnoughTimePassed::event()             // manually tell that an event has happened just now
{
  last_time = millis();
  once_called = true;
}

void EnoughTimePassed::change_intervall(unsigned long desired_intervall)
{
  intervall = desired_intervall;
}

