//AERO 405 Thrust Test Stand Code-Base Station Radio Test
// Cameron Gable | 3/15/2021

//------------------------------------------Libraries
//nRF24L01
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>
//------------------------------------------Global Set Up
//nRF24L01: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
  #define CE_PIN   7
  #define CSN_PIN 8

  const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
  RF24 radio(CE_PIN, CSN_PIN);

  char MenuNumber[17];
  bool newData = false;
  
  char A = '0';
  char BBB[4] = {'0','0','0','\0'};
  char CCC[4] = {'0','0','0','\0'};
  char DDD[4] = {'0','0','0','\0'};
  char EEE[4] = {'0','0','0','\0'};
  const char Seperator = ':';

//Buttons
  const int SensorOutButtonPin = 9;   //"Button 1"
  const int CalibrationButtonPin = 10; //"Button 2"
  const int SettingButtonPin = 11;     //"Button 3"

  int SensorOutButtonState = 0;
  int CalibrationButtonState = 0;
  int SettingButtonState = 0;
  
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.begin(9600);
  }

  //nRF24L01 Setup
  Serial.println("SimpleRx Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();

  //Button Setup
    pinMode(SensorOutButtonPin,INPUT);
    pinMode(CalibrationButtonPin,INPUT);
    pinMode(SettingButtonPin,INPUT);
    
}

void loop() {
  SensorOutButtonState = digitalRead(SensorOutButtonPin);
  CalibrationButtonState = digitalRead(CalibrationButtonPin);
  SettingButtonState = digitalRead(SettingButtonPin);
  if(SensorOutButtonState || CalibrationButtonState || SettingButtonState){
    Serial.println("Button Pressed");
    SensorOutButtonState = 0;
    CalibrationButtonState = 0;
    SettingButtonState = 0;
    delay(2000);
  }
  RemoteCommunication();
}


//------------------------------------------Main Helper Functions
void RemoteCommunication(){
  if ( radio.available() ) {
        radio.read( &MenuNumber, sizeof(MenuNumber) );
        newData = true;
    }
  if (newData == true) {
      Serial.print("Data received ");
      Serial.println(MenuNumber);
      GroupRemoteMessage();
      newData = false;
  }
}

void GroupRemoteMessage(){
  A = MenuNumber[0];
  BBB[0] = MenuNumber[2];
  BBB[1] = MenuNumber[3];
  BBB[2] = MenuNumber[4];
  CCC[0] = MenuNumber[6];
  CCC[1] = MenuNumber[7];
  CCC[2] = MenuNumber[8];
  DDD[0] = MenuNumber[10];
  DDD[1] = MenuNumber[11];
  DDD[2] = MenuNumber[12];
  EEE[0] = MenuNumber[14];
  EEE[1] = MenuNumber[15];
  EEE[2] = MenuNumber[16];
  Serial.print("Saved ID = ");
  Serial.print(A);
  Serial.print(" ");
  Serial.print(BBB);
  Serial.print(" ");
  Serial.print(CCC);
  Serial.print(" ");
  Serial.print(DDD);
  Serial.print(" ");
  Serial.println(EEE);
}
