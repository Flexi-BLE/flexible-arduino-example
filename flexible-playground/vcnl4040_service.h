#ifndef VCNL4040_SERVICE_H
#define VCNL4040_SERVICE_H

#include <Arduino.h>
#include <bluefruit.h>
#include <Adafruit_VCNL4040.h>

const uint8_t vncl4040ServiceUuid128[] =          { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x01, 0x00, 0x24, 0x1a };
const uint8_t vncl4040ServiceDataChrUuid128[] =   { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x02, 0x00, 0x24, 0x1a };
const uint8_t vncl4040ServiceConfigChrUuid128[] = { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x03, 0x00, 0x24, 0x1a };

BLEService vncl4040Service = BLEService(vncl4040ServiceUuid128);
BLECharacteristic vncl4040DataChr = BLECharacteristic(vncl4040ServiceDataChrUuid128);
BLECharacteristic vncl4040ConfigChr = BLECharacteristic(vncl4040ServiceConfigChrUuid128);

Adafruit_VCNL4040 vcnl4040 = Adafruit_VCNL4040();

/*
  config: 
    byte 0: sensor state
      - 0=Off
      - 1=On
    byte 1-2: desired frequency 
      - Range: 1 - 250Hz
    byte 3: LED Current:
      - 0=50mA (default)
      - 1=75mA
      - 2=100mA
      - 3=120mA
      - 4=140mA
      - 5=160mA
      - 6=180mA
      - 7=200mA
    byte 4: LED Duty Cycle:
      - 0=1/40 (default)
      - 1=1/80
      - 2=1/160
      - 3=1/320
    byte 5: Ambient Integration Time:
      - 0=80ms (default)
      - 1=160ms
      - 2=320ms
      - 3=640ms
    byte 6: Proximty Integration Time:
      - 0:1T (default)
      - 1:1.5T
      - 2:2T
      - 3:2.5T
      - 4:3T
      - 5:3.5T
      - 6:4T
      - 7:8T
    byte 7: High Resolution:
     - 0:False
     - 1:True (default)
*/
const uint8_t vncl4040ConfigDataLen = 8;
uint8_t vncl4040ConfigData[vncl4040ConfigDataLen] = { 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8_t vncl4040DataLen = 240;
uint8_t vncl4040DataBuf[vncl4040DataLen] = { 0x00 };

uint8_t vncl4040Cursor = 0;
uint32_t vncl4040LastRecordedMS = millis();

bool isVNCL4040ServiceEnabled(void) {
  return vncl4040ConfigData[0] == 1;
}

uint16_t vncl4040DesiredFrequencyMS(void) {
  uint16_t desiredFreqHZ = (vncl4040ConfigData[2] | vncl4040ConfigData[1] << 8);
  return 1000 / desiredFreqHZ;
}

uint8_t vncl4040DesiredLedCurrent(void) {
  return vncl4040ConfigData[3];
}

uint8_t vncl4040DesiredDutyCycle(void) {
  return vncl4040ConfigData[4];
}

uint8_t vncl4040DesiredAmbientIntegrationTime(void) {
  return vncl4040ConfigData[5];
}

uint8_t vncl4040DesiredProximityIntegrationTime(void) {
  return vncl4040ConfigData[6];
}

bool vncl4040DesiredResolution(void) {
  return (bool)vncl4040ConfigData[7];
}

void setupVNCL4040Sensor() {
  if (!vcnl4040.begin()) {
    Serial.println("Couldn't find VCNL4040 chip");
    while (1);
  }
  Serial.println("Found VCNL4040 chip");
}

void vncl_config_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  (void) conn_hdl;
  (void) chr;

  if (len == vncl4040ConfigDataLen) {
    Serial.println("Did write vncl4040 service configuration: ");

    if ( vncl4040ConfigData[0] != data[0] ) {
      // Serial.println(" - VNCL4040 Sensor State: %d -> %d", vncl4040ConfigData[0], data[0]);             
    }

    if ( vncl4040ConfigData[1] != data[1] || vncl4040ConfigData[2] != data[2]) {
      // Serial.println(" - VNCL4040 Frequency: %d -> %d", (uint16_t)(vncl4040ConfigData[1] << 8 | vncl4040ConfigData[2]), (uint16_t)(data[1] << 8 | data[2]));
    }

    if ( vncl4040ConfigData[3] != data[3] ) {
      // Serial.println(" - VNCL4040 LED Current: %d -> %d: ", vncl4040ConfigData[3], data[3]);
      vcnl4040.setProximityLEDCurrent((VCNL4040_LEDCurrent)data[3]);
    }

    if ( vncl4040ConfigData[4] != data[4] ) {
      // Serial.println(" - VNCL4040 LED Duty Cycle: %d -> %d: ", vncl4040ConfigData[4], data[4]);
      vcnl4040.setProximityLEDDutyCycle((VCNL4040_LEDDutyCycle)data[4]);
    }

    if ( vncl4040ConfigData[5] != data[5] ) {
      // Serial.println(" - VNCL4040 Ambient Integration Time: %d -> %d: ", vncl4040ConfigData[5], data[5]);
      vcnl4040.setAmbientIntegrationTime((VCNL4040_AmbientIntegration)data[5]);
    }

    if ( vncl4040ConfigData[6] != data[6] ) {
      // Serial.println(" - VNCL4040 Proximity Integration Time: %d -> %d: ", vncl4040ConfigData[6], data[6]);
      vcnl4040.setProximityIntegrationTime((VCNL4040_ProximityIntegration)data[6]);
    }

    if ( vncl4040ConfigData[7] != data[7] ) {
      // Serial.println(" - VNCL4040 Desired Resolution: %d -> %d: ", vncl4040ConfigData[7], data[7]);
      vcnl4040.setProximityHighResolution((bool)data[7]);
    }

    for (uint8_t i = 0; i < len; i++) {
      vncl4040ConfigData[i] = data[i];
    }
  }
}

void vncl_data_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  (void) conn_hdl;
  (void) chr;

  Serial.println("Did write vncl4040 service data");

  if (len == vncl4040ConfigDataLen) {
    for (uint8_t i = 0; i < len; i++) {
      vncl4040DataBuf[i] = data[i];     
    }
  }
}

void setupVNCL4040Service() 
{
  vncl4040Service.begin();

  vncl4040ConfigChr.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  vncl4040ConfigChr.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  vncl4040ConfigChr.setFixedLen(vncl4040ConfigDataLen);
  vncl4040ConfigChr.setWriteCallback(vncl_config_write_callback);
  vncl4040ConfigChr.begin();
  vncl4040ConfigChr.write(vncl4040ConfigData, vncl4040ConfigDataLen);

  vncl4040DataChr.setProperties(CHR_PROPS_NOTIFY);
  vncl4040DataChr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  vncl4040DataChr.setFixedLen(vncl4040DataLen);
  vncl4040DataChr.begin();
  vncl4040DataChr.notify(vncl4040DataBuf, vncl4040DataLen);
}

void _recordProximity(uint16_t proximity, uint16_t ambientLight, uint16_t rawWhiteLight, uint32_t referenceTimeMS)
{ 
  if (!isVNCL4040ServiceEnabled()) {
    return;
  }
  
  uint32_t recordOffset;

  if ( vncl4040Cursor > (vncl4040DataLen - 7))
  {
    if (vncl4040DataChr.notifyEnabled())
    {
      Serial.println("Sending VNCL4040 Data");

      vncl4040DataChr.notify(vncl4040DataBuf, vncl4040DataLen);
    }    
    vncl4040Cursor = 0;   
  }

  if ( vncl4040Cursor == 0 ) {
    // write the anchor time in the data buffer
    uint32_t anchorMS = millis() - referenceTimeMS;
    vncl4040DataBuf[vncl4040Cursor++] = (anchorMS) & 0xFF;
    vncl4040DataBuf[vncl4040Cursor++] = (anchorMS >> 8) & 0xFF;
    vncl4040DataBuf[vncl4040Cursor++] = (anchorMS >> 16) & 0xFF;
    vncl4040DataBuf[vncl4040Cursor++] = (anchorMS >> 24) & 0xFF;

    Serial.print("Timestamp: ");
    Serial.println(anchorMS);

    recordOffset = 0;
  } else {
    recordOffset = millis() - vncl4040LastRecordedMS;    
  }
    
  // generate and write value
  vncl4040LastRecordedMS = millis();
  vncl4040DataBuf[vncl4040Cursor++] = (proximity >> 8) & 0xff;
  vncl4040DataBuf[vncl4040Cursor++] = (proximity) & 0xff;
  vncl4040DataBuf[vncl4040Cursor++] = (ambientLight >> 8) & 0xff;
  vncl4040DataBuf[vncl4040Cursor++] = (ambientLight) & 0xff;
  vncl4040DataBuf[vncl4040Cursor++] = (rawWhiteLight >> 8) & 0xff;
  vncl4040DataBuf[vncl4040Cursor++] = (rawWhiteLight) & 0xff;
  vncl4040DataBuf[vncl4040Cursor++] = (uint8_t)recordOffset;
}

void vncl4040LoopCall(uint32_t referenceTimeMS) { 
  // Serial.print("next time: "); Serial.println(vncl4040LastRecordedMS + vncl4040DesiredFrequencyMS());
  // Serial.print("current time: "); Serial.println(millis());
  if ( isVNCL4040ServiceEnabled() && (vncl4040LastRecordedMS + vncl4040DesiredFrequencyMS()) < millis() ) {
    Serial.println("recording prox");
    Serial.println(millis());
    uint16_t proximity = vcnl4040.getProximity();
    Serial.print("Proximity: ");
    Serial.println(proximity);
    uint16_t lux = vcnl4040.getLux();
    uint16_t whiteLight = vcnl4040.getWhiteLight();
    _recordProximity(proximity, lux, whiteLight, referenceTimeMS);
    Serial.println(millis());
  }
}

void checkAndPrintVCL4040(void) {
   //vcnl4040.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
  Serial.print("Proximity LED current set to: ");
  switch(vcnl4040.getProximityLEDCurrent()) {
    case VCNL4040_LED_CURRENT_50MA: Serial.println("50 mA"); break;
    case VCNL4040_LED_CURRENT_75MA: Serial.println("75 mA"); break;
    case VCNL4040_LED_CURRENT_100MA: Serial.println("100 mA"); break;
    case VCNL4040_LED_CURRENT_120MA: Serial.println("120 mA"); break;
    case VCNL4040_LED_CURRENT_140MA: Serial.println("140 mA"); break;
    case VCNL4040_LED_CURRENT_160MA: Serial.println("160 mA"); break;
    case VCNL4040_LED_CURRENT_180MA: Serial.println("180 mA"); break;
    case VCNL4040_LED_CURRENT_200MA: Serial.println("200 mA"); break;
  }
  
  //vcnl4040.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);
  Serial.print("Proximity LED duty cycle set to: ");
  switch(vcnl4040.getProximityLEDDutyCycle()) {
    case VCNL4040_LED_DUTY_1_40: Serial.println("1/40"); break;
    case VCNL4040_LED_DUTY_1_80: Serial.println("1/80"); break;
    case VCNL4040_LED_DUTY_1_160: Serial.println("1/160"); break;
    case VCNL4040_LED_DUTY_1_320: Serial.println("1/320"); break;
  }

  //vcnl4040.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
  Serial.print("Ambient light integration time set to: ");
  switch(vcnl4040.getAmbientIntegrationTime()) {
    case VCNL4040_AMBIENT_INTEGRATION_TIME_80MS: Serial.println("80 ms"); break;
    case VCNL4040_AMBIENT_INTEGRATION_TIME_160MS: Serial.println("160 ms"); break;
    case VCNL4040_AMBIENT_INTEGRATION_TIME_320MS: Serial.println("320 ms"); break;
    case VCNL4040_AMBIENT_INTEGRATION_TIME_640MS: Serial.println("640 ms"); break;
  }


  //vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
  Serial.print("Proximity integration time set to: ");
  switch(vcnl4040.getProximityIntegrationTime()) {
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_1T: Serial.println("1T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_1_5T: Serial.println("1.5T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_2T: Serial.println("2T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_2_5T: Serial.println("2.5T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_3T: Serial.println("3T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_3_5T: Serial.println("3.5T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_4T: Serial.println("4T"); break;
    case VCNL4040_PROXIMITY_INTEGRATION_TIME_8T: Serial.println("8T"); break;
  }

  //vcnl4040.setProximityHighResolution(false);
  Serial.print("Proximity measurement high resolution? ");
  Serial.println(vcnl4040.getProximityHighResolution() ? "True" : "False");

  Serial.println("");
}


#endif // VCNL4040_SERVICE_H