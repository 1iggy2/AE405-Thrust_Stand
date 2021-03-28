//AERO 405 Thrust Test Stand Code-Base Station
// Cameron Gable | 3/15/2021

//------------------------------------------Libraries
//nRF24L01
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>
  
//BME280
  #include <Adafruit_BME280.h>

//ESC Use
  #include <Servo.h>

//Time based commands
  #include <Time.h>
  #include <TimeLib.h>
//------------------------------------------Global Set Up
//nRF24L01: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
  #define CE_PIN   7
  #define CSN_PIN 8

  const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
  RF24 radio(CE_PIN, CSN_PIN);

  char MenuNumber[17];
  bool newData = false;
  
  char charA[2] = {'0','\0'};
  char charBBB[4] = {'0','0','0','\0'};
  char charCCC[4] = {'0','0','0','\0'};
  char charDDD[4] = {'0','0','0','\0'};
  char charEEE[4] = {'0','0','0','\0'};
  int A = 0;
  int BBB = 0;
  int CCC = 0;
  int DDD = 0;
  int EEE = 0;
  const char Seperator = ':';

//BME280
  #define BME_SCK 13 //Set Pins
  #define BME_MISO 12
  #define BME_MOSI 11
  #define BME_CS 10
  Adafruit_BME280 bme;
  int AmbTemp = 0;
  int AmbPressure = 0;
  
//ESC: MVFT Code Sensor_Package_source.ino
  int ESC_PIN = 0;
  Servo esc;

//Buttons
  const int SensorOutButtonPin = 9;   //"Button 1"
  const int CalibrationButtonPin = 10; //"Button 2"
  const int SettingButtonPin = 11;     //"Button 3"
  int SensorOutButtonState = 0;
  int CalibrationButtonState = 0;
  int SettingButtonState = 0;
  
//Testing Global Variables
  int Throttle_Start = 0;
  int Throttle_End = 0;
  int Throttle_Step = 0;
  int Step_Time = 0;
  int Throttle_High = 0;
  int Throttle_Low = 0;
  int Test_Time = 0;
  unsigned long currentMillis; //Timing Vars
  unsigned long currentMillis2;
  unsigned long prevMillis;
  unsigned long prevMillis2;  
  bool Disp_Thrust = false;
  bool Disp_Torque = false;
  bool Disp_RPM = false;
  bool Disp_VoltAmp = false;
  bool Thrust_Cal = false;
  bool Torque_Cal = false;
  bool ESC_Cal = false;
  bool VoltAmp_Cal = false;
  //Temp Data Storage Variables
  int Voltage;
  int Amp;
  int Power;
  //Default Settings
  float Warn_Volt = 3.4;
  float Min_Volt = 2.8;
  float Max_Amp = 75;
  float Max_Power = 1500;
  int Config_Num = 1;

  int Throttle = 1000;

  
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.begin(9600);
  }
  //nRF24L01 Setup
  Serial.println("Radio Comms Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
    
  //ESC Setup
    esc.attach(ESC_PIN);
    esc.writeMicroseconds(1000);

  //BME280 Setup
    unsigned status;
    status = bme.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        //while (1) delay(10);
    }
    
  //Initial Data Gathering
    AmbientMeasure();
  
}

void loop() {
  if (A == 0){              //Gather Remote Data
    RemoteCommunication();
    GroupRemoteMessage();
    CheckButtonStatus();
  }else if(A == 1){         //Throttle Sweep
    Throttle_Start = 1000+10*BBB;
    Throttle_End = 1000+10*CCC;
    Test_Time = DDD*1000; //Seconds to MS
    Step_Time = EEE*1000; //Seconds to MS
    ThrottleSweep();    //Run Subfunction
    
  }else if(A == 2){         //Constant Throttle
    Throttle_Start = 1000+10*BBB;
    Test_Time = DDD*1000; //Seconds to MS
    ConstantThrottle(); //Run Subfunction
    
  }else if(A == 3){         //Stress Test
    Throttle_High = 1000+10*BBB;
    Throttle_Low = 1000+10*CCC;
    Test_Time = DDD*1000;
    Step_Time = EEE*1000;
    StressTest();       //Run Subfunction
    
  }else if(A == 4){         //Sensor Output     BEGIN SERIAL OPERATIONS
    GetSensorOutputParameters();
    SensorOutput();     //Run Subfunction
    
  }else if(A == 5){         //Calibration
    GetCalibrationParameters();
    Calibration();      //Run Subfunction
    
  }else if(A == 6){         //Edit Settings
    GetSettingsParameters(); //Gather Settings
  }else{
    A = 0; //If outside bounds gather remote signal again.
  }
  

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
      newData = false;
  }
}

void GroupRemoteMessage(){
  charA[0] = MenuNumber[0];
  charBBB[0] = MenuNumber[2];
  charBBB[1] = MenuNumber[3];
  charBBB[2] = MenuNumber[4];
  charCCC[0] = MenuNumber[6];
  charCCC[1] = MenuNumber[7];
  charCCC[2] = MenuNumber[8];
  charDDD[0] = MenuNumber[10];
  charDDD[1] = MenuNumber[11];
  charDDD[2] = MenuNumber[12];
  charEEE[0] = MenuNumber[14];
  charEEE[1] = MenuNumber[15];
  charEEE[2] = MenuNumber[16];
  Serial.print("Saved ID = ");
  Serial.print(charA);
  Serial.print(" ");
  Serial.print(charBBB);
  Serial.print(" ");
  Serial.print(charCCC);
  Serial.print(" ");
  Serial.print(charDDD);
  Serial.print(" ");
  Serial.println(charEEE);
  A = atoi(charA);
  BBB = atoi(charBBB);
  CCC = atoi(charCCC);
  DDD = atoi(charDDD);
  EEE = atoi(charEEE);
  Serial.print("Saved Ints = ");
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

void AmbientMeasure(){
  AmbTemp = bme.readTemperature();
  AmbPressure = bme.readPressure();
}

void PostTestReset(){
  Serial.println();
  Serial.println("Test Complete");
  esc.writeMicroseconds(1000);
  MenuNumber[17];
  A = 0;
  while(true){}
}

void CheckButtonStatus(){
  SensorOutButtonState = 0;   //Reset Button States
  CalibrationButtonState = 0;
  SettingButtonState = 0;
  SensorOutButtonState = digitalRead(SensorOutButtonPin);   //Read Button States
  CalibrationButtonState = digitalRead(CalibrationButtonPin);
  SettingButtonState = digitalRead(SettingButtonPin);
  ButtonInterpretation(); //Interpret the press
}

void ButtonInterpretation(){
  if(SensorOutButtonState){
    A = 4;
  }else if(CalibrationButtonState){
    A = 5;
  }else if(SettingButtonState){
    A = 6;
  }
}

void ThrottleSweep(){
  float FloatTS = (Throttle_End-Throttle_Start)/(Test_Time/Step_Time);
  Throttle_Step = (int) FloatTS;
  Throttle = Throttle_Start;
  while(Throttle < Throttle_End){
    esc.writeMicroseconds(Throttle);
    currentMillis = millis();
    prevMillis = millis();
    while(prevMillis - currentMillis <= Step_Time){
      Measure();
      prevMillis = millis();
    }
    Throttle = Throttle + Throttle_Step;
  }
  PostTestReset();
}

void ConstantThrottle(){
  Serial.println("In Constant Throttle");
  esc.writeMicroseconds(Throttle_Start);
  Serial.print("Testing Throttle: ");
  Serial.println(Throttle_Start);
  currentMillis = millis();
  prevMillis = millis();
  while(prevMillis - currentMillis <= Test_Time){
    Measure();
    prevMillis = millis();
  }
  PostTestReset();
}

void StressTest(){
  Serial.println("In Stress Test");
  currentMillis2 = millis();
  prevMillis2 = millis();
  Throttle = Throttle_High; //Start on Low Throttle after IF
  while(prevMillis2-currentMillis2 <= Test_Time){
    if(Throttle == Throttle_Low){
      Serial.println("HIGH");
      Throttle = Throttle_High;
    }else if(Throttle == Throttle_High){
      Serial.println("LOW");
      Throttle = Throttle_Low;
    }else{
      esc.writeMicroseconds(1000); //Else turn off
    }
    Serial.print("Testing Throttle: ");
    Serial.println(Throttle);
    esc.writeMicroseconds(Throttle);
    currentMillis = millis();
    prevMillis = millis();
    while(prevMillis-currentMillis <= Step_Time){
      Measure();
      prevMillis = millis();
    }
    prevMillis2 = millis();
  }
  PostTestReset();
}

void GetSensorOutputParameters(){
  Serial.println("Sensor Output Settings");
  Serial.println("Answer all with the number 1 or 0 (true or false)");
  Serial.print("1.) Would you like to display thrust output: ");
  BBB = Serial.read();
  Serial.println();
  Serial.print("2.) Would you like to display torque output: ");
  CCC = Serial.read();
  Serial.println();
  Serial.print("3.) Would you like to display RPM output: ");
  DDD = Serial.read();
  Serial.println();
  Serial.print("4.) Would you like to display Voltage and Amperage output: ");
  EEE = Serial.read();
  Serial.println();
  Serial.println("Sensor Output Settings Complete");
}

void SensorOutput(){
  
}

void GetCalibrationParameters(){
  Serial.println("Calibration Settings");
  Serial.println("Answer all with the number 1 or 0 (true or false)");
  Serial.print("1.) Would you like to calibrate thrust sensor: ");
  BBB = Serial.read();
  Serial.println();
  Serial.print("2.) Would you like to calibrate torque sensor: ");
  CCC = Serial.read();
  Serial.println();
  Serial.print("3.) Would you like to calibrate RPM sensor: ");
  DDD = Serial.read();
  Serial.println();
  Serial.print("4.) Would you like to calibrate Voltage and Amperage sensor: ");
  EEE = Serial.read();
  Serial.println();
  Serial.println("Calibration Settings Complete");
}

void Calibration(){
  
}

void GetSettingsParameters(){
  Serial.println("Safety Settings");
  Serial.print("1.) Enter Warning Voltage in Volts (Default 3.4v): ");
  Warn_Volt = Serial.read();
  Serial.println();
  Serial.print("2.) Enter Minimum Voltage in Volts (Default 2.8v): ");
  Min_Volt = Serial.read();
  Serial.println();
  Serial.print("3.) Enter Maximum Amperage in Amps (Default 75A): ");
  Max_Amp = Serial.read();
  Serial.println();
  Serial.print("4.) Enter Maximum Power in Watts (Default 1500W): ");
  Max_Power = Serial.read();
  Serial.println();
  Serial.print("4.) Enter Configuration Number (interger): ");
  Config_Num = Serial.read();
  Serial.println();
  Serial.println("Settings Complete");
}


//------------------------------------------Test Helper Functions
void Measure(){
  SafetyCheck();
  Thrust_Measurement();
  Torque_Measurement();
  RPM_Measurement();
  VoltCurrent_Measurement();
  //Airspeed_Measurement(); //May not be included
  Save();
}

void Save(){
  
}

void SafetyCheck(){
  if(Voltage < Warn_Volt){
    SendWarning();
  }
  if(Voltage < Min_Volt || Amp > Max_Amp || Power > Max_Power){
    esc.writeMicroseconds(1000);
  }
  RemoteCommunication(); //Checks for remote kill switch
  CheckKillSwitch();
}

void SendWarning(){
  
}

void CheckKillSwitch(){
  if(MenuNumber[0] == '9'){
    esc.writeMicroseconds(1000);
    while(true){  //Infinitely loop
      Serial.println("KILLSWITCH ACITVATED");
    }
  }
}

//------------------------------------------Measurement Helper Functions
void Thrust_Measurement(){
  
}

void Torque_Measurement(){
  
}

void RPM_Measurement(){
  
}

void VoltCurrent_Measurement(){
  
}

void Airspeed_Measurement(){
  
}
