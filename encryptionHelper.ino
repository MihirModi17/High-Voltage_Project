#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_NeoPixel.h>

const int MAXLENGTH = 64;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  char n[MAXLENGTH] = "test";
  Serial.print(n);
  Serial.print(" ");

  //Encrypt the array and print to Serial
  encrypt(n, 1);
  Serial.print("{");
  for(int i=0;i < strlen(n);i++)
  {
    Serial.print(int(n[i]));
    Serial.print(", ");
  }
  Serial.print("'\\0'} ");

  //Code to test if it works
  //char k[MAXLENGTH] = {116, 101, 115, 116, '\0'};
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
int keygen(char pass[]){
  int key = 0;
  for(int i = 0; i < strlen(pass); i++){
    key += pass[i];
  }
  return key;
}
