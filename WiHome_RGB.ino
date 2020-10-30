#include "WiHomeComm.h"
#include "NoBounceButtons.h"
#include "SignalLED.h"
#include "RGBstrip.h"
#include "WiHome_Config.h"

WiHomeComm whc;

SignalLED* led[N_LEDS];
RGBstrip* rgbstrip[N_RGBSTRIPS];
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
  for (int n=0; n<N_RGBSTRIPS; n++)
    rgbstrip[n] = new RGBstrip(RGBSTRIP_PIN[n][0],RGBSTRIP_PIN[n][1],RGBSTRIP_PIN[n][2],RGBSTRIP_ACTIVE_LOW[n]);
  // Setup led to display Wifi status:
  whc.set_status_led(led[WIFI_LED]);
}


void loop()
{
  DynamicJsonBuffer jsonBuffer;

  // Handling routines for various libraries used:
  JsonObject& root = whc.check(&jsonBuffer);
  nbb.check();
  for (int n=0; n<N_LEDS; n++)
    led[n]->check();
  for (int n=0; n<N_RGBSTRIPS; n++)
    rgbstrip[n]->check();

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
        if (BUTTON_RGBSTRIP[n]>=0)
        {
          rgbstrip[BUTTON_RGBSTRIP[n]]->invert_on();
          whc.sendJSON("cmd", "info", "parameter", "on", "value", rgbstrip[n]->get_on(), "channel", n);
        }
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
      if (root["parameter"]=="on" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_on((int)root["value"]);
        whc.sendJSON("cmd", "info", "parameter", "on", "value", rgbstrip[n]->get_on(), "channel", n);
      }
      if (root["parameter"]=="hue" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_hue((int)root["value"]);
        whc.sendJSON("cmd", "info", "parameter", "hue", "value", rgbstrip[n]->get_hue(), "channel", n);
      }
      if (root["parameter"]=="saturation" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_saturation((int)root["value"]);
        whc.sendJSON("cmd", "info", "parameter", "saturation", "value", rgbstrip[n]->get_saturation(), "channel", n);
      }
      if (root["parameter"]=="brightness" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_brightness((int)root["value"]);
        whc.sendJSON("cmd", "info", "parameter", "brightness", "value", rgbstrip[n]->get_brightness(), "channel", n);
      }
    }
    else if (root["cmd"]=="get")
    {
      if (root["parameter"]=="on" && n<N_RGBSTRIPS)
        whc.sendJSON("cmd", "info", "parameter", "on", "value", rgbstrip[n]->get_on(), "channel", n);
      else if (root["parameter"]=="signal")
        whc.sendJSON("cmd", "info", "value", WiFi.RSSI());
    }
  }

  // Sync LEDs with rgbstrip status per config:
  // for (int n=0; n<N_LEDS; n++)
  //   if (n!=WIFI_LED && LED_RGBSTRIP[n]>=0 && LED_RGBSTRIP[n]<N_RGBSTRIPS)
  //     led[n]->set(rgbstrip[LED_RGBSTRIP[n]]->get_on());

}
