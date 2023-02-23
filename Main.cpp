#include <Arduino.h>
#include <ThingerESP32.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_LTR329_LTR303.h"

//                                USERNAME, DEVICE_ID, DEVICE_CREDENTIAL)
ThingerESP32 thing = ThingerESP32("Kozter001", "ESP32-S3", "Password123");
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_LTR329 ltr = Adafruit_LTR329();
unsigned long lastMillies = 0;
float lastTemp = 0;
float lastHum = 0;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
//               SSID, SSID_PASSWORD
  thing.add_wifi("Google", "Nussi123");

    if (! sht31.begin(0x44)) 
    {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
    }

    if ( ! ltr.begin() ) 
    {
      Serial.println("Couldn't find LTR sensor!");
      while (1) delay(10);
    }
    // Setup LTR sensor (see advanced demo in library for all options!)
    Serial.println("Found LTR sensor!");
    ltr.setGain(LTR3XX_GAIN_4);
    ltr.setIntegrationTime(LTR3XX_INTEGTIME_50);
    ltr.setMeasurementRate(LTR3XX_MEASRATE_50);
  }

void loop() 
{
  thing.handle();
  if (millis() - lastMillies > 10000)
  {
    lastMillies = millis();
    float t = sht31.readTemperature();
    if (! isnan(t))   
    { // check if 'is not a number'
      Serial.print("Temp *C = "); 
      Serial.print(t); 
      Serial.print("\t\t");
      thing["temperature"] >> [&t](pson& out)
      {
        out = t;
      };
      lastTemp = t;
    }  
    else 
    { 
      Serial.println("Failed to read temperature");
      t = lastTemp;
    }

    float h = sht31.readHumidity();
    if (! isnan(h)) 
    { // check if 'is not a number'
      Serial.print("Hum. % = "); 
      Serial.println(h);
      thing["humidity"] >> [&h](pson& out)
      {
        out = h;
      };
      lastHum = h;
    }
    else 
    { 
      Serial.println("Failed to read humidity");
      h = lastHum;
    }

    uint16_t visible_plus_ir, infrared;
    if (ltr.newDataAvailable()) {
      bool valid = ltr.readBothChannels(visible_plus_ir, infrared);
      if (valid) 
      {
        Serial.print("CH0 Visible + IR: ");
        Serial.print(visible_plus_ir);
        Serial.print("\tCH1 Infrared: ");
        Serial.println(infrared); 
        thing["light"] >> [=](pson& out) {
          out["visible_plus_ir"] = visible_plus_ir;
          out["infrared"] = infrared;
        };
      }
    }
  }
  delay(1000);
}
