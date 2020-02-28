/*
 * Project lab2
 * Description: 
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: July 22, 2019
 */
#include "Particle.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Grove_ChainableLED.h"
#include "DiagnosticsHelperRK.h"

SYSTEM_THREAD(ENABLED);

const unsigned long UPDATE_INTERVAL = 2000;
unsigned long lastUpdate = 0;

DHT dht(D2);
ChainableLED leds(A4, A5, 1);

int toggleLed(String args);

int temp, humidity;
double currentLightLevel;

void setup()
{
  Serial.begin(9600);

  dht.begin();

  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  pinMode(A0, INPUT);

  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);

  Particle.function("toggleLed", toggleLed);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
  {
    lastUpdate = millis();

    temp = (int)dht.getTempFarenheit();
    humidity = (int)dht.getHumidity();

    Serial.printlnf("Temp: %f", temp);
    Serial.printlnf("Humidity: %f", humidity);

    double lightAnalogVal = analogRead(A0);
    currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

    if (currentLightLevel > 50)
    {
      Particle.publish("light-meter/level", String(currentLightLevel), PRIVATE);
    }
  }
}

int toggleLed(String args)
{
  leds.setColorHSB(0, 0.0, 1.0, 0.5);

  delay(1000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}