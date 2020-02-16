#include "WiHomeComm.h"
#include "NoBounceButtons.h"
#include "SignalLED.h"
#include "WiHome_Config.h"

WiHomeComm whc;

SignalLED* led[N_LEDS];
SignalLED* relay[N_RELAYS];
NoBounceButtons nbb;
char button[N_BUTTONS];

void setup()
{
  if (SERIAL_DEBUG)
    Serial.begin(115200);
  else
    Serial.end();
  Serial.println();
  delay(100);
  for (int n=0; n<N_BUTTONS; n++)
    button[n] = nbb.create(BUTTON_PIN[n]);
  for (int n=0; n<N_LEDS; n++)
    led[n] = new SignalLED(LED_PIN[n],LED_INIT[n],LED_ACTIVE_LOW[n]);
  for (int n=0; n<N_RELAYS; n++)
    relay[n] = new SignalLED(RELAY_PIN[n],RELAY_INIT[n],RELAY_ACTIVE_LOW[n]);
  // Setup which led to display Wifi status:
  if (LED_RELAY[WIFI_LED]>=0 && LED_RELAY[WIFI_LED]<N_RELAYS)
    whc.set_status_led(led[WIFI_LED], relay[LED_RELAY[WIFI_LED]]);
  else
    whc.set_status_led(led[WIFI_LED]);
}


void report_relay_status(int n)
{
    Serial.printf("Reporting relay channel %d status: %d\n", n, relay[n]->get());
    whc.sendJSON("cmd", "info", "parameter", "relay", "value", relay[n]->get(), "channel", n);
}


void loop()
{
  DynamicJsonBuffer jsonBuffer;

  // Handling routines for various libraries used:
  JsonObject& root = whc.check(&jsonBuffer);
  nbb.check();
  for (int n=0; n<N_LEDS; n++)
    led[n]->check();
  for (int n=0; n<N_RELAYS; n++)
    relay[n]->check();

  // React to button actions:
  for (int n=0; n<N_BUTTONS; n++)
  {
    if (nbb.action(button[n])==NBB_LONG_CLICK)
    {
      Serial.printf("Button %d long click.\n",n);
      Serial.printf("Attempting to go to SoftAP mode.\n");
      whc.softAPmode=true;
      nbb.reset(button[n]);
    }
    if (nbb.action(button[n])==NBB_CLICK)
    {
      if (whc.softAPmode==true)
        whc.softAPmode=false;
      else
      {
        Serial.printf("Button %d click.\n",n);
        if (BUTTON_RELAY[n]>=0)
          relay[BUTTON_RELAY[n]]->invert();
        report_relay_status(BUTTON_RELAY[n]);
      }
      nbb.reset(button[n]);
    }
  }

  // React to received JSON command objects:
  if (root!=JsonObject::invalid())
  {
    int n=0;
    if (root.containsKey("channel"))
      n = root["channel"];
    if (root["cmd"]=="set")
    {
      if (root["parameter"]=="relay" && n<N_RELAYS)
      {
        relay[n]->set((int)root["value"]);
        report_relay_status(n);
      }
    }
    else if (root["cmd"]=="get")
    {
      if (root["parameter"]=="relay" && n<N_RELAYS)
        report_relay_status(n);
      else if (root["parameter"]=="signal")
        whc.sendJSON("cmd", "info", "value", WiFi.RSSI());
    }
  }

  // Sync LEDs with relays per config:
  for (int n=0; n<N_LEDS; n++)
    if (n!=WIFI_LED && LED_RELAY[n]>=0 && LED_RELAY[n]<N_RELAYS)
      led[n]->set(relay[LED_RELAY[n]]->get());

}
