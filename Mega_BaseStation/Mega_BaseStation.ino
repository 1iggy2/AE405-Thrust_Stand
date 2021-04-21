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

//SD Card
  #include <SD.h>

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

//------------------------------------------Sensor Pin Declaration
//If confused reference this pinout diagram:
    // https://www.javatpoint.com/arduino-mega-pinout
  #define LCPin A0      //Wire LC Voltage (O+) wire to pin A0
  //See Datasheet page 12 for wiring information:
    // http://www.farnell.com/datasheets/2864809.pdf
  //NOTE:   V+ goes to 5V
        //  0+ pin to A0
        //  COM to Ground
  #define IoutPin A1    //Wire KR-Sense Iout to pin A1
  #define VoutPin A2    //Wire KR-Sense Vout to pin A2
  //NOTE:   Connect "⏚" Pin to Ground    

  //Hall Effect Sensor
    //SS49E DataSheet: https://www.addicore.com/SS49E-Linear-Hall-Sensor-p/ad316.htm
    //Pin 1 -> 5V
    //Pin 2 -> Ground
    //Pin 3 -> A3
  #define HallPin A3

  //Tach
  #define RPMPin 2

  //BME 280
  Adafruit_BME280 bme; //Initialized I2C using default I2C Bus
    //Vin -> 5V
    //SCK -> 21 (SCL)
    //SDI -> 20 (SDA)
    //GND -> GND

  //SD Card Writer
  #define chipSelect 53
    //CS -> 53
    //5V -> 5V
    //GND -> GND
    //CLK -> 52
    //DO -> 50
    //DI -> 51
  
// SD Setup
    File DataOut;

//ESC Setup
  int ESC_PIN = 6;
  Servo esc;

//------------------------------------------Variable Set Up
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
  float Test_Time = 0;
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

  //RPM Test Variables
  int RPMread1 = 0;
  int RPMread2 = 0;
  unsigned long RPMfirst;
  unsigned long RPMsecond;

//Data Storage Variables
  float LCval = 0;
  float Voltval = 0;
  float Currval = 0;
  float Hallval = 0;
  unsigned long RPMval = 0;

  //BME 280
  float BMEtempFloat = 0;
  float BMEpressFloat = 0;

  //Calibrated Values
  float LCval_cal = 0;
  float Voltval_cal = 0;
  float Currval_cal = 0;
  float Hallval_cal = 0;
  float RPMval_cal = 0;

  //Human Readable Power Values
  float Voltage = 0;
  float Amp = 0;
  float Power = 0;

  //Calibration Factors
  float LCcal_factor = 12.5;      //From the datasheet by the 4V range and 50lb capacity
  float LCoffset = 0.5;           //By the datasheet from the range of 0.5-4.5V
  float LCmomentArm = 1;
  float Voltcal_factor = 15.625;  //Takes Vout and converts to source voltage
  float Currcal_factor = 28.75;   //Takes Iout and converts to source current
  float Hallcal_factor = 1;       //UNKNOWN AND Based off flexure
  float RPMcal_factor = 60000;    //MS per Minute

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
    
  //Sensor Pin Initialization
  pinMode(LCPin, INPUT);
  pinMode(IoutPin, INPUT);
  pinMode(VoutPin, INPUT);
  pinMode(HallPin, INPUT);  
  pinMode(RPMPin, INPUT);


  //ESC Setup -- THIS BREAKS EVERYTHING
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
  }

  //Initial Data Gathering
  AmbientMeasure();

  //SD Setup
  pinMode(chipSelect,OUTPUT);
  DataOut = SD.open("Data.txt", FILE_WRITE);
  if (DataOut){
    Serial.print("Writing SD Header...");
    DataOut.println("AE405 Thrust Stand,Winter 2021,,,Cameron Gable");
    DataOut.println("Find Documentation at:, https://github.com/1iggy2/AE405-Thrust_Stand");
    DataOut.println("");
    DataOut.println("Ambient Conditions:");
    DataOut.print("Temperature:,");
    DataOut.print(BMEtempFloat);
    DataOut.print(", Celsius");
    DataOut.println("Pressure:,");
    DataOut.print(BMEpressFloat);
    DataOut.println(" Pascals");
    DataOut.println("");
    DataOut.println("Thrust (N),Volts,Amps,Torque (n-m),RPM");
    Serial.println("done.");
  }else{
    Serial.println("Error opening data file");
  }
    
  
  
}

void loop() {
  if (A == 0){              //Gather Remote Data
    RemoteCommunication();
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
  Serial.println("Trying to read data ");
  if ( radio.available() ) {
        radio.read( &MenuNumber, sizeof(MenuNumber) );
        newData = true;
    } else {
      Serial.println("Read Failure");
    }
  if (newData == true) {
      Serial.print("Data received ");
      Serial.println(MenuNumber);
      GroupRemoteMessage();      
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
  Measure280();
}

void Measure280(){
  BMEtempFloat = bme.readTemperature();
  BMEpressFloat = bme.readPressure();
}

void PostTestReset(){
  Serial.println();
  Serial.println("Test Complete");
  esc.writeMicroseconds(1000);
  MenuNumber[17];
  A = 0;
  while(true){
    Serial.println("Testing Complete");
    }
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
  Serial.print("Throttle Step Size: ");
  Serial.println(Throttle_Step);
  Throttle = Throttle_Start;
  while(Throttle < Throttle_End){
    Serial.println("Throttle Sweep Main");
    esc.writeMicroseconds(Throttle);
    currentMillis = millis();
    prevMillis = millis();
    while(prevMillis - currentMillis <= Step_Time){
      Serial.println("Throttle Sweep Subloop");
      Measure();
      prevMillis = millis();
    }
    Serial.println("STEP");
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
  Serial.println("Save");  
  //Thrust (N) | Volts | Amps | Torque (n-m) | RPM
  DataOut.print(LCval_cal);
  DataOut.print(",");
  DataOut.print(Voltval_cal);
  DataOut.print(",");
  DataOut.print(Currval_cal);
  DataOut.print(",");
  DataOut.print(Hallval_cal);
  DataOut.print(",");
  DataOut.print(RPMval_cal);
  DataOut.println(",");
}

void SafetyCheck(){
  if(Voltage < Warn_Volt){
    SendWarning();
  }
  if(Voltage < Min_Volt || Amp > Max_Amp || Power > Max_Power){
    Serial.println("Ending Test");
    esc.writeMicroseconds(1000);
  }
  RemoteCommunication(); //Checks for remote kill switch
  CheckKillSwitch();
}

void SendWarning(){
  Serial.println("SYSTEM WARNING");
  Serial.print("Voltage: ");
  Serial.print(Voltage);
  Serial.println(" volts");
  //Future work: Install a buzzer
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
  LCval = analogRead(LCPin);
  CalibrateLC();
}

void CalibrateLC(){
  LCval_cal = LCcal_factor * (LCval-LCoffset) * LCmomentArm;
}

void Torque_Measurement(){
  Hallval = analogRead(HallPin);
  CalibrateHall();
}

void CalibrateHall(){
  Hallval_cal = Hallcal_factor*Hallval;
}

void RPM_Measurement(){
  while(RPMread1 == 0){
    RPMread1 = digitalRead(RPMPin);
  }

  RPMfirst = millis();
  
  while(RPMread2 == 0){
    RPMread2 = digitalRead(RPMPin);
  }
  RPMsecond = millis();

  RPMval = RPMsecond-RPMfirst; //Milliseconds per rotation

  //Reset state storage variables
  RPMread1 = 0;
  RPMread2 = 0;
  CalibrateRPM();
}

void CalibrateRPM(){
  RPMval_cal = RPMcal_factor/RPMval; //RP(Ms)->RPM
}

void VoltCurrent_Measurement(){
  Voltval = analogRead(VoutPin);
  CalibrateVolt();
  Currval = analogRead(IoutPin);
  CalibrateCurrent();
  Voltage = Voltval_cal;
  Amp = Currval_cal;
  Power = Voltage*Amp;
}

void CalibrateVolt(){
  //Calibration function from datasheet
  Voltval_cal = Voltcal_factor*Voltval;
}

void CalibrateCurrent(){
  Currval_cal = Currcal_factor*Currval;
}

void Airspeed_Measurement(){
  //Future Work: Implement
}
