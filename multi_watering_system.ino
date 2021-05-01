
#include "bat.h"
#include "bluetooth.h"
#include <ArduinoJson.h>

struct DevicePins
{
  int pump;
  int sensor;
  int sensorMin;
  int sensorMax;
  int pumpDelay;
};

const DevicePins devices[] = {
    {16, A6, 0, 0, 0},
    {15, A5, 0, 0, 0},
    {7, A4, 0, 0, 0},
    {11, A3, 0, 0, 0},
    {27, A2, 0, 0, 0},
    {26, A1, 0, 0, 0},
    {25, A0, 0, 0, 0},
};

#define VBATPIN A7
#define DONEPIN 12

Bluetooth bt;

using namespace ArduinoJson;

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(VBATPIN, INPUT);
  pinMode(DONEPIN, OUTPUT);
  digitalWrite(DONEPIN, LOW);
  for (const auto device : devices)
  {
    pinMode(device.pump, OUTPUT);
    pinMode(device.sensor, INPUT);
    digitalWrite(device.pump, LOW);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  bt.init();
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop()
{
  auto msg = bt.readMessage();
  if (msg.length())
  {
    Serial.println(msg);
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, msg) == DeserializationError::Ok)
    {
      const String command = doc["command"];
      if (command.equals("led"))
      {
        const bool val = doc["value"];
        Serial.print(F("Setting LED to "));
        Serial.println(val ? F("on") : F("off"));
        digitalWrite(LED_BUILTIN, val ? HIGH : LOW);
      }
      else if (command.equals("bat"))
      {
        const int val = doc["value"];
        Serial.print(F("Setting battery value to "));
        Serial.println(val);
        bt.updateBatPercent(val);
      }
    }
    else
    {
      Serial.println(F("Unable to decode JSON"));
    }

    // float vbat_mv = readVBAT(VBATPIN);
    // uint8_t vbat_per = mvToPercent(vbat_mv);
    // bt.updateBatPercent(vbat_per);
  }
  delay(100);
  // while (bleuart.available())
  // {
  //   char c = bleuart.read();

  //   if (c == 'b')
  //   {
  //     float vbat_mv = readVBAT(VBATPIN);
  //     uint8_t vbat_per = mvToPercent(vbat_mv);
  //     bleuart.print("LIPO = ");
  //     bleuart.print(vbat_mv);
  //     bleuart.print(" mV (");
  //     bleuart.print(vbat_per);
  //     bleuart.println("%)");
  //   }
  //   else if (c == 'd')
  //   {
  //     bleuart.println("Done");
  //     delay(200);
  //     digitalWrite(DONEPIN, HIGH);
  //   }
  //   else if (c == 'x' || c == 'o' || c == 'm')
  //   {
  //     char d = bleuart.read() - 48;
  //     if (d < 0 || d >= 7)
  //     {
  //       continue;
  //     }
  //     const auto &device = devices[d];
  //     if (c == 'x')
  //     {
  //       bleuart.print("Activating: ");
  //       bleuart.println((int)d);
  //       digitalWrite(device.pump, HIGH);
  //     }
  //     else if (c == 'o')
  //     {
  //       bleuart.print("Dectivating: ");
  //       bleuart.println((int)d);
  //       digitalWrite(device.pump, LOW);
  //     }
  //     else if (c == 'm')
  //     {
  //       bleuart.print("Measuring: ");
  //       bleuart.println((int)d);
  //       bleuart.print("Value: ");
  //       bleuart.println(analogRead(device.sensor));
  //     }
  //   }
  // }
}
