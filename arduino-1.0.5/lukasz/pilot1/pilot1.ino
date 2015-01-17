#include <IRremote.h>
#define irPin 4
IRrecv irrecv(irPin);
decode_results results;
 
void setup() {
   Serial.begin(9600);
   Serial.print("Czesc");
   pinMode(7, OUTPUT);
   pinMode(10, OUTPUT);
   digitalWrite(7, HIGH);
   irrecv.enableIRIn();
}
 
void loop() {
   if (irrecv.decode(&results)) {
      Serial.print("0x");
      Serial.println(results.value, HEX);
      /*if(results.value == 0xC26BF044)
      {
        digitalWrite(7, LOW);
        digitalWrite(10, HIGH);
      }
      if(results.value == 0xC4FFB646)
      {
        digitalWrite(7, HIGH);
        analogWrite(10, 50);
      }
      */
      //delay(250);
      irrecv.resume();
      }
   //Serial.println(".");
}
