/* Edge Impulse ingestion SDK
   Copyright (c) 2022 EdgeImpulse Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

/* Includes ---------------------------------------------------------------- */
#include <Lab_Monitoring_-_Anomaly_Detection_inferencing.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>

SensirionI2CSht4x sht4x;
#define PIN        6
#define NUMPIXELS 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

static const float features[] = {
  // copy raw features here (for example from the 'Live classification' page)
  // see https://docs.edgeimpulse.com/docs/running-your-impulse-arduino
};

/**
   @brief      Copy raw feature data in out_ptr
               Function called by inference library

   @param[in]  offset   The offset
   @param[in]  length   The length
   @param      out_ptr  The out pointer

   @return     0
*/
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}


/**
   @brief      Arduino setup function
*/
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // comment out the below line to cancel the wait for USB connection (needed for native USB)
  while (!Serial);
  Serial.println("Edge Impulse Inferencing Demo");
  pixels.begin();
  Wire.begin();

  uint16_t error;
  char errorMessage[256];

  sht4x.begin(Wire);

  uint32_t serialNumber;
  error = sht4x.serialNumber(serialNumber);
  if (error) {
    Serial.print("Error trying to execute serialNumber(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.print("Serial Number: ");
    Serial.println(serialNumber);
  }
}

/**
   @brief      Arduino main function
*/
void loop()
{
  ei_printf("Edge Impulse standalone inferencing (Arduino)\n");
  uint16_t error;
  char errorMessage[256];

  delay(1000);

  float temperature;
  float humidity;
  error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    Serial.print("Error trying to execute measureHighPrecision(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.print("Temperature:");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print("Humidity:");
    Serial.println(humidity);
  }
  pixels.clear();

  if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
    ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
              EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
    delay(1000);
    return;
  }

  ei_impulse_result_t result = { 0 };

  // the features are stored into flash, and we don't want to load everything into RAM
  signal_t features_signal;
  features_signal.total_length = sizeof(features) / sizeof(features[0]);
  features_signal.get_data = &raw_feature_get_data;

  // invoke the impulse
  EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
  ei_printf("run_classifier returned: %d\n", res);

  if (res != 0) return;

  // print the predictions
  ei_printf("Predictions ");
  ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
  ei_printf(": \n");
  ei_printf("[");
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
    ei_printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf(", ");
#else
    if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
      ei_printf(", ");
    }
#endif
  }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
  ei_printf("%.3f", result.anomaly);
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // Here we're using a moderately bright red color:
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); //RED
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(5); // Pause before next pass through loop
  }
#endif
  ei_printf("]\n");
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));  //GREEN
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(5); // Pause before next pass through loop
  }

  // human-readable predictions
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
    ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
  }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
  ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

  delay(1000);
}
