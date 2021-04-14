#include <Adafruit_BME280.h>

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


  float LCcal_factor = 12.5;      //From the datasheet by the 4V range and 50lb capacity
  float LCoffset = 0.5;           //By the datasheet from the range of 0.5-4.5V
  float LCmomentArm = 1;
  float Voltcal_factor = 15.625;  //Takes Vout and converts to source voltage
  float Currcal_factor = 28.75;   //Takes Iout and converts to source current
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

}

void loop() {
  MeasureLC();
  MeasureVolt();
  MeasureCurrent();
  MeasureHall();
  Measure280();
  DisplayData();
}

//----------------------------HELPER FUNCTIONS
void MeasureLC(){
  LCval = analogRead(LCPin);
  CalibrateLC();
}

void CalibrateLC(){
  LCval_cal = LCcal_factor * (LCval-LCoffset) * LCmomentArm;
}

void MeasureVolt(){
  Voltval = analogRead(VoutPin);
  CalibrateVolt();
}

void CalibrateVolt(){
  //Calibration function from datasheet
  Voltval_cal = Voltcal_factor*Voltval;
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
  Hallval_cal = Hallcal_factor*Hallval;
}

void Measure280(){
  BMEtempFloat = bme.readTemperature();
  BMEpressFloat = bme.readPressure();
}

void DisplayData(){
  //DisplayLC();
  //DisplayVC();
  //DisplayHall();
  DisplayBME();
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
  Serial.print("BME Temperature: ");
  Serial.print(BMEtempFloat);
  Serial.println(" degrees Centigrade");
  Serial.print("BME Pressure: ");
  Serial.print(BMEpressFloat);
  Serial.println(" Pascals");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}
