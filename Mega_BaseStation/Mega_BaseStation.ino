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
  RF24 radio(9, 8); // CE, CSN //create an RF24 object
  const byte address[6] = "00001"; //address through which two modules communicate.
  char MenuNumber[] = {00000000000000};
  int A = 0;
  int BBB = 000;
  int CCC = 000;
  int DDD = 000;
  int EEE = 000;
  int F = 0;

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
  
//Testing Global Variables
  int Throttle_Start = 0;
  int Throttle_End = 0;
  int Throttle_Step = 0;
  int Step_Time = 0;
  int Throttle_High = 0;
  int Throttle_Low = 0;
  int Test_Time = 0;
  bool Disp_Thrust = false;
  bool Disp_Torque = false;
  bool Disp_RPM = false;
  bool Disp_VoltAmp = false;
  bool Thrust_Cal = false;
  bool Torque_Cal = false;
  bool ESC_Cal = false;
  bool VoltAmp_Cal = false;
  //Default Settings
  int Warn_Volt = 3.4;
  int Min_Volt = 2.8;
  int Max_Amp = 75;
  int Max_Power = 1500;
  int Config_Num = 1;

  int Throttle = 1000;

  
void setup() {
  while (!Serial) {
    Serial.begin(9600);
  }
  //nRF24L01 Setup
    radio.begin();
    radio.openReadingPipe(0, address);//set the address
    radio.startListening();//Set module as receiver
    
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
        while (1) delay(10);
    }
    
  //Initial Settings
    Settings();
    
  //Initial Data Gathering
    AmbientMeasure();
  
}

void loop() {
  if (A == 0){              //Gather Remote Data
    RemoteCommunication();
    GroupRemoteMessage();
    
  }else if(A == 1){         //Throttle Sweep
    Throttle_Start = BBB;
    Throttle_End = CCC;
    Throttle_Step = DDD;
    Step_Time = EEE*1000;
    ThrottleSweep();    //Run Subfunction
    
  }else if(A == 2){         //Constant Throttle
    Throttle_Start = BBB;
    Step_Time = CCC*1000;
    ConstantThrottle(); //Run Subfunction
    
  }else if(A == 3){         //Stress Test
    Throttle_High = BBB;
    Throttle_Low = CCC;
    Step_Time = DDD*1000;
    Test_Time = EEE;
    StressTest();       //Run Subfunction
    
  }else if(A == 4){         //Sensor Output
    Disp_Thrust = BBB;
    Disp_Torque = CCC;
    Disp_RPM = DDD;
    Disp_VoltAmp = EEE;
    SensorOutput();     //Run Subfunction
    
  }else if(A == 5){         //Calibration
    Thrust_Cal = BBB;
    Torque_Cal = CCC;
    ESC_Cal = DDD;
    VoltAmp_Cal = EEE;
    Calibration();      //Run Subfunction
    
  }else if(A == 6){         //Edit Settings
    Warn_Volt = BBB/10;
    Min_Volt = CCC/10;
    Max_Amp = DDD;
    Max_Power = EEE;
    Config_Num = F;
    Settings();       //Run Subfunction
    
  }else{
    A = 0; //If outside bounds gather remote signal again.
  }
  

}


//------------------------------------------Main Helper Functions
void RemoteCommunication(){
  if (radio.available() && MenuNumber == 00000000000000) {
    radio.read(&MenuNumber, sizeof(MenuNumber));
    Serial.println(MenuNumber);
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
  F = atoi(MenuNumber[13]);
}

void AmbientMeasure(){
  AmbTemp = bme.readTemperature();
  AmbPressure = bme.readPressure();
}

void PostTestReset(){
  esc.writeMicroseconds(1000);
  char MenuNumber[] = {00000000000000};
  GroupRemoteMessage();
}

void ThrottleSweep(){
  Throttle = 1000 + 10*Throttle_Start;
  while(Throttle < Throttle_End){
    esc.writeMicroseconds(Throttle);
    Throttle = Throttle + Throttle_Step;
    Measure();
  }
  PostTestReset();
}

void ConstantThrottle(){
  Throttle = 1000 + 10*Throttle_Start;
  Measure();
  PostTestReset();
}

void StressTest(){
  while(TESTTIME???? FIX HERE NEXT){
    Throttle = 1000 + 10*Throttle_Low;
    esc.writeMicroseconds(Throttle);
    Measure();
    Throttle = 1000 + 10*Throttle_High;
    esc.writeMicroseconds(Throttle);
    Measure();
  }
}

void SensorOutput(){
  
}

void Calibration(){
  
}

void Settings(){
  
}


//------------------------------------------Test Helper Functions
void Measure(){
  SafetyCheck();
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
}

void SendWarning(){
  
}

//------------------------------------------Measurement Helper Functions
void Thrust_AVG(){
  
}

void Torque_AVG(){
  
}

void RPM_AVG(){
  
}

void VoltCurrent_AVG(){
  
}

void Airspeed_AVG(){
  
}
