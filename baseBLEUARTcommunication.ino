
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_NeoPixel.h>

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
// BLE Service

BLEDfu bledfu;    //OTA DFU Service
BLEDis bledis;    //device information
BLEUart bleuart;  //uart over ble
BLEBas blebas;    //battery

//password setup

const char* PASSWORD = "mypassword";
const int MAXLENGTH = 64;

void setup() 
{ 
  Serial.begin(115200);

  // CFG_DEBUG = 1;

  while(!Serial) delay(10);

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
  Serial.print("Advertising is started: ");
  Serial.println("CLUE");
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

bool firstLoop = true;

bool verified = false;


void loop()
{
  if (firstLoop && Bluefruit.connected() && bleuart.notifyEnabled() )
  {
     digitalToggle(LED_BUILTIN);
     firstLoop = false;   
     neopixel.clear();
     neopixel.setPixelColor(0, neopixel.Color(0,150,0));
     neopixel.show();
     char printout[47] = "Welcome to the High Voltage Password Manager.\n";
     centralOutput(printout);
     char printout2[22] = "Enter the password: \n";
     centralOutput(printout2);
     char empty[1] = "";
     centralOutput(empty);  
  
     while (!verified)
     {
       char userInput[strlen(PASSWORD)];
       int index = 0;

       while (index < strlen(PASSWORD))
        {
         if (bleuart.available())
          {
            char c = bleuart.read();
            userInput[index] = c;
            index++;
            
          }
        }
      Serial.println(userInput);
      if (strcmp(userInput, PASSWORD) == 0)
        {
          centralOutput(userInput);
          bleuart.println("Password correct!");
          verified = true;
        }
      else
        {
         centralOutput(userInput); 
         bleuart.println("Password incorrect!");
        }
     }
  }
  if (!firstLoop && !Bluefruit.connected() )
  {
    digitalToggle(LED_BUILTIN);
    firstLoop = true;
    verified = false;
    neopixel.clear();
    neopixel.setPixelColor(0, neopixel.Color(0,0,0));
    neopixel.show();
  }
  

}


// callback invoked when central connects
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

void centralOutput(char out[MAXLENGTH]){
  /*
   * Output the out string to display on Central using UART(via Bluetooth)
   */
  bleuart.print(out);
}

void encrypt(char password[], int key){
  for(int i=0;i,strlen(password);i++){
    password[i] = password[i] - key;
  }
}
