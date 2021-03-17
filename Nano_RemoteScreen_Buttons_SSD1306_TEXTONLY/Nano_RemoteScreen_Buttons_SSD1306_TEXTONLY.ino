//AERO 405 Thrust Test Stand Code-Remote Screen
// Cameron Gable 3/16/2021

//------------------------------------------Libraries
// SSD1306 Text Only
  #include <SPI.h>
  #include <SSD1306_text.h>
  #include <stdlib.h>


//nRF24L01
  //#include <SPI.h> //Shared Dependancy
  #include <nRF24L01.h>
  #include <RF24.h>

//------------------------------------------Global Set Up
//Screen Text Only
  // Hardware SPI pins include D11=Data and D13=Clk
  #define OLED_DC 7
  #define OLED_CS 0
  #define OLED_RST 10
  SSD1306_text oled(OLED_DC, OLED_RST, OLED_CS);
  
//nRF24L01: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
  RF24 radio(9, 8); // CE, CSN //create an RF24 object
  const byte address[6] = "00001"; //address through which two modules communicate.
  char MenuNumber[] = {00000000000000};
  char A = {0};
  char BBB[] = {000};
  char CCC[] = {000};
  char DDD[] = {000};
  char EEE[] = {000};
  
//Buttons
  const int ThrottleSweepButtonPin = 2;     //Rework Pins
  const int ConstantThrottleButtonPin = 3;  //Rework Pins
  const int StressTestButtonPin = 4;        //Rework Pins
  const int LeftButtonPin = 5;
  const int RightButtonPin = 6;
  const int SelectionButtonPin = 7;

  int ConstantThrottleButtonState = 0;
  int ThrottleSweepButtonState = 0;
  int StressTestButtonState = 0;
  int LeftButtonState = 0;
  int RightButtonState = 0;
  int SelectionButtonState;

//General
  int PageSelector = 0;

  char Value1 = '0'; //Number Selection Values
  char Value2 = '0';
  char Value3 = '0';

  char buf[3];
  const char SelectRange[] = {'0','1','2','3','4','5','6','7','8','9'};
  int SelectIterator = 0;

  bool Press1 = false;
  bool Press2 = false;
  bool Press3 = false;
  
void setup() {
  while (!Serial) {
    Serial.begin(9600);
  }
  //------------------------------------------Screen Set Up
    oled.init();
    oled.clear();
    oled.setTextSize(1,1);        // 5x7 characters, pixel spacing = 1
    oled.write("Hello world!");
    
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
  
}

void loop() {
  ThrottleSweepButtonState = digitalRead(ThrottleSweepButtonPin);
  ConstantThrottleButtonState = digitalRead(ConstantThrottleButtonPin);
  StressTestButtonState = digitalRead(StressTestButtonPin);
  
  if (ThrottleSweepButtonState == HIGH){
    A = '1';
    GatherMenuID();
    Confirmation();
    SendMenuID();
  }else if (ConstantThrottleButtonState == HIGH){
    A= '2';
    GatherMenuID();
    Confirmation();
    SendMenuID();
    
  }else if (StressTestButtonState == HIGH){
    A = '3';
    GatherMenuID();
    Confirmation();
    SendMenuID();
    
  }
  
}

//------------------------------------------Helper Functions

char NumberSelection() {
  Value1 = '0';
  Value2 = '0';
  Value3 = '0';
  Press1 = false;
  Press2 = false;
  Press3 = false;
  SelectionButtonState = 0;
  SelectIterator = 0;
  
  while(!SelectionButtonState){
    LeftButtonState = digitalRead(LeftButtonPin);
    RightButtonState = digitalRead(RightButtonPin);
    SelectionButtonState = digitalRead(SelectionButtonPin);
    if(LeftButtonState){
      SelectIterator = SelectIterator -1;
    }else if(RightButtonState){
      SelectIterator = SelectIterator +1;
    }
    SelectIteratorRefine();
    Value1 = SelectRange[SelectIterator];
    ResetButtonState();
    DisplaySelectedValue();
  }
  Press1 = true;
  SelectionButtonState = 0;
  SelectIterator = 0;
  
  while(!SelectionButtonState){
    LeftButtonState = digitalRead(LeftButtonPin);
    RightButtonState = digitalRead(RightButtonPin);
    SelectionButtonState = digitalRead(SelectionButtonPin);
    if(LeftButtonState){
      SelectIterator = SelectIterator -1;
    }else if(RightButtonState){
      SelectIterator = SelectIterator +1;
    }
    SelectIteratorRefine();
    Value2 = SelectRange[SelectIterator];
    ResetButtonState();
    DisplaySelectedValue();
  }
  Press2 = true;
  SelectionButtonState = 0;
  SelectIterator = 0;
  
  while(!SelectionButtonState){
    LeftButtonState = digitalRead(LeftButtonPin);
    RightButtonState = digitalRead(RightButtonPin);
    SelectionButtonState = digitalRead(SelectionButtonPin);
    if(LeftButtonState){
      SelectIterator = SelectIterator -1;
    }else if(RightButtonState){
      SelectIterator = SelectIterator +1;
    }
    SelectIteratorRefine();
    Value2 = SelectRange[SelectIterator];
    ResetButtonState();
    DisplaySelectedValue();
  }
  Press3 = true;
  SelectionButtonState = 0;
  SelectIterator = 0;
  buf[2];

  strcpy(buf,Value1);
  strcpy(buf,Value2);
  strcpy(buf,Value3);
  
  return buf;
}

void ResetButtonState(void) {
  ThrottleSweepButtonState = 0;
  ConstantThrottleButtonState = 0;
  StressTestButtonState = 0;
  LeftButtonState = 0;
  RightButtonState = 0;
}

void SelectIteratorRefine(){
  if(SelectIterator < 0){
    SelectIterator = 8;
  }
  if(SelectIterator > 8){
    SelectIterator = 0;
  }
}

void SendMenuID(){
  radio.write(&MenuNumber, sizeof(MenuNumber));
  delay(1000);
}

void GatherMenuID(){
  ResetButtonState();
  InitialThrottleScreen();
  strcpy(BBB, NumberSelection());
  SecondaryThrottleScreen();
  strcpy(CCC, NumberSelection());
  TestTimeScreen();
  strcpy(DDD, NumberSelection());
  StepSwitchScreen();
  strcpy(EEE, NumberSelection());
  strcpy(MenuNumber,A);
  strcpy(MenuNumber,BBB);
  strcpy(MenuNumber,CCC);
  strcpy(MenuNumber,DDD);
  strcpy(MenuNumber,EEE);
}

void Confirmation(){
  ConfirmationScreen();
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
  oled.setCursor(3, 10);
  DisplaySelectedPage();
  if (!Press1){
    oled.write(SelectRange[SelectIterator]);
  }else{
    oled.write(Value3);
  }
  oled.setCursor(3, 17);
  if (!Press2){
    oled.write(SelectRange[SelectIterator]);
  }else{
    oled.write(Value2);
  }
  oled.setCursor(3, 24);
  if (!Press3){
    oled.write(SelectRange[SelectIterator]);
  }else{
    oled.write(Value3);
  }
  oled.setCursor(3, 31);
  oled.write('%');
  oled.setCursor(0, 0);
}

//------------------------------------------Menu Functions
void SplashScreen(void) {
  PageSelector = 0;
  oled.clear();
  oled.write("Press A Button");  
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
  oled.clear();
  oled.write("Start Test?: ");
  oled.setCursor(3, 10);
  oled.write(MenuNumber);  
  
  while(!SelectionButtonState){
  }
}

//------------------------------------------Library Functions
void Error ( int encoderNum ) {
    Serial.print("ERROR creating encoder "); Serial.print(encoderNum); Serial.println("!");
    Serial.println("Possible malloc error, the rotary ID is incorrect, or too many rotaries");
    while(1);   // Halt and catch fire
}
