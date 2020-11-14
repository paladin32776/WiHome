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
  DynamicJsonDocument doc(1024);
  // Handling routines for various libraries used:
  whc.check(doc);
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
  if (~doc.isNull())
  {
    int n=0;
    if (doc.containsKey("channel"))
      n = doc["channel"];
    if (doc["cmd"]=="set")
    {
      if (doc["parameter"]=="on" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_on((int)doc["value"]);
        whc.sendJSON("cmd", "info", "parameter", "on", "value", rgbstrip[n]->get_on(), "channel", n);
      }
      if (doc["parameter"]=="hue" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_hue((int)doc["value"]);
        whc.sendJSON("cmd", "info", "parameter", "hue", "value", rgbstrip[n]->get_hue(), "channel", n);
      }
      if (doc["parameter"]=="saturation" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_saturation((int)doc["value"]);
        whc.sendJSON("cmd", "info", "parameter", "saturation", "value", rgbstrip[n]->get_saturation(), "channel", n);
      }
      if (doc["parameter"]=="brightness" && n<N_RGBSTRIPS)
      {
        rgbstrip[n]->set_brightness((int)doc["value"]);
        whc.sendJSON("cmd", "info", "parameter", "brightness", "value", rgbstrip[n]->get_brightness(), "channel", n);
      }
    }
    else if (doc["cmd"]=="get")
    {
      if (doc["parameter"]=="on" && n<N_RGBSTRIPS)
        whc.sendJSON("cmd", "info", "parameter", "on", "value", rgbstrip[n]->get_on(), "channel", n);
      else if (doc["parameter"]=="signal")
        whc.sendJSON("cmd", "info", "value", WiFi.RSSI());
    }
  }

  // Sync LEDs with rgbstrip status per config:
  // for (int n=0; n<N_LEDS; n++)
  //   if (n!=WIFI_LED && LED_RGBSTRIP[n]>=0 && LED_RGBSTRIP[n]<N_RGBSTRIPS)
  //     led[n]->set(rgbstrip[LED_RGBSTRIP[n]]->get_on());

}
