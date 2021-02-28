#include "HX711.h"
#include "time.h"
#include <Servo.h>

//pins for data and clock of both load cells
#define DATA1 47
#define CLK1  46
#define DATA2 45  
#define CLK2  44

//initialize both load cells
HX711 force(DATA1, CLK1);
HX711 torque(DATA2, CLK2);


// Voltage sensor variables
int analogVoltage = 0;
float voltageCalibrationFactor = 1;  //1.0215 is voltage multiplier default for 6s voltage range
float correctedVoltage = 0.0; 
float averageVoltage = 0.0;
float lastVoltage = 0.0;
float vout = 0.0;
float vin = 0.0;
float R1 = 46700; //47000ohm resistor //resitance value of resistor 1 in the voltage divider circuit
float R2 = 9890; //10000ohm resistor //resitance value of resistor 2 in the voltage divider circuit


// Load cell variables
float calibration_factor_force = -147900; //arbitrary scaling factor for force load cell
float calibration_factor_torque = -33000; //arbitrary scaling factor for torque load cell


// Current sensor varialbes
int analogCurrent = 0;
int currentCalibrationFactor = 0;
float correctedCurrent = 0.0;
float averageCurrent = 0.0;
float lastCurrent = 0.0;



//tachometer
int hall_sensor = 3;
long rpm = 0;
int mag_reading = 0;
int magnets_passed = 0;
int last_reading = 0;
unsigned long time_at_last_reset = 0;

/*
// Tachometer Calibration and Variables

////// Calibration:
byte PulsesPerRevolution = 2;  // Set how many pulses there are on each revolution. Default: 2
unsigned long ZeroTimeout = 100000;  // For high response time, a good value would be 100000
                                           // For reading very low RPM, a good value would be 300000
                                           
const byte numReadings = 5;  // Number of samples for smoothing. The higher, the more smoothing, but it's going to
                             // react slower to changes. 1 = no smoothing. Default: 2
////// Variables:
volatile unsigned long LastTimeWeMeasured;  // Stores the last time we measured a pulse so we can calculate the period
volatile unsigned long PeriodBetweenPulses = ZeroTimeout+1000;  // Stores the period between pulses in microseconds
volatile unsigned long PeriodAverage = ZeroTimeout+1000;  // Stores the period between pulses in microseconds in total, if we are taking multiple pulses
unsigned long FrequencyRaw;  // Calculated frequency, based on the period
unsigned long FrequencyReal;  // Frequency without decimals
unsigned long RPM;  // Raw RPM without any processing
unsigned int PulseCounter = 1;  // Counts the amount of pulse readings we took so we can average multiple pulses before calculating the period

unsigned long PeriodSum; // Stores the summation of all the periods to do the average

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;  // Stores the last time we measure a pulse in that cycle
long CurrentMicros = micros();  // Stores the micros in that cycle

unsigned int AmountOfReadings = 1; //Amount of pulses to take before calaulating the RPM, its set automatically based on the speed
unsigned int ZeroDebouncingExtra;  // Stores the extra value added to the ZeroTimeout to debounce it

// Variables for smoothing tachometer:
unsigned long readings[numReadings];  // The input.
unsigned long readIndex;  // The index of the current reading.
unsigned long total;  // The running total.
unsigned long average;  // The RPM value after applying the smoothing.

*/

//initialize esc pin
Servo esc;



void setup() {
  //start serial monitor
  Serial.begin(9600);
  instructions();

  //coles tachometer
  pinMode(hall_sensor, INPUT);
  
  // Enable interruption pin 2 when going from LOW to HIGH for tachometer
  //attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);  
  //zero both load cells and apply scaling factor
  force.set_scale(calibration_factor_force);
  torque.set_scale(calibration_factor_torque);
  force.tare();
  torque.tare();
  setTime(0);
  //assign esc digital pin and set throttle to 0
  esc.attach(10);
  esc.writeMicroseconds(1000);
}


String tempString = "";
int inputThrottle = 0;
int throttle = 0;
int testTime = 0;

void loop() {
  esc.writeMicroseconds(1000); //ensures motor is off by default

  if(Serial.available()){
    //reads in user input from serial monitor
    tempString = Serial.readStringUntil('\n');
    tempString.trim();

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // Load cell Calibration function
    if(tempString == "loadcal"){
      Serial.println("Load Cell Calibration Mode:");
      Serial.println("Remove all weights from sensors");
      delay(3000);
      Serial.println("After readings begin, place known load on sensor");
      Serial.println("Press + to increase calibration factor");
      Serial.println("Press - to decrease calibration factor");
      delay(1000);
      // zero both load cells after weight has been removed
      force.tare();
      torque.tare();

      // enters force load cell calibration. 'c' exits the loop
      Serial.println("Force Calibration. Type 'c' to continue to torque calibration");
      delay(500);
      while(1 == 1){
        //updates scaling factor every loop
        force.set_scale(calibration_factor_force);
        Serial.print("Force Reading: ");
        Serial.print(-force.get_units()); // output force measurement
        Serial.print(" lbs"); 
        Serial.print(" calibration_factor: ");
        //displays calibration factor for future use
        Serial.print(calibration_factor_force);
        Serial.println();
        //reads user input to change scaling factor or exit loop
        if(Serial.available()){
           char temp = Serial.read();
           if(temp == '+'){
            calibration_factor_force += 100;
           }
           else if(temp == '-'){
            calibration_factor_force -= 100;
           }
           else if(temp == 'c'){
              break;
           }
        }
        delay(100);
      }

      // enters torque load cell calibration. 'c' exits the loop
      // loop follows same format as force calibration loop
      Serial.println("Torque Calibration. Type 'x' to exit to main menu");
      delay(500);
      while(1 == 1){
        force.set_scale(calibration_factor_torque);
        Serial.print("Torque Reading: ");
        Serial.print(torque.get_units()); // output force measurement
        Serial.print(" lbs"); 
        Serial.print(" calibration_factor: ");
        Serial.print(calibration_factor_torque);
        Serial.println();
        if(Serial.available()){
           char temp = Serial.read();
           if(temp == '+'){
            calibration_factor_torque += 100;
           }
           else if(temp == '-'){
            calibration_factor_torque -= 100;
           }
           else if(temp == 'x'){
              Serial.println("Exiting Load Cell Calibration");
              break;
           }
        }
        delay(100);
      }
    }

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    /*
    // Tachometer Calibration function
    if(tempString == "tachcal"){
      Serial.println();
      Serial.println();
      Serial.println("Tachometer Calibration Mode:");
      delay(500);
      // enters number of pulses calibration. 'c' exits the loop
      Serial.println("Enter number of magnets inside motor");
      while(Serial.available() == 0){
        PulsesPerRevolution = Serial.parseInt();
      }
      Serial.read();

      Serial.print("Pulses per revolution will be: ");
      Serial.println(PulsesPerRevolution);
      Serial.println();


      // enters tachometer timeout calibration
      // loop follows same format as magnets calibration loop
      Serial.println("Timeout Calibration. Enter a value between 100000 and 300000");
      Serial.println("Values near 100000 will increase responsiveness while values near 300000 will allow for low RPM reading ");
      while(Serial.available() == 0){
        ZeroTimeout = Serial.parseInt();
      }
      Serial.read();
      
      Serial.print("ZeroTimeout: ");
      Serial.println(ZeroTimeout);

    }
*/
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // Current Sensor Calibration function
    if(tempString == "currcal"){
      Serial.println();
      Serial.println();
      Serial.println("Current Sensor Calibration Mode:");
      delay(500);
      // enters current offset calibration
      Serial.println("Unplug battery");
      delay(3000);
      Serial.println("Once readings start, if reading is not between -1 and 1 enter value ");
      Serial.println("between -100 and 100 to offset value to zero. Enter 200 to exit to main menu.");
      delay(3000);
      
      while(1 == 1){
        //updates current offset every loop
        Serial.print("Analog Current Reading: ");
        Serial.print(analogRead(A1) - (434 - currentCalibrationFactor)); // output current measurement
        Serial.println(" A");
        //reads user input to change scaling factor or exit loop
        if(Serial.available()){
           int temp = Serial.parseInt();
           if(temp >= -100 && temp <= 100 && temp != 0){
            currentCalibrationFactor += temp;
           }
           else if(temp == 0){
              Serial.println("Exiting Current Calibration");
              break;
           }
        }
        delay(100);
      }
    }

 // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // Voltage Sensor Calibration function
    if(tempString == "voltcal") {
      Serial.println();
      Serial.println();
      Serial.println("Voltage Sensor Calibration Mode:");
      Serial.println("Measure battery voltage with multimeter");
      delay(2000);
      Serial.println("Plug the battery in");
      delay(2000);
      // enters voltage multiplier calibration
      Serial.println("Once readings start, if reading is not accurate enter actual voltage");
      Serial.println("of the battery. Enter 'x' to exit to main menu.");
      delay(3000);

      while(1 == 1){
        analogVoltage = analogRead(A0);
        vout = (analogVoltage * 5) / 1023.0; //mult by 5 bc 5v is the reference voltage and divide by 1023 to get actual voltage in a 5v range
  
        vin = (vout*(R1 + R2))/R2;  //bc we want to mesaure more than 5v we use a volatge divider and this formula takes the lowered voltage 
                                    //from the output of the divider and corrects it to the voltage at the postiive terminal of the divider

        correctedVoltage = vin * voltageCalibrationFactor;
        
        //updates voltage multiplier every loop
        Serial.print("Voltage Reading: ");
        Serial.print(correctedVoltage); // output voltage measurement
        Serial.println(" V");

        //reads user input to change scaling factor or exit loop
        if(Serial.available()){
           String temp0 = Serial.readString();
           float temp = temp0.toFloat();
           
           if(temp > 0 && temp <= 28.60){                 // 28.6v is the largest value the voltage sensor can handle with the resistors in use
            voltageCalibrationFactor = temp/correctedVoltage;
           }
           else if(temp == 0){
              Serial.println("Exiting Voltage Calibration");
              break;
           }
        }
        delay(100);
      }
    }
    
   /*   If multipliers can be accurate for each voltage range(need more testing) 
    *   this would be a nice implementation so a multimeter was not needed to    
    *   calibrate each time. Possibly pair with analog calibration to zero to make
    *   accurate?
        
      while(Serial.available() == 0){
        String temp = Serial.read();
        if(temp == "6s") {
          voltageCalibrationFactor = 1.0238;
        }
        else if(temp == "5s") {
          voltageCalibrationFactor = 1.02455;
        }
        else if(temp == "4s") {
          voltageCalibrationFactor = 1.0264;
        }
        else if(temp == "3s") {
          voltageCalibrationFactor = 1.029;
        }
        else if(temp == "2s") {
          voltageCalibrationFactor = 1.033;
        }
        else if(temp == "1s") {
          voltageCalibrationFactor = 1.03883;
        }
      }
      Serial.read();

    }

   */
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Throttle sweep test procedure. steps through throttle values up to 100% and back down
    else if(tempString == "sweep"){
      Serial.println("Throttle Sweep:");
      Serial.println("Type 'x' to kill sequence");
      Serial.println("Type 'c' to start sequence");
      Serial.println("Time, Throttle, Thrust, Torque, Current, Voltage, Power");
      while(Serial.available() == 0){}
      double tStep = 0.02; //throttle step size in percentage
      int throttle = 1000; //pulsewidth in microsecond, 0% throttle
      int reverse = 1/tStep; // calculates the total number of steps before it needs to decrease throttle
      int count = 0; // initializes a counter
      bool kill = 0; // initializes kill command boolean
      double startTime = millis();
      // enters a loop to sweep through throttle positions until throttle comes back to 0
      while(count < 2/tStep){
        // resets time for each throttle setting
        setTime(0);
        //updates throttle to new value
        esc.writeMicroseconds(throttle);
        // loops to output data until timer reaches step time
        while(now() < 1){ //set step time for each throttle step
          //outputs throttle value
          Serial.print((millis()-startTime)/1000);
          Serial.print(",   ");
          Serial.print((throttle-1000)/10);
          Serial.print(",   ");
          // call dataCollect function to output voltage, current, forces, and rpm
          dataCollect();
          //check if kill command is sent
          if(Serial.available()){
            char temp = Serial.read();
            // if kill command is sent, it will break both loops and stop the motor
            if(temp == 'x'){
              kill = 1;
              esc.writeMicroseconds(1000);
              Serial.println("Exiting Sweep Procedure");
              break;
            }
          }
        }
        // increase throttle on the way up
        if(count < reverse){
          throttle += tStep * 1000;
        }
        //decrease throttle on the way down
        else if(count >= reverse){
          throttle -= tStep * 1000;
        }
        //break throttle loop if kill command sent
        if(kill){
          break;
        }
        ++count;
      }
    }

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // throttle hold procedure
    else if(tempString == "hold"){
      Serial.println("Throttle Hold;");
      // asks for throttle setting to hold
      Serial.println("Enter desired throttle (1-100)");
      while(Serial.available() == 0){
        inputThrottle = Serial.parseInt();
      }
      Serial.read();
      throttle = 1000 + inputThrottle * 10; // calculates throttle command pulsewidth
      // asks for time to hold throttle setting for
      Serial.println("Enter desired time (in seconds)");
      while(Serial.available() == 0){
        testTime = Serial.parseInt();
      }
      Serial.read();
      Serial.println("To kill hold sequence type 'x'");
      Serial.println("Type 'c' to start sequence");
      Serial.println("Time, Throttle, Thrust, Torque, Current, Voltage, Power"); 
      while(Serial.available() == 0){
        Serial.read();
      }
      //starts motor
      esc.writeMicroseconds(throttle);
      // sets time to 0
      setTime(0);
      // outputs data for given time
      double startTime = millis();
      while(now() < testTime){
        //outputs current time in seconds
        Serial.print((millis()-startTime)/1000);
        Serial.print(",    ");
        Serial.print(inputThrottle);
        Serial.print(",    ");
        // outputs data from dataCollect function
        dataCollect();
        // checks for kill command and breaks loop and cuts throttle
        if(Serial.available()){
          char temp = Serial.read();
          if(temp == 'x'){
            esc.writeMicroseconds(1000);
            Serial.println("Exiting Hold Procedure");
            break;
          }
          else{}
        }
      }
      esc.writeMicroseconds(1000);
    }

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // throttle repeat procedure to repeat the last hold procedure performed
    else if(tempString == "repeat"){
      Serial.println("Throttle Hold;");
      // print throttle setting to hold
      Serial.print("Holding at ");
      Serial.print(inputThrottle);
      Serial.println("% throttle");
      // print time to hold throttle setting for
      Serial.print("Holding for ");
      Serial.print(testTime);
      Serial.println(" seconds");
      Serial.println("");
      delay(1000);
      Serial.println("To kill hold sequence type 'x'");
      Serial.println("Type 'c' to start sequence");
      Serial.println("Time, Throttle, Thrust, Torque, Current, Voltage, Power"); 
      while(Serial.available() == 0){
        Serial.read();
      }
      //starts motor
      esc.writeMicroseconds(throttle);
      // sets time to 0
      setTime(0);
      // outputs data for given time
      double startTime = millis();
      while(now() < testTime){
        //outputs current time in seconds
        Serial.print((millis()-startTime)/1000);
        Serial.print(",    ");
        Serial.print(inputThrottle);
        Serial.print(",    ");
        // outputs data from dataCollect function
        dataCollect();
        // checks for kill command and breaks loop and cuts throttle
        if(Serial.available()){
          char temp = Serial.read();
          if(temp == 'x'){
            esc.writeMicroseconds(1000);
            Serial.println("Exiting Repeated Hold Procedure");
            break;
          }
          else{}
        }
      }
      esc.writeMicroseconds(1000);
    }

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // ESC calibration procedure
    else if(tempString == "esc"){
      Serial.println();
      Serial.println("plug in esc and type 'c' to start calibration");
      
      // waits until enter is pressed to drop the throttle
      while(Serial.available() == 0){}
      //sets full throttle esc is plugged in
      esc.writeMicroseconds(2000);
      Serial.println("Holding maximum throttle for 3 seconds");
      //waits a given time once the ESC is powered on and then goes to 0% throttle
      delay(3000);
      Serial.println("Cutting throttle to 0%");
      esc.writeMicroseconds(1000);
      delay(1000);
      Serial.println("Calibration Complete");
      delay(1000);
    }

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Sensor Monitor
    else if(tempString == "monitor"){
      Serial.println("Sensor monitor. Type 'x' to exit");
      delay(2000);
      while(1 == 1){
        dataCollect();
        if(Serial.available()){
          if(Serial.read() == 'x'){
            Serial.println("Exiting Data Monitor");
            break;
          }
        }
        delay(100);
      }
    }
    
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    instructions(); // print main menu instructions and prep for new command
    Serial.read(); // weird workaround for serial.available to work in the "if" statement at the top of the void loop
    Serial.read();
  }
}


// data collection function. outputs comma-separated data points 
void dataCollect(){
  ////// Output Force //////
  Serial.print(-force.get_units());
  Serial.print(",  ");
  
  ////// Output Torque //////
  Serial.print(torque.get_units());
  Serial.print(",   ");

  
  ////// Calculate and output current //////
  analogCurrent = analogRead(A1);
  //Serial.print("AnalogCurrent: ");
  //Serial.print(analogCurrent);
  //Serial.print(", ");
  // adjusts analog data to current measurement (specified for 75A sensor. For 50A sensor change 178.57 to 125)
  correctedCurrent = ((analogCurrent - (434 - currentCalibrationFactor)) * 178.57) / 1023.0;

  //filter out erroneous negative values when no current is applied
  if(correctedCurrent <= 0.1) {
    correctedCurrent = 0.00;
  }

  // average over last two readings to smooth
  averageCurrent = (correctedCurrent + lastCurrent) / 2.00;
  
  //output current
  Serial.print(averageCurrent);
  Serial.print(",   ");


  ////// Calculate and output voltage //////
  analogVoltage = analogRead(A0);
  vout = (analogVoltage * 5) / 1023.0; //mult by 5 bc 5v is the reference voltage and divide by 1023 to get actual voltage in a 5v range
  
  vin = (vout*(R1 + R2))/R2; //bc we want to mesaure more than 5v we use a volatge divider and this formula takes the lowered voltage 
                             //from the output of the divider and corrects it to the voltage at the postiive terminal of the divider

  correctedVoltage = vin * voltageCalibrationFactor; //1.0238; // * 1.029 for 3s; //multiplication factor to calibrate 
  averageVoltage = (correctedVoltage + lastVoltage) / 2.00;
  
  Serial.print(averageVoltage);
  Serial.print(",  ");
  Serial.print(averageVoltage * averageCurrent);
  Serial.println(", ");

  lastCurrent = averageCurrent;
  lastVoltage = averageVoltage;



  //coles tachometer
  // take direct reading from hall sensor and calibrate to ~ zero (need actual calibration for integration)
  int mag_reading = analogRead(hall_sensor) - 512;
  millis();
  //if (magents_passed = 0) {
  //  millis();
  //}

  //delay(10);
  //Serial.println("I got here 1"); //debugging
  
  // count number of times that reading has switched between pos and neg which mirrors the reversed polarity magnets passing the sensor
  if ((mag_reading >= 5 && last_reading < -5) || (mag_reading < -5 and last_reading >= 5)) {
    
    //Serial.println("I got here 2");  //debugging
    
    magnets_passed += 1;
    //Serial.print("Magnetic Reading: ");
    //Serial.println(mag_reading);
    //Serial.print("Magents Passed: ");
    //Serial.println(magnets_passed);
  }
  
  if (magnets_passed == 14) {
    // convert the rpm from rev/milli to rev/min
    rpm = (60000/(millis()-time_at_last_reset));
    //Serial.print("RPM: ");
    //Serial.println(rpm);
    time_at_last_reset = millis();
    //Serial.print("Magents Passed: ");
    //Serial.println(magnets_passed);
    //Serial.print("time at last reset: ");
    //Serial.println(time_at_last_reset);
    magnets_passed = 0;
  }
  else {
    //Serial.println("");
  }

  //Serial.print("Magnetic Reading: ");
  //Serial.println(mag_reading);
  
  last_reading = mag_reading;
  //delay(500);


  
  /*
  ////// Calculate and output RPM //////
  LastTimeCycleMeasure = LastTimeWeMeasured;  // Store the LastTimeWeMeasured in a variable
  CurrentMicros = micros();  // Store the micros() in a variable

  if(CurrentMicros < LastTimeCycleMeasure)
  {
    LastTimeCycleMeasure = CurrentMicros;
  }

  FrequencyRaw = 10000000000 / PeriodAverage;  // Calculate the frequency using the period between pulses

  // Detect if pulses stopped or frequency is too low, so we can show 0 Frequency:
  if(PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra)
  {  // If the pulses are too far apart that we reached the timeout for zero:
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;  // Change the threshold a little so it doesn't bounce.
  }
  else
  {
    ZeroDebouncingExtra = 0;  // Reset the threshold to the normal value so it doesn't bounce.
  }

  FrequencyReal = FrequencyRaw / 10000;  // Get frequency without decimals

  RPM = FrequencyRaw / PulsesPerRevolution * 60;  // Frequency divided by amount of pulses per revolution multiply by
                                                  // 60 seconds to get minutes
  RPM = RPM / 10000;  // Remove the decimals
  
  // Smoothing RPM:
  total = total - readings[readIndex];  // Advance to the next position in the array
  readings[readIndex] = RPM;  // Takes the value that we are going to smooth
  total = total + readings[readIndex];  // Add the reading to the total
  readIndex = readIndex + 1;  // Advance to the next position in the array

  if (readIndex >= numReadings)  // If we're at the end of the array:
  {
    readIndex = 0;  // Reset array index.
  }
  
  // Calculate the average:
  average = total / numReadings;  // The average value it's the smoothed result

  Serial.println(average); //print tachometer
  
  */
}

/*
void Pulse_Event()  // The interrupt runs this to calculate the period between pulses:
{

  PeriodBetweenPulses = micros() - LastTimeWeMeasured;  // Current "micros" minus the old "micros" when the last pulse happens.
                                                        // This will result with the period (microseconds) between both pulses.
                                                        // The way is made, the overflow of the "micros" is not going to cause any issue.

  LastTimeWeMeasured = micros();  // Stores the current micros so the next time we have a pulse we would have something to compare with.


  if(PulseCounter >= AmountOfReadings)  // If counter for amount of readings reach the set limit:
  {
    PeriodAverage = PeriodSum / AmountOfReadings;  // Calculate the final period dividing the sum of all readings by the
                                                   // amount of readings to get the average.
    PulseCounter = 1;  // Reset the counter to start over. The reset value is 1 because its the minimum setting allowed (1 reading).
    PeriodSum = PeriodBetweenPulses;  // Reset PeriodSum to start a new averaging operation.


    // Change the amount of readings depending on the period between pulses.
    // To be very responsive, ideally we should read every pulse. The problem is that at higher speeds the period gets
    // too low decreasing the accuracy. To get more accurate readings at higher speeds we should get multiple pulses and
    // average the period, but if we do that at lower speeds then we would have readings too far apart (laggy or sluggish).
    // To have both advantages at different speeds, we will change the amount of readings depending on the period between pulses.
    // Remap period to the amount of readings:
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);  // Remap the period range to the reading range.
    // 1st value is what are we going to remap. In this case is the PeriodBetweenPulses.
    // 2nd value is the period value when we are going to have only 1 reading. The higher it is, the lower RPM has to be to reach 1 reading.
    // 3rd value is the period value when we are going to have 10 readings. The higher it is, the lower RPM has to be to reach 10 readings.
    // 4th and 5th values are the amount of readings range.
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);  // Constrain the value so it doesn't go below or above the limits.
    AmountOfReadings = RemapedAmountOfReadings;  // Set amount of readings as the remaped value.
  }
  else
  {
    PulseCounter++;  // Increase the counter for amount of readings by 1.
    PeriodSum = PeriodSum + PeriodBetweenPulses;  // Add the periods so later we can average.
  }

}
*/

void instructions(){
  Serial.println();
  Serial.println("Sensor Package");
  Serial.println("Enter 'loadcal' to enter Load Cell Calibration");
  Serial.println("Enter 'tachcal' to enter Tachometer Calibration");
  Serial.println("Enter 'currcal' to enter Current Sensor Calibration");
  Serial.println("Enter 'voltcal' to enter Voltage Sensor Calibration");
  Serial.println("Enter 'monitor' to enter Sensor Monitor");
  Serial.println("Enter 'esc' to calibrate ESCs");
  Serial.println("Enter 'sweep' to enter Throttle sweep procedure");
  Serial.println("Enter 'hold' to enter Throttle hold mode");
  Serial.println("Enter 'repeat' to repeat the previous Throttle hold");
}
