#include "WiHome_Support.h"

bool Wifi_connect(char* ssid, char* passwd, char* mdns_client_name, NoBounceButtons* nbb, int button)
{
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  if (strlen(ssid)==0)
    return false;
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1);
    nbb->check();
    if (nbb->action(button))
    {
      nbb->reset(button);
      return false;
    }
  }
  if (WiFi.status() == WL_CONNECTED)
  {
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
    return true;
  }
  else
    return false;
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
  // Disconnect infrastructure based Wifi if connected
  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("Stopping infrastructure mode.");
    WiFi.disconnect();
  }
  // Setup Soft AP  
  IPAddress apIP(192, 168, 4, 1);
  IPAddress netMsk(255, 255, 255, 0);
  Serial.print("Setting soft-AP ... ");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  boolean result = WiFi.softAP(ssid);
  if(result == true)
    Serial.println("Ready");
  else
    Serial.println("Failed!");
}


Adafruit_MQTT_Publish* Adafruit_MQTT_Publish_3A(Adafruit_MQTT_Client* mqtt, char* client_name, const char* topic_name)
{
  char buf[64];
  strcpy(buf,client_name);
  strcat(buf,topic_name);
  return new Adafruit_MQTT_Publish(mqtt,buf);
}


Adafruit_MQTT_Subscribe* Adafruit_MQTT_Subscribe_3A(Adafruit_MQTT_Client* mqtt, char* client_name, const char* topic_name)
{
  char buf[64];
  strcpy(buf,client_name);
  strcat(buf,topic_name);
  return new Adafruit_MQTT_Subscribe(mqtt,buf);
}


ConfigWebServer::ConfigWebServer(int port, UserData* pud)
{
  userdata = pud;
  // Setup the DNS server redirecting all the domains to the apIP
  IPAddress apIP(192, 168, 4, 1);
  dnsServer = new DNSServer();
  dnsServer->start(DNS_PORT, "*", apIP);
  webserver = new ESP8266WebServer(port);
  webserver->on("/", std::bind(&ConfigWebServer::handleRoot, this));
  webserver->onNotFound(std::bind(&ConfigWebServer::handleRoot, this));
  webserver->on("/save_and_restart.php", std::bind(&ConfigWebServer::handleSaveAndRestart, this));
  webserver->begin();
  Serial.println("HTTP server started");
}


void ConfigWebServer::handleRoot() 
{
  userdata->load();
  String html = html_config_form1;
  html += userdata->wlan_ssid;
  html += html_config_form2;
  html += userdata->wlan_pass;
  html += html_config_form3;
  html += userdata->mqtt_broker;
  html += html_config_form4;
  html += userdata->mdns_client_name;
  html += html_config_form5;
  webserver->send(200, "text/html", html);
}


void ConfigWebServer::handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webserver->uri();
  message += "\nMethod: ";
  message += (webserver->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webserver->args();
  message += "\n";
  for (uint8_t i=0; i<webserver->args(); i++)
  {
    message += " " + webserver->argName(i) + ": " + webserver->arg(i) + "\n";
  }
  webserver->send(404, "text/plain", message);
}


void ConfigWebServer::handleSaveAndRestart() 
{
  char buf[32];
  String message = "Save and Restart\n\n";
  message += "URI: ";
  message += webserver->uri();
  message += "\nMethod: ";
  message += (webserver->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webserver->args();
  message += "\n";
  for (uint8_t i=0; i<webserver->args(); i++)
  {
    message += " " + webserver->argName(i) + ": " + webserver->arg(i) + "\n";
    if ((webserver->argName(i)).compareTo("wlan_ssid")==0)
      strcpy(userdata->wlan_ssid, (webserver->arg(i)).c_str());
    if ((webserver->argName(i)).compareTo("wlan_pass")==0)
      strcpy(userdata->wlan_pass, (webserver->arg(i)).c_str());
    if ((webserver->argName(i)).compareTo("mqtt_broker")==0)
      strcpy(userdata->mqtt_broker, (webserver->arg(i)).c_str());   
    if ((webserver->argName(i)).compareTo("mdns_client_name")==0)
      strcpy(userdata->mdns_client_name, (webserver->arg(i)).c_str());
  }
  Serial.println("--- Data to be saved begin ---");
  Serial.println(userdata->wlan_ssid);
  Serial.println(userdata->wlan_pass);
  Serial.println(userdata->mqtt_broker);
  Serial.println(userdata->mdns_client_name);
  Serial.println("--- Data to be saved end ---");
  userdata->save();
  message += "Userdata saved to EEPROM.\n";
  Serial.print("Userdata saved to EEPROM.");
  webserver->send(200, "text/plain", message);
}


void ConfigWebServer::handleClient()
{
  dnsServer->processNextRequest();
  webserver->handleClient();
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


UserData::UserData()
{
}

bool UserData::load()
{
  EEPROM.begin(129);
  strcpy(wlan_ssid, "");
  strcpy(wlan_pass, "");
  strcpy(mqtt_broker, "");
  strcpy(mdns_client_name, "");
  EEPROM.get(EEPROM_UserData, ud_id);
  if (ud_id != EEPROM_ud_id)
    return false;
  EEPROM.get(EEPROM_UserData+1,wlan_ssid);
  EEPROM.get(EEPROM_UserData+33,wlan_pass);
  EEPROM.get(EEPROM_UserData+65,mqtt_broker);
  EEPROM.get(EEPROM_UserData+97,mdns_client_name);
  EEPROM.end();
  return true;
}

void UserData::save()
{
  EEPROM.begin(129);
  CharArrayToEEPROM(wlan_ssid, 1, 32);
  CharArrayToEEPROM(wlan_pass, 33, 32);
  CharArrayToEEPROM(mqtt_broker, 65, 32);
  CharArrayToEEPROM(mdns_client_name, 97, 32);
  ud_id = EEPROM_ud_id;
  EEPROM.write(EEPROM_UserData, ud_id);
  EEPROM.end();
  delay(100);
}

void UserData::CharArrayToEEPROM(char* ps, unsigned int offset, unsigned int bytes)
{
  for (unsigned int a=0; a<bytes; a++)
    EEPROM.write(a+offset, *(ps+a));
}

void UserData::CharArrayFromEEPROM(char* ps, unsigned int offset, unsigned int bytes)
{
  for (unsigned int a=0; a<bytes; a++)
    *(ps+a) = EEPROM.read(a+offset);
}





