#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_NeoPixel.h>

const int MAXLENGTH = 64;
const char* PASSWORD = "hivo23gr11";

void setup() {
  // put your setup code here, to run once:
}

//char password[NUM_ACCOUNTS][MAXLENGTH] = {"Qpw2?*Ae2mQ@T_=r","passwordpassword"}; //Original Passwords For Reference

void loop() {
  // put your main code here, to run repeatedly:
  char n[MAXLENGTH] = "Qpw2?*Ae2mQ@T_=r";
  int KEY = keygen(PASSWORD);
  Serial.print(n);
  Serial.print(" ");

  //Encrypt the array and print to Serial
  encrypt(n, KEY);
  Serial.print("{");
  for(int i=0;i < strlen(n);i++)
  {
    Serial.print(int(n[i]));
    Serial.print(", ");
  }
  Serial.println("'\\0'} ");

  //Code to test if it works
  //char k[MAXLENGTH] = {111, 96, 114, 114, 118, 110, 113, 99, 111, 96, 114, 114, 118, 110, 113, 99, '\0'};
  //decrypt(k, KEY);
  //Serial.print(k);

  Serial.println("");
  delay(1000);
  
}

void encrypt(char password[], int key){
  for(int i=0;i < strlen(password);i++)
  {
    password[i] = password[i] - key;
  }
}

void decrypt(char password[], int key){
  
  for(int i=0; i<strlen(password); i++)
  {
    password[i] = password[i] + key;
  }
}

int keygen(const char pass[]){
  int key = 0;
  for(int i = 0; i < strlen(pass); i++){
    key += pass[i];
  }
  return key;
}
