//AERO 405 Thrust Test Stand Code-Remote Screen
// Cameron Gable 3/16/2021

//------------------------------------------Libraries
// Adafruit SSD1306 - Version: 2.4.3
  #include <Adafruit_SSD1306.h>
  #include <splash.h>
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>

//nRF24L01
  //#include <SPI.h> //Shared Dependancy
  #include <nRF24L01.h>
  #include <RF24.h>

//------------------------------------------Global Set Up
//Screen
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  #define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  #define Pos1 63 //Define Repeatable Line Positions
  #define Select1Pos 100
  #define Select2Pos 105
  #define Select3Pos 110
  #define SelectPercentPost 115
  
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
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    //Set Global Text Characteristics
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      
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
  DisplaySelectedPage();
  display.setCursor(Select1Pos,Pos1);
  if (!Press1){
    display.print(SelectRange[SelectIterator]);
  }else{
    display.print(Value3);
  }
  
  display.setCursor(Select2Pos,Pos1);
  if (!Press2){
    display.print(SelectRange[SelectIterator]);
  }else{
    display.print(Value2);
  }
  
  display.setCursor(Select3Pos,Pos1);
  if (!Press3){
    display.print(SelectRange[SelectIterator]);
  }else{
    display.print(Value3);
  }
  display.setCursor(SelectPercentPost,Pos1);
  display.print('%');
}

//------------------------------------------Menu Functions
void SplashScreen(void) {
  PageSelector = 0;
  display.clearDisplay();
  display.setCursor(0,Pos1);
  display.print("Press A Button");  
  display.display();
}

void InitialThrottleScreen(void) {
  PageSelector = 1;
  display.clearDisplay();
  display.setCursor(0,Pos1);
  display.print("Initial Throttle: ");  
  display.display();
}

void SecondaryThrottleScreen(void) {
  PageSelector = 2;
  display.clearDisplay();
  display.setCursor(0,Pos1);
  display.print("Secondary Throttle: ");  
  display.display();
}

void TestTimeScreen(void) {
  PageSelector = 3;
  display.clearDisplay();
  display.setCursor(0,Pos1);
  display.print("Test Time: ");  
  display.display();
}

void StepSwitchScreen(void) {
  PageSelector = 4;
  display.clearDisplay();
  display.setCursor(0,Pos1);
  display.print("Step(TS)/Switch(ST) Time: ");  
  display.display();
}

void ConfirmationScreen(void) {
  ResetButtonState();
  PageSelector = 5;
  display.clearDisplay();
  display.setCursor(0,Pos1);
  display.print("Start Test?: ");
  display.setCursor(60,Pos1);
  display.print(MenuNumber);  
  display.display();
  while(!SelectionButtonState){
  }
}

//------------------------------------------Library Functions
void Error ( int encoderNum ) {
    Serial.print("ERROR creating encoder "); Serial.print(encoderNum); Serial.println("!");
    Serial.println("Possible malloc error, the rotary ID is incorrect, or too many rotaries");
    while(1);   // Halt and catch fire
}
