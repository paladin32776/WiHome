#include "WiHomeComm.h"
#include "NoBounceButtons.h"
#include "SignalLED.h"
#include "WiHome_Config.h"

WiHomeComm whc;

SignalLED led(PIN_LED,SLED_BLINK_FAST_1,PIN_LED_ACTIVE_LOW);
SignalLED relay(PIN_RELAY,SLED_OFF,PIN_RELAY_ACTIVE_LOW);
NoBounceButtons nbb;
char button;

void setup()
{
  if (SERIAL_DEBUG)
    Serial.begin(115200);
  else
    Serial.end();
  Serial.println();
  delay(100);
  button = nbb.create(PIN_BUTTON);
}


void report_relay_status()
{
  Serial.printf("Reporting relay status (now %d).\n",relay.get());
  whc.sendJSON("cmd", "info", "parameter", "relay", "value", relay.get());
}


void loop()
{
  DynamicJsonBuffer jsonBuffer;

  // Handling routines for various libraries used:
  JsonObject& root = whc.check(&jsonBuffer);
  nbb.check();
  led.check();
  relay.check();

  // Logic for LED status display:
  if (whc.status()==1)
    led.set(relay.get());
  else if (whc.status()==2)
    led.set(SLED_BLINK_FAST_3);
  else if (whc.status()==3)
    led.set(SLED_BLINK_FAST_1);
  else if (whc.status()==4)
    led.set(SLED_BLINK_SLOW);
  else
    led.set(SLED_BLINK_FAST);

  // React to button actions:
  if (nbb.action(button)==2)
  {
    Serial.printf("Button1 pressed (action=2).\n");
    Serial.printf("Attempting to go to SoftAP mode.\n");
    whc.softAPmode=true;
    nbb.reset(button);
  }
  if (nbb.action(button)==1)
  {
    if (whc.softAPmode==true)
      whc.softAPmode=false;
    else
    {
      Serial.printf("Button1 pressed (action=1).\n");
      relay.invert();
      Serial.printf("LED is now %d.\n",relay.get());
      report_relay_status();
    }
    nbb.reset(button);
  }

  // React to received JSON command objects:
  if (root!=JsonObject::invalid())
  {
    if (root["cmd"]=="set")
    {
      if (root["parameter"]=="relay")
      {
        relay.set((int)root["value"]);
        report_relay_status();
      }
    }
    else if (root["cmd"]=="get")
    {
      if (root["parameter"]=="relay")
        report_relay_status();
      else if (root["parameter"]=="signal")
        whc.sendJSON("cmd", "info", "value", WiFi.RSSI());
    }
  }

}
