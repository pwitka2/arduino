/*
Program, ktory na podstawie informacji wysylanej z komputera, 
ustawia jasnosc diody
 created 2006
 by David A. Mellis
 modified 30 Aug 2011
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Dimmer
 
 */

const int ledPin = 53;      // nadaj nazwe Pinowi do ktorego 
//jest przyczepiona dioda

void setup()
{
  // rozpocznij komunikacje szeregowa
  Serial.begin(9600);
  // ustaw pin diody na wyjscie
  pinMode(ledPin, OUTPUT);
}

void loop() {
  byte brightness; // zmienna okreslajaca jasnosc diody

  // sprawdz czy wyslano informacje z komputera
  if (Serial.available()) {
    // odczytaj informacje ( bajt)
    brightness = Serial.read();
    // ustaw jasnosc diody na odczytany poziom
    //analogWrite(ledPin, brightness);
    
    if (brightness < 100)
    {
      digitalWrite(ledPin, HIGH);
    }
    else{
      digitalWrite(ledPin, LOW);
    }
      
  }
}

