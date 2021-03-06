#include <Adafruit_BME280.h>
#include <SD.h>

//Pin Setup
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
    
//Variable Setup
  float LCval = 0;
  float Voltval = 0;
  float Currval = 0;
  float Hallval = 0;

  //BME 280
  float BMEtempFloat = 0;
  float BMEpressFloat = 0;

  float LCval_cal = 0;
  float Voltval_cal = 0;
  float Currval_cal = 0;
  float Hallval_cal = 0;


  float LCcal_factor = .0166;      //From the datasheet by the 4V range and 50lb capacity
  float LCoffset = 107;           //By the datasheet from the range of 0.5-4.5V
  float LCmomentArm = 1;
  float Voltcal_factor = 0.0773;  //Takes Vout and converts to source voltage
  float Currcal_factor = 0.1335;   //Takes Iout and converts to source current
  float Hallcal_factor = 1;       //UNKNOWN AND Based off flexure

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Calibration Data Function");
  pinMode(LCPin, INPUT);
  pinMode(IoutPin, INPUT);
  pinMode(VoutPin, INPUT);
  pinMode(HallPin, INPUT);  

  //BME280 Setup
  unsigned status;
  status = bme.begin();
    //Gather Ambient Conditions
    Measure280();
    DisplayBME();
  //SD Setup
  pinMode(chipSelect,OUTPUT);
  DataOut = SD.open("Data.txt", FILE_WRITE);
  if (DataOut){
    Serial.print("Writing SD Header...");
    DataOut.println("AE405 Thrust Stand");
    DataOut.print("Ambient Conditions: [Temperature: ");
    DataOut.print(BMEtempFloat);
    DataOut.print(" Celcius] | [Pressure: ");
    DataOut.print(BMEpressFloat);
    DataOut.println(" Pascals]");
    DataOut.println("");
    DataOut.println("Thrust (N) | Volts | Amps | Torque (n-m)");
    Serial.println("done.");
  }else{
    Serial.println("Error opening data file");
  }
}

void loop() {
  MeasureLC();
  MeasureVolt();
  MeasureCurrent();
  MeasureHall();
  DisplayData();
}

//----------------------------HELPER FUNCTIONS
void MeasureLC(){
  LCval = analogRead(LCPin);
  CalibrateLC();
}

void CalibrateLC(){
  LCval_cal = LCcal_factor * (LCval-LCoffset);
}

void MeasureVolt(){
  Voltval = analogRead(VoutPin);
  CalibrateVolt();
}

void CalibrateVolt(){
  //Calibration function from datasheet
  Voltval_cal = Voltcal_factor*(Voltval-1);
}

void MeasureCurrent(){
  Currval = analogRead(IoutPin);
  CalibrateCurrent();
}

void CalibrateCurrent(){
  Currval_cal = Currcal_factor*Currval;
}

void MeasureHall(){
  Hallval = analogRead(HallPin);
  CalibrateHall();
}

void CalibrateHall(){
//  Hallval_cal = -182 + 1.02*Hallval + -0.00205*pow(Hallval,2) +  0.00000144*pow(Hallval,3);
  Hallval_cal = 26.131 - 0.771*Hallval + 0.00365*pow(Hallval,2) - 0.000006444*pow(Hallval,3) + 0.0000000040224*pow(Hallval,4);
}

void Measure280(){
  BMEtempFloat = bme.readTemperature();
  BMEpressFloat = bme.readPressure();
}

void DisplayData(){
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  DisplayLC();
  DisplayVC();
  DisplayHall();
  delay(2000);
}
void DisplayLC(){
  Serial.println("New Data Measurement");
  Serial.print("Load Cell Raw Output: ");
  Serial.print(LCval);
  Serial.print(" | Load Cell Calibrated Output: ");
  Serial.println(LCval_cal);
}
void DisplayVC(){
  Serial.print("Voltage Raw Output: ");
  Serial.print(Voltval);
  Serial.print(" | Voltage Calibrated Output: ");
  Serial.println(Voltval_cal);
  Serial.print("Current Raw Output: ");
  Serial.print(Currval);
  Serial.print(" | Current Calibrated Output: ");
  Serial.println(Currval_cal);
}

void DisplayHall(){
  Serial.print("Hall Raw Output: ");
  Serial.print(Hallval);
  Serial.print(" | Hall Calibrated Output: ");
  Serial.println(Hallval_cal);
}

void DisplayBME(){
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.print("BME Temperature: ");
  Serial.print(BMEtempFloat);
  Serial.println(" degrees Centigrade");
  Serial.print("BME Pressure: ");
  Serial.print(BMEpressFloat);
  Serial.println(" Pascals");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

}

void WriteData(){
  //Thrust (N) | Volts | Amps | Torque (n-m)
  DataOut.print(LCval_cal);
  DataOut.print(",");
  DataOut.print(Voltval_cal);
  DataOut.print(",");
  DataOut.print(Currval_cal);
  DataOut.print(",");
  DataOut.print(Hallval_cal);
  DataOut.println(",");
}




