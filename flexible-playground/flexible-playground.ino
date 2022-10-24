#include <bluefruit.h>
#include <Adafruit_AS7341.h>
#include <Adafruit_VCNL4040.h>

#include "ble.h"
#include "info_service.h"
// #include "vcnl4040_service.h"
/*
  #########################
  ### ARDUINO LIFECYCLE ###
  #########################
*/

void setup() {
  Serial.begin(115200);
  Serial.println("FlexiBLE nRF Bluefruit Example");
  Serial.println("-------------------------\n");

  // setupVNCL4040Sensor();

  setupBluefruit();
}

void loop() {
  while (!isConnected) {
    Serial.println("");

    Serial.println("waiting for ble connection");
    delay(1000); 
  }

  while (!isEpochSet) {
    Serial.println("waiting for set of epoch time");
    delay(1000);    
  }

  uint32_t lastLoopMS = millis();
  
  while (isEpochSet && isConnected) {
    // vncl4040LoopCall(referenceTimeMS);
    

    recordRandomNumber(0, referenceTimeMS);
    recordRandomNumber(1, referenceTimeMS);
    recordRandomNumber(2, referenceTimeMS);
    delay(randomNumberDesiredFrequencyMS());

    // printf(
    //   "random number multiselect %d, %d, %d, %d, %d, %d\n",
    //   (randomNumberConfigData[3] & 1) > 0,
    //   (randomNumberConfigData[3] & 2) > 0,
    //   (randomNumberConfigData[3] & 4) > 0,
    //   (randomNumberConfigData[3] & 8) > 0,
    //   (randomNumberConfigData[3] & 16) > 0,
    //   (randomNumberConfigData[3] & 32) > 0
    // );
  }
}
