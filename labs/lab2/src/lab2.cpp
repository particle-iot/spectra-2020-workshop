/*
 * Project: Spectra 2020 Workshop - Lab 2
 * Description: Source for Lab 2 of the 2020 Spectra Workshop
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: March 3rd, 2020
 */
#include "Particle.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Grove_ChainableLED.h"

DHT dht(D2);
ChainableLED leds(A4, A5, 1);

int toggleLed(String args);

int temp, humidity;
double temp_dbl, humidity_dbl;
double currentLightLevel;

void setup()
{
  Serial.begin(9600);

  dht.begin();

  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  pinMode(A0, INPUT);

  Particle.variable("temp", temp_dbl);
  Particle.variable("humidity", humidity_dbl);

  Particle.function("toggleLed", toggleLed);
}

void loop()
{
  temp = dht.getTempFarenheit();
  humidity = dht.getHumidity();

  temp_dbl = temp;
  humidity_dbl = humidity;

  Serial.printlnf("Temp: %f", temp);
  Serial.printlnf("Humidity: %f", humidity);

  double lightAnalogVal = analogRead(A0);
  currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

  if (currentLightLevel > 50)
  {
    Particle.publish("light-meter/level", String(currentLightLevel), PRIVATE);
  }

  delay(5000);
}

int toggleLed(String args)
{
  leds.setColorHSB(0, 0.0, 1.0, 0.5);

  delay(1000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}