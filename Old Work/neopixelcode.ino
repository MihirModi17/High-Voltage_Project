/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

// This sketch is intended to be used with the NeoPixel control
// surface in Adafruit's Bluefruit LE Connect mobile application.
//
// - Compile and flash this sketch to the nRF52 Feather
// - Open the Bluefruit LE Connect app
// - Switch to the NeoPixel utility
// - Click the 'connect' button to establish a connection and
//   send the meta-data about the pixel layout
// - Use the NeoPixel utility to update the pixels on your device

/* NOTE: This sketch required at least version 1.1.0 of Adafruit_Neopixel !!! */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <bluefruit.h>

#define NEOPIXEL_VERSION_STRING "Neopixel v2.0"

/* Pin used to drive the NeoPixels */
#define PIN     18


#define MAXCOMPONENTS  4
uint8_t *pixelBuffer = NULL;
uint8_t width = 0;
uint8_t height = 0;
uint8_t stride;
uint8_t componentsValue;
bool is400Hz;
uint8_t components = 3;     // only 3 and 4 are valid values

Adafruit_NeoPixel neopixel(1,18);
BLEDfu bledfu;    //OTA DFU Service
BLEDis bledis;    //device information
BLEUart bleuart;  //uart over ble
BLEBas blebas;    //battery


void setup() 
{ 
  Serial.begin(115200);

  // CFG_DEBUG = 1;

 #if CFG_DEBUG
  //Blocking wait for connection when debug mode is enabled via IDE
  while (!Serial) yield();
 #endif

  //Configures the BLE LED to be enabled on Connection, typically default behavior
  Bluefruit.autoConnLed(true);
  //Bluefruit.setAppearance(100);
  neopixel.begin();
  //Serial.println(Bluefruit.getAppearance());
  
  //Config the peripheral connection with maximum bandwidth
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  //Bluefruit.setName(getMcuUniqueID());//useful for testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  //to be consistent, add OTA DFU first if it exists
  bledfu.begin();

  //Config and start Device Info Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  //Config and start BLEUART services
  bleuart.begin();

  //Config and start BLE Battery service
  blebas.begin();
  blebas.write(100);

  startAdv();

  //Can print lines to serial if you want to
}

void startAdv(void)
{

  //Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary scan packet since there is no room for 'Name' in advertising packet
  Bluefruit.ScanResponse.addName();

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

void loop()
{
  // Echo received data
  if ( Bluefruit.connected() && bleuart.notifyEnabled() )
  {
   neopixel.clear();
   neopixel.setPixelColor(0, neopixel.Color(0,150,0));
   neopixel.show();
   delay(500);
      }
      if(!Bluefruit.connected())
      {
        neopixel.clear();
        neopixel.setPixelColor(0, neopixel.Color(0,0,0));
        neopixel.show();
      }
} 
 
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
   
  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));
  
  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
