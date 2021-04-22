
  #include <Servo.h>

  int ESC_PIN = 6;
  Servo esc;
  
  #define IoutPin A1 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
   while (!Serial) {
      Serial.begin(9600);
   }
  esc.attach(ESC_PIN);
  esc.writeMicroseconds(1000);
  pinMode(IoutPin, INPUT); 
}

int Throttle = 0;
int start = 0;
int sum = 0;
int val = 0;
double avg = 0;
int count = 0;
void loop() {
  sum = 0;
  avg = 0;
  count = 0;
esc.writeMicroseconds(1000);
Serial.println("waiting for Throttle");
while(Serial.available() == 0){
}
Throttle = Serial.parseInt();
Serial.println(Throttle);
Serial.println("starting");
esc.writeMicroseconds(1000+Throttle*10);
start = millis();s
while(1==1){
    val = analogRead(IoutPin);
    sum += val;
    count++;
    avg = sum/count;
    Serial.print(avg);
    Serial.print(", ");
    Serial.println(val);
    if(Serial.available()){
      break;
    }
    delay(100);
}
Serial.read();
}
