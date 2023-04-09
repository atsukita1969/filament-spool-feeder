//for attiny85

//#include <MsTimer2.h>
unsigned int timeCount = 0;

int inPin = 1;
int outPin = 3;

void timerFire() {
  if(digitalRead(inPin)==LOW){
    //timer reset
    timeCount = 0;
      }else {
    //timer count up
          timeCount++;
    }
    }

void setup() {
  digitalWrite(outPin, HIGH);
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(outPin, OUTPUT);
  
 }

void loop() {
  if (digitalRead(inPin) == HIGH){
    
    delay(10);
    timerFire();
    //timer start
    
  }
  
  Serial.print(timeCount);
  Serial.println("");
  
  if (timeCount >3000) { //stop signal after continue 3000count
    digitalWrite(outPin, LOW);
    while(1);
   }
}
