
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_NeoPixel.h>
#include <dht.h>

#define PIN 2   // input pin Neopixel is attached 
#define NUMPIXELS      16 // number of neopixels in Ring

#define DHT11_PIN 7

int sensorpin = 8;
int buzzer = 9;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
dht DHT;

int delayval = 100; // timing delay

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

int temperature = 0,humidity = 0;
int pressure = 0, altitude = 0;
int gas = 0;

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

//Neopixel, BMP280,  Air sensor, Buzzer

void setup() {
  pixels.begin(); // Initializes the NeoPixel library.
  Serial.begin(115200);
  pinMode(sensorpin,INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.println(F("BMP280 test"));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
  sensor_read();
  // Push the data via USART
  Serial.print("#Temperature = ");
  Serial.print(temperature);
  Serial.print("+Humidity = ");
  Serial.print(humidity);
  Serial.print("+Pressure = ");
  Serial.print(pressure);
  Serial.print("+Altitude = ");
  Serial.print(altitude);
  Serial.print("+Gas = ");
  Serial.print(gas);
  Serial.println("&");
}

void sensor_read()
{
  temp();
  buzcheck();
  bmp280();
  gas_status();
  setColor();
}

void temp()
{
  int chk = DHT.read11(DHT11_PIN);
 // Serial.print("Temp: ");
  temperature = DHT.temperature;
  humidity = DHT.humidity;
 // Serial.print(temperature);
 // Serial.print("C   ");
 // Serial.print(" Humidity: ");
 // Serial.print(DHT.humidity);
 // Serial.println("%");
}

void gas_status()
{
  gas = digitalRead(sensorpin);
}

void buzcheck()
{
  if(temperature > 60) 
  {
    tone(buzzer, 1000); // Send 1KHz sound signal...
    delay(100);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(100); 
  }
  else
    digitalWrite(buzzer,LOW);
}
void bmp280()
{
  pressure = bmp.readPressure();
  altitude = bmp.readAltitude(1013.25);
//    Serial.print(F("Temperature = "));
//    Serial.print(bmp.readTemperature());
//    Serial.println(" *C");
//
//    Serial.print(F("Pressure = "));
//    Serial.print(bmp.readPressure());
//    Serial.println(" Pa");
//
//    Serial.print(F("Approx altitude = "));
//    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
//    Serial.println(" m");
//
//    Serial.println();
//    delay(2000);
}

// setColor()
// picks random values to set for RGB
void setColor() {
//icy - (<12)
//cold 1 - blue(13- 16)
//cold 2 - blue (17-24)
//cold 6 - yellow (25-27)
//cold 7 - bright orange (28-35)
//hottest - red (36& above)

  if (temperature >0 && temperature <=12 ) {
    Serial.println("icy");
    for (int i = 0; i < 5; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 106, 225)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

    }
  }
  
  if (temperature >= 13 && temperature <= 16 ) {
    Serial.println("cold");
    for (int i = 0; i < 7; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 203, 225)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware

    }
  }
  if (temperature >= 17 && temperature <=24 ) {
    Serial.println("cold2");
    for (int i = 0; i < 9; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 225, 148)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

    }
  }
  
  if (temperature >= 25 && temperature <=27 ) {
    Serial.println("warm");
    for (int i = 0; i < 11; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(225, 179, 0)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.


    }
  }
  if (temperature >= 28 && temperature <= 35 ) {
    Serial.println("Sunny");
    for (int i = 0; i < 13; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(225, 62, 0)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.
    }
  }

  if (temperature >= 36 && temperature <=48 ) {
    Serial.println("Hottest");
    for (int i = 0; i < 15; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(225, 0, 0)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.
    }
  }
  //delay(5000);
  delay(100);
  pixels.clear();

}




