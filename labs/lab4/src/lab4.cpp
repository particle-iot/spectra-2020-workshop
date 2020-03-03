/*
 * Project: Spectra 2020 Workshop - Lab 4
 * Description: Source for Lab 4 of the 2020 Spectra Workshop
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: March 3rd, 2020
 */
#include "Particle.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Grove_ChainableLED.h"
#include "JsonParserGeneratorRK.h"
#include "DiagnosticsHelperRK.h"

DHT dht(D2);
ChainableLED leds(A4, A5, 1);

SYSTEM_THREAD(ENABLED);

bool timeToSleep;

// Private battery and power service UUID
const BleUuid serviceUuid("5c1b9a0d-b5be-4a40-8f7a-66b36d0a5176");

BleCharacteristic uptimeCharacteristic("uptime", BleCharacteristicProperty::NOTIFY, BleUuid("fdcf4a3f-3fed-4ed2-84e6-04bbb9ae04d4"), serviceUuid);
BleCharacteristic signalStrengthCharacteristic("strength", BleCharacteristicProperty::NOTIFY, BleUuid("cc97c20c-5822-4800-ade5-1f661d2133ee"), serviceUuid);
BleCharacteristic freeMemoryCharacteristic("freeMemory", BleCharacteristicProperty::NOTIFY, BleUuid("d2b26bf3-9792-42fc-9e8a-41f6107df04c"), serviceUuid);

int toggleLed(String args);
void createEventPayload(int temp, int humidity, double light);
void readSensors();
void button_handler(system_event_t event, int duration, void*);
void configureBLE();

int temp, humidity;
double temp_dbl, humidity_dbl;
double currentLightLevel;

const unsigned long UPDATE_INTERVAL = 30000;
unsigned long lastUpdate = 0;

void button_handler(system_event_t event, int duration, void*)
{
  if (!duration) {
    digitalWrite(D7, HIGH);
  }  else {
    digitalWrite(D7, LOW);

    timeToSleep = true;
  }
}

void configureBLE()
{
  BLE.addCharacteristic(uptimeCharacteristic);
  BLE.addCharacteristic(signalStrengthCharacteristic);
  BLE.addCharacteristic(freeMemoryCharacteristic);

  BleAdvertisingData advData;

  // Advertise our private service only
  advData.appendServiceUUID(serviceUuid);

  // Continuously advertise when not connected
  BLE.advertise(&advData);
}

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

  Particle.publishVitals(10);

  configureBLE();

  pinMode(D7, OUTPUT);
  pinMode(D4, INPUT_PULLUP);

  System.on(button_status, button_handler);
}

void loop()
{
  if (timeToSleep) 
  {
    timeToSleep = false;
    // System.sleep(WKP, RISING, 20);
    System.sleep(D4, RISING);
  }

  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
  {
    lastUpdate = millis();

    temp = dht.getTempFarenheit();
    humidity = dht.getHumidity();

    temp_dbl = temp;
    humidity_dbl = humidity;

    Serial.printlnf("Temp: %f", temp);
    Serial.printlnf("Humidity: %f", humidity);

    double lightAnalogVal = analogRead(A0);
    currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

    createEventPayload(temp, humidity, currentLightLevel);

    if (currentLightLevel > 50)
    {
      Particle.publish("light-meter/level", String(currentLightLevel), PRIVATE);
    }

    if (BLE.connected())
    {
      uint8_t uptime = (uint8_t)DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_UPTIME);
      uptimeCharacteristic.setValue(uptime);

      uint8_t signalStrength = (uint8_t)(DiagnosticsHelper::getValue(DIAG_ID_NETWORK_SIGNAL_STRENGTH) >> 8);
      signalStrengthCharacteristic.setValue(signalStrength);

      int32_t usedRAM = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_USED_RAM);
      int32_t totalRAM = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_TOTAL_RAM);
      int32_t freeMem = (totalRAM - usedRAM);
      freeMemoryCharacteristic.setValue(freeMem);
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

void readSensors()
{
  temp = (int)dht.getTempFarenheit();
  humidity = (int)dht.getHumidity();

  double lightAnalogVal = analogRead(A0);
  currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

  createEventPayload(temp, humidity, currentLightLevel);
}

void createEventPayload(int temp, int humidity, double light)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("temp", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("light", light);
  }

  Particle.publish("env-vals", jw.getBuffer(), PRIVATE);
}