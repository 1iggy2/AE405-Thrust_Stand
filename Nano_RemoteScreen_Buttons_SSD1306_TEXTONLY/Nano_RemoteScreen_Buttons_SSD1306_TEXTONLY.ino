//AERO 405 Thrust Test Stand Code-Remote Screen
// Cameron Gable 3/16/2021

//------------------------------------------Libraries
// SSD1306 Text Only
  #include <SPI.h>
  #include <SSD1306_text_I2C.h>
  #include <ssdfont.h>
  #include <stdlib.h>


//nRF24L01
  //#include <SPI.h> //Shared Dependancy
  #include <nRF24L01.h>
  #include <RF24.h>

//------------------------------------------Global Set Up
//Screen Text Only
  // Hardware SPI pins include D11=Data and D13=Clk
  SSD1306_text oled;
  
//nRF24L01: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
  RF24 radio(9, 8); // CE, CSN //create an RF24 object
  const byte address[6] = "00001"; //address through which two modules communicate.
  char MenuNumber[14];
  char A;
  char BBB[3];
  char CCC[3];
  char DDD[3];
  char EEE[3];
  
//Buttons
  const int ThrottleSweepButtonPin = 6;     //"Button 1" -> D6
  const int ConstantThrottleButtonPin = 7;  //"Button 2" -> D7
  const int StressTestButtonPin = 8;        //"Button 3" -> D8
  const int LeftButtonPin = 9;              //"Button 4" -> D9
  const int RightButtonPin = A0;            //"Button 5" -> A0
  const int SelectionButtonPin = A1;        //"Button 6" -> A1

  int ConstantThrottleButtonState = 0;
  int ThrottleSweepButtonState = 0;
  int StressTestButtonState = 0;
  int LeftButtonState = 0;
  int RightButtonState = 0;
  int SelectionButtonState;

//General
  int PageSelector = 0;
  bool Confirmed = false;

  char Value1 = '0'; //Number Selection Values
  char Value2 = '0';
  char Value3 = '0';

  char buf[3] = {'0','0','0'};
  const char SelectRange[] = {'0','1','2','3','4','5','6','7','8','9'};
  int SelectIterator = 0;

  bool Press1 = false;
  bool Press2 = false;
  bool Press3 = false;
  
void setup() { 
  Serial.begin(9600);
  while (!Serial) {
    Serial.begin(9600);
  }
  Serial.println("Setup Screen");
  //------------------------------------------Screen Set Up
    oled.init();
    oled.clear();
    oled.setTextSize(1,1);        // 5x7 characters, pixel spacing = 1
    oled.write("AERO 405 - GO BLUE");
    Serial.println("Waking Up");
    delay(1000);
  //------------------------------------------nRF24L01 Setup
    radio.begin();
    radio.openWritingPipe(address);//set the address
    radio.stopListening();//Set module as transmitter
    
  //------------------------------------------Button Set Up
    pinMode(ThrottleSweepButtonPin,INPUT);
    pinMode(ConstantThrottleButtonPin,INPUT);
    pinMode(StressTestButtonPin,INPUT);
    pinMode(LeftButtonPin,INPUT);
    pinMode(RightButtonPin,INPUT);
    pinMode(SelectionButtonPin,INPUT);
    
  //------------------------------------------Initialize Splash Screen
    SplashScreen();
    Serial.println("Splashed");
}

void loop() {
  ThrottleSweepButtonState = digitalRead(ThrottleSweepButtonPin);
  ConstantThrottleButtonState = digitalRead(ConstantThrottleButtonPin);
  StressTestButtonState = digitalRead(StressTestButtonPin);
  
  if (ThrottleSweepButtonState == HIGH){
    Serial.println("Throttle Sweep Pressed");
    A = '1';
    GatherMenuID();
    Confirmation();
    SendMenuID();
  }else if (ConstantThrottleButtonState == HIGH){
    Serial.println("Constant Throttle Pressed");
    A= '2';
    GatherMenuID();
    Confirmation();
    SendMenuID();
    
  }else if (StressTestButtonState == HIGH){
    Serial.println("Stress Test Pressed");
    A = '3';
    GatherMenuID();
    Confirmation();
    if(Confirmed){
      SendMenuID();
    }else{
      SplashScreen();
    }
    
  }
  
}

//------------------------------------------Helper Functions

void NumberSelection() {
  Serial.println("Number Selection ");
  Serial.print("buf: ");
  Serial.println(buf);
  Value1 = '0';
  Value2 = '0';
  Value3 = '0';
  Press1 = false;
  Press2 = false;
  Press3 = false;
  SelectionButtonState = 0;
  SelectIterator = 0;
  DisplaySelectedValue();

  while(!SelectionButtonState){
    while(!SelectionButtonState && !RightButtonState && !LeftButtonState){
      LeftButtonState = digitalRead(LeftButtonPin);
      RightButtonState = digitalRead(RightButtonPin);
      SelectionButtonState = digitalRead(SelectionButtonPin);
    }
    if(LeftButtonState){
      SelectIterator = SelectIterator -1;
      Serial.println("Left");
      delay(200);
    }else if(RightButtonState){
      SelectIterator = SelectIterator +1;
      Serial.println("Right");
      delay(200);
    }
    SelectIteratorRefine();
    Serial.println(SelectIterator);
    Value1 = SelectRange[SelectIterator];
    Serial.println("New Value 1: ");
    Serial.println(Value1);
    delay(100);
    ResetButtonState();
    DisplaySelectedValue();
  }
  Serial.println("Select Pressed");
  Press1 = true;
  SelectionButtonState = 0;
  SelectIterator = 0;
  delay(500);
  
  while(!SelectionButtonState){
    if(PageSelector < 3 && Value1 == '1'){
      break;
    }
    while(!SelectionButtonState && !RightButtonState && !LeftButtonState){
      LeftButtonState = digitalRead(LeftButtonPin);
      RightButtonState = digitalRead(RightButtonPin);
      SelectionButtonState = digitalRead(SelectionButtonPin);
    }
    if(LeftButtonState){
      SelectIterator = SelectIterator -1;
      delay(200);
    }else if(RightButtonState){
      SelectIterator = SelectIterator +1;
      delay(200);
    }
    SelectIteratorRefine();
    Value2 = SelectRange[SelectIterator];
    ResetButtonState();
    DisplaySelectedValue();
  }
  Press2 = true;
  SelectionButtonState = 0;
  SelectIterator = 0;
  delay(500);
  
  while(!SelectionButtonState){
    if(PageSelector < 3 && Value1 == '1'){
      break;
    }
    while(!SelectionButtonState && !RightButtonState && !LeftButtonState){
      LeftButtonState = digitalRead(LeftButtonPin);
      RightButtonState = digitalRead(RightButtonPin);
      SelectionButtonState = digitalRead(SelectionButtonPin);
    }
    if(LeftButtonState){
      SelectIterator = SelectIterator -1;
      delay(200);
    }else if(RightButtonState){
      SelectIterator = SelectIterator +1;
      delay(200);
    }
    SelectIteratorRefine();
    Value3 = SelectRange[SelectIterator];
    ResetButtonState();
    DisplaySelectedValue();
  }
  Press3 = true;
  SelectionButtonState = 0;
  SelectIterator = 0;
  buf[2];
  delay(500);

  Serial.print("Final Values: ");
  Serial.print(Value1);
  Serial.print(Value2);
  Serial.println(Value3);
  buf[0] = Value1;
  buf[1] = Value2;
  buf[2] = Value3;
  buf[3] = '\0';
  Serial.print("Written Buf: ");
  Serial.println(buf);
}

void ResetButtonState(void) {
  ThrottleSweepButtonState = 0;
  ConstantThrottleButtonState = 0;
  StressTestButtonState = 0;
  LeftButtonState = 0;
  RightButtonState = 0;
}

void SelectIteratorRefine(){
  if(PageSelector > 2 || Press1){
    if(SelectIterator < 0){
      SelectIterator = 9;
    }
    if(SelectIterator > 9){
      SelectIterator = 0;
    }
  }else{
    if(SelectIterator < 0){
      SelectIterator = 1;
    }
    if(SelectIterator > 1){
      SelectIterator = 0;
    }
  }
}

void SendMenuID(){
  radio.write(&MenuNumber, sizeof(MenuNumber));
  delay(1000);
}

void GatherMenuID(){
  Serial.println("GatherMenu ID Loop Entered");
  ResetButtonState();
  InitialThrottleScreen();
  NumberSelection();
  Serial.print("Returned BBB Buf: ");
  Serial.println(buf);
  strncpy(BBB,buf,3);
  Serial.println("BBB Saved As: ");
  Serial.println(BBB);
  SecondaryThrottleScreen();
  NumberSelection();
  Serial.print("Returned CCC Buf: ");
  Serial.println(buf);
  strncpy(CCC,buf,3);
  Serial.println("CCC Saved As:");
  Serial.println(CCC);
  TestTimeScreen();
  NumberSelection();
  Serial.print("Returned DDD Buf: ");
  Serial.println(buf);
  strncpy(DDD,buf,3);
  Serial.println("DDD Saved As");
  Serial.println(DDD);
  StepSwitchScreen();
  NumberSelection();
  Serial.print("Returned EEE Buf: ");
  Serial.println(buf);
  strncpy(EEE,buf,3);
  Serial.println("EEE Saved As:");
  Serial.println(EEE);
  MenuNumber[0] = A;
  MenuNumber[1] = BBB[0];
  MenuNumber[2] = BBB[1];
  MenuNumber[3] = BBB[2];
  MenuNumber[4] = CCC[0];
  MenuNumber[5] = CCC[1];
  MenuNumber[6] = CCC[2];
  MenuNumber[7] = DDD[0];
  MenuNumber[8] = DDD[1];
  MenuNumber[9] = DDD[2];
  MenuNumber[10] = EEE[0];
  MenuNumber[11] = EEE[1];
  MenuNumber[12] = EEE[2];
  MenuNumber[13] = '\0';
}

void Confirmation(){
  ConfirmationScreen();
  Serial.println("Confirmed 2");
}

void DisplaySelectedPage(){
  if (PageSelector = 0){
    SplashScreen();
  }else if (PageSelector = 1){
    InitialThrottleScreen();
  }else if (PageSelector = 2){
    SecondaryThrottleScreen();
  }else if (PageSelector = 3){
    TestTimeScreen();
  }else if (PageSelector = 4){
    StepSwitchScreen();
  }else if (PageSelector = 5){
    ConfirmationScreen();
  }else{
    PageSelector = 0;
    DisplaySelectedPage();
  }
}

void DisplaySelectedValue(){
  oled.setCursor(3,10);
  oled.write(Value1);
  oled.write(Value2);
  oled.write(Value3);
  if (PageSelector == 3 || PageSelector == 4){
    oled.write(" seconds");
  }else{
    oled.write("%");
  }
}

//------------------------------------------Menu Functions
void SplashScreen(void) {
  PageSelector = 0;
  oled.clear();
  oled.write("Press a Test Button");  
}

void InitialThrottleScreen(void) {
  PageSelector = 1;
  oled.clear();
  oled.write("Initial Throttle: ");  
}

void SecondaryThrottleScreen(void) {
  PageSelector = 2;
  oled.clear();
  oled.write("Secondary Throttle: ");  
}

void TestTimeScreen(void) {
  PageSelector = 3;
  oled.clear();
  oled.write("Test Time: ");  
}

void StepSwitchScreen(void) {
  PageSelector = 4;
  oled.clear();
  oled.write("Step(TS)/Switch(ST) Time: ");  
}

void ConfirmationScreen(void) {
  ResetButtonState();
  PageSelector = 5;
  Serial.print("Final Menu ID: ");
  Serial.println(MenuNumber);
  oled.clear();
  oled.write("Start Test?");
  oled.setCursor(2,1);
  oled.write("Enter to Begin, Left to Cancel");
  oled.setCursor(5,0);
  oled.write("MenuID Number:");
  oled.setCursor(6, 10);
  oled.write(MenuNumber);  
  Serial.println("Waiting for Confimation");
  while(!SelectionButtonState && !LeftButtonState){
      LeftButtonState = digitalRead(LeftButtonPin);
      SelectionButtonState = digitalRead(SelectionButtonPin);
  }
  if(LeftButtonState){
    Confirmed = false;
    oled.clear();
    oled.write("Canceled Test");
    delay(2000);
    SplashScreen();
  }else if(SelectionButtonState){
    Serial.println("Confirmed");
    Confirmed = true;
    oled.clear();
    oled.write("Confirmed Test");
    delay(1000);
    oled.clear();
    oled.write("Starting Test");
  }
}

//------------------------------------------Library Functions
void Error ( int encoderNum ) {
    Serial.print("ERROR creating encoder "); Serial.print(encoderNum); Serial.println("!");
    Serial.println("Possible malloc error, the rotary ID is incorrect, or too many rotaries");
    while(1);   // Halt and catch fire
}
