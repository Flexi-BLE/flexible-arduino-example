#include <bluefruit.h>
#include <Adafruit_AS7341.h>

const char deviceName[] = "flexible-example 01";

/* INFO SERVICE: 1a220001-c2ed-4d11-ad1e-fc06d8a02d37 (REQUIRED)

*/
const uint8_t infoServiceUuid128[] =               { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x01, 0x00, 0x22, 0x1a };
const uint8_t infoServiceEpochChrUuid128[] =       { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x02, 0x00, 0x22, 0x1a };
const uint8_t infoServiceSpecVersionChrUuid128[] = { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x03, 0x00, 0x22, 0x1a };
const uint8_t infoServiceSpecIdChrUuid128[] =      { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x04, 0x00, 0x22, 0x1a };
BLEService infoService = BLEService(infoServiceUuid128);
BLECharacteristic epochChr = BLECharacteristic(infoServiceEpochChrUuid128);
BLECharacteristic specVersionChr = BLECharacteristic(infoServiceSpecVersionChrUuid128);
BLECharacteristic specIdChr = BLECharacteristic(infoServiceSpecIdChrUuid128);
const uint8_t epochDataLen = 8;
uint8_t epochDataBuf[epochDataLen] = { 0x00 };

bool isEpochSet = false;
bool isConnected = false;
uint32_t referenceTimeMS = 0;

/* RANDOM NUMBER SERVICE: 1a230001-c2ed-4d11-ad1e-fc06d8a02d37 (OPTIONAL - Service per Data Stream)

*/
const uint8_t randomNumberServiceUuid128[] =          { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x01, 0x00, 0x23, 0x1a };
const uint8_t randomNumberServiceDataChrUuid128[] =   { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x02, 0x00, 0x23, 0x1a };
const uint8_t randomNumberServiceConfigChrUuid128[] = { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x03, 0x00, 0x23, 0x1a };
BLEService randomNumberService = BLEService(randomNumberServiceUuid128);
BLECharacteristic randomNumberDataChr = BLECharacteristic(randomNumberServiceDataChrUuid128);
BLECharacteristic randomNumberConfigChr = BLECharacteristic(randomNumberServiceConfigChrUuid128);
const uint8_t randomNumberConfigDataLen = 4;
uint8_t randomNumberConfigData[randomNumberConfigDataLen] = { 0x00, 0x00, 0x20, 0x00 };
const uint8_t randomNumberDataLen = 240;
uint8_t randomNumberDataBuf[randomNumberDataLen] = { 0x00 };

uint8_t rnCursor = 0;
uint32_t rnLastRecordedMS = 0;

/* MULTI SPECTRA SENSOR: 1a230001-c2ed-4d11-ad1e-fc06d8a02d37 (OPTIONAL - Service per Data Stream)

*/
// const uint8_t multSpecServiceUuid128[] =          { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x01, 0x00, 0x24, 0x1a };
// const uint8_t multSpecServiceDataChrUuid128[] =   { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x02, 0x00, 0x24, 0x1a };
// const uint8_t multSpecServiceConfigChrUuid128[] = { 0x37, 0x2d, 0xa0, 0xd8, 0x06, 0xfc, 0x1e, 0xad, 0x11, 0x4d, 0xed, 0xc2, 0x03, 0x00, 0x24, 0x1a };
// BLEService randomNumberService = BLEService(multSpecServiceUuid128);
// BLECharacteristic multSpecDataChr = BLECharacteristic(multSpecServiceDataChrUuid128);
// BLECharacteristic multSpecConfigChr = BLECharacteristic(multSpecServiceConfigChrUuid128);
// const uint8_t multSpecConfigDataLen = 3;
// uint8_t multSpecConfigData[randomNumberConfigDataLen] = { 0x00, 0x00, 0x20 };
// const uint8_t randomNumberDataLen = 240;
// uint8_t randomNumberDataBuf[randomNumberDataLen] = { 0x00 };

bool isRandomNumberDataStreamEnabled()
{
  return randomNumberConfigData[0] == 1;
}

uint16_t randomNumberDesiredFrequencyMS()
{
  uint16_t desiredFreqHZ = randomNumberConfigData[1] << 8 | randomNumberConfigData[2];
  return 1000 / desiredFreqHZ;
}

/* 
   #########################
   ###### BLE SETUP ########
   #########################
*/
void setupInfoService()
{
  infoService.begin();

  epochChr.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  epochChr.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  epochChr.setFixedLen(8);
  epochChr.setWriteCallback(epoch_write_callback);

  epochChr.begin();


  epochChr.write(epochDataBuf, epochDataLen); 

  specVersionChr.setProperties(CHR_PROPS_READ);
  specVersionChr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  specVersionChr.setFixedLen(3);
  specVersionChr.begin();
  uint8_t specVersion[3] = { 0x00, 0x03, 0x05 };
  specVersionChr.write(specVersion, 3);

  specIdChr.setProperties(CHR_PROPS_READ);
  specIdChr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  specIdChr.setFixedLen(24);
  specIdChr.begin();
  uint8_t specId[24] = { 0x61, 0x72, 0x64, 0x75, 0x69, 0x6e, 0x6f, 0x2d, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2d, 0x73, 0x70, 0x65, 0x63, 0x00, 0x00, 0x00, 0x00 };
  specIdChr.write(specId, 24);
}

void setupRandomNumberService() 
{
  randomNumberService.begin();

  randomNumberConfigChr.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  randomNumberConfigChr.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  randomNumberConfigChr.setFixedLen(randomNumberConfigDataLen);
  randomNumberConfigChr.setWriteCallback(rn_config_write_callback);
  randomNumberConfigChr.begin();
  randomNumberConfigChr.write(randomNumberConfigData, randomNumberConfigDataLen);
  // randomNumberConfigChr.setWriteCallback(write_cb_t fp)

  randomNumberDataChr.setProperties(CHR_PROPS_NOTIFY);
  randomNumberDataChr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  randomNumberDataChr.setFixedLen(randomNumberDataLen);
  randomNumberDataChr.begin();
  randomNumberDataChr.notify(randomNumberDataBuf, randomNumberDataLen);
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.setName(deviceName);
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  Bluefruit.Advertising.addService(infoService);

  Bluefruit.ScanResponse.addName();  

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);
}

/* 
   #########################
   #### BLE CALLBACKS ######
   #########################
*/
void epoch_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  (void) conn_hdl;
  (void) chr;
  
  referenceTimeMS = millis();
  Serial.print("Did write epoch time: ");

  if (len == epochDataLen) {
    for (uint8_t i = 0; i < len; i++) {
      epochDataBuf[i] = data[i];
      Serial.print(epochDataBuf[i]);
      Serial.print("\r");
    }
  }
  Serial.println("");

  isEpochSet = true;
}

void rn_config_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  (void) conn_hdl;
  (void) chr;

  Serial.println("Did write random number service configuration");

  if (len == randomNumberConfigDataLen) {
    for (uint8_t i = 0; i < len; i++) {
      randomNumberConfigData[i] = data[i];
    }
  }
}

void rn_data_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  (void) conn_hdl;
  (void) chr;

  Serial.println("Did write random number service data");

  if (len == randomNumberConfigDataLen) {
    for (uint8_t i = 0; i < len; i++) {
      randomNumberDataBuf[i] = data[i];     
    }
  }
}

void cccd_callback(BLECharacteristic& chr, uint16_t cccd_value)
{
  Serial.print("CCCD Updated: ");
  Serial.print(cccd_value);
  Serial.println("");
}

void connect_callback(uint16_t conn_handle)
{
  BLEConnection* conn = Bluefruit.Connection(conn_handle);
  
  char central_name[32] = { 0 };
  conn->getPeerName(central_name, sizeof(central_name));
  
  Serial.print("Connected to ");
  Serial.println(central_name);

  Serial.println("Request PHY Update - 2M");
  conn->requestPHY();

  Serial.println("Request data length update");
  conn->requestDataLengthUpdate();
    
  Serial.println("Request MTU update");
  conn->requestMtuExchange(247);

  delay(1000);

  isConnected = true;
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);

  isConnected = false;
  isEpochSet = false;;
}

/*
  #########################
  ### ARDUINO LIFECYCLE ###
  #########################
*/

void setup() {
  Serial.begin(115200);
  Serial.println("FlexiBLE nRF Bluefruit Example");
  Serial.println("-------------------------\n");

  Serial.println("initializing nRF module");
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(4);  
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Periph.setConnInterval(6, 12);

  Serial.println("Setting up FlexiBLE services");
  setupInfoService();
  setupRandomNumberService();


  Serial.println("Setup and Start Advertising");
  startAdv();
}

void recordRandomNumber(uint8_t group)
{ 
  if (!isRandomNumberDataStreamEnabled()) {
    return;
  }
  
  uint32_t recordOffset;

  if ( rnCursor > (randomNumberDataLen - 3))
  {
    if (randomNumberDataChr.notifyEnabled())
    {
      Serial.println("Sending Random Number Data");

      randomNumberDataChr.notify(randomNumberDataBuf, randomNumberDataLen);
    }    
    rnCursor = 0;   
  }

  if ( rnCursor == 0 ) {
    // write the anchor time in the data buffer
    uint32_t anchorMS = millis() - referenceTimeMS;
    randomNumberDataBuf[rnCursor++] = (anchorMS) & 0xFF;
    randomNumberDataBuf[rnCursor++] = (anchorMS >> 8) & 0xFF;
    randomNumberDataBuf[rnCursor++] = (anchorMS >> 16) & 0xFF;
    randomNumberDataBuf[rnCursor++] = (anchorMS >> 24) & 0xFF;

    Serial.print("Timestamp: ");
    Serial.println(anchorMS);

    recordOffset = 0;
  } else {
    recordOffset = millis() - rnLastRecordedMS;    
  }
    
  // generate and write value
  uint8_t value = (uint8_t)random(255);
  rnLastRecordedMS = millis();
  randomNumberDataBuf[rnCursor++] = value;
  randomNumberDataBuf[rnCursor++] = group;
  randomNumberDataBuf[rnCursor++] = (uint8_t)recordOffset;
  Serial.print("Recording random number -> value: ");
  Serial.print(value);
  Serial.print(" cursor: ");
  Serial.print(rnCursor);
  Serial.print(" offset ms: ");
  Serial.println((uint8_t) recordOffset);
}

void loop() {

  while (!isConnected) {
    Serial.println("waiting for ble connection");
    delay(1000); 
  }

  while (!isEpochSet) {
    Serial.println("waiting for set of epoch time");
    delay(1000);    
  }

  uint32_t lastLoopMS = millis();

  while (isEpochSet && isConnected) {
    recordRandomNumber(0);
    delay(1);
    recordRandomNumber(1);
    delay(1);
    recordRandomNumber(2);
    delay(randomNumberDesiredFrequencyMS());

    printf(
      "random number multiselect %d, %d, %d, %d, %d, %d\n",
      (randomNumberConfigData[3] & 1) > 0,
      (randomNumberConfigData[3] & 2) > 0,
      (randomNumberConfigData[3] & 4) > 0,
      (randomNumberConfigData[3] & 8) > 0,
      (randomNumberConfigData[3] & 16) > 0,
      (randomNumberConfigData[3] & 32) > 0
    );
  }
}
