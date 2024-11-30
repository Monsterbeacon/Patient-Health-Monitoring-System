#include <LiquidCrystal_I2C.h> // Include the I2C library
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Create an LCD object with the I2C address of your converter
LiquidCrystal_I2C lcd(0x27, 16, 2); // Replace 0x27 with the actual address if different

#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celsius = 0;
float Fahrenheit = 0;

#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected() {
  Serial.println("Beat!");
}

void setup() {
  Serial.begin(9600);
  sensors.begin(); // Initialize temperature sensor
  Serial.print("Initializing pulse oximeter and temperature sensor..");
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.print("Initializing...");
  delay(1000);
  lcd.clear();

  // Initialize the PulseOximeter instance
  if (!pox.begin()) {
    Serial.println("FAILED");
    for(;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  sensors.requestTemperatures(); // Update temperature sensor

  Celsius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celsius);
  
  Serial.print("Temperature: ");
  Serial.print(Celsius);
  Serial.print(" °C / ");
  Serial.print(Fahrenheit);
  Serial.println(" °F");

  pox.update(); // Update pulse oximeter
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(Fahrenheit);
    lcd.print(" °F");

    lcd.setCursor(0, 1);
    lcd.print("HR: ");
    lcd.print(pox.getHeartRate());
    lcd.print(" bpm / ");
    lcd.print("SpO2: ");
    lcd.print(pox.getSpO2());
    lcd.print(" %");

    tsLastReport = millis();
    delay(1000);
  }
}