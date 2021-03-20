//AERO 405 Thrust Test Stand Code-Base Station Radio Test
// Cameron Gable | 3/15/2021

//------------------------------------------Libraries
//nRF24L01
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>
//------------------------------------------Global Set Up
//nRF24L01: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
  RF24 radio(7, 8); // CE, CSN //create an RF24 object
  const byte address[6] = "00001";
  char MenuNumber[14];
  bool newData = false;
  
  int A = 0;
  int BBB = 000;
  int CCC = 000;
  int DDD = 000;
  int EEE = 000;
  int F = 0;

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
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(0, address);
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
  if (radio.available()) {
    radio.read(&MenuNumber, sizeof(MenuNumber));
    newData = true;
    Serial.print("MenuNumber Recieved: ");
    Serial.println(MenuNumber);
    newData = false;
    GroupRemoteMessage();
    while(A != 0){
      Serial.println("SUCCESS");
    }
  }
}

void GroupRemoteMessage(){
  A = atoi(MenuNumber[0]);
  BBB = atoi(MenuNumber[1])*100;
  BBB = BBB + atoi(MenuNumber[2])*10;
  BBB = BBB + atoi(MenuNumber[3]);
  CCC = atoi(MenuNumber[4])*100;
  CCC = CCC + atoi(MenuNumber[5])*10;
  CCC = CCC + atoi(MenuNumber[6]);
  DDD = atoi(MenuNumber[7])*100;
  DDD = DDD + atoi(MenuNumber[8])*10;
  DDD = DDD + atoi(MenuNumber[9]);
  EEE = atoi(MenuNumber[10])*100;
  EEE = EEE + atoi(MenuNumber[11])*10;
  EEE = EEE + atoi(MenuNumber[12]);
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
