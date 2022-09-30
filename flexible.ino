#include <bluefruit.h>

const uint8_t infoServiceUuid128 = { };
BLEService infoService = BLEService(BLEuuid()

)

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
  // TODO:


  Serial.println("Setup and Start Advertising");
  startAdv();
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // TODO: add INFO SERVICE

  Bluefruit.ScanResponse.addName();  

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);
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
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
}

void loop() {
  

}
