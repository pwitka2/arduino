/*
  Wysylanie danych do komputera
 
 Przyklad wysylania danych z czujniczka analogowego ( np. odleglosci
 albo natezenia swiatla) i wysylania ich do komputera
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 modified 9 Apr 2012
 modified 10 Oct 2013
 by Tom Igoe
 by Igor Zubrycki
 
 This example code is in the public domain.
 
*/
const int analogInPin = A0;  // nazwa lub numer nozki do ktorej podlaczony jest czujnik

int sensorValue = 0;        //odczytana wartosc z czujnika

void setup() {
 // ustaw komunikacje
  Serial.begin(9600); 
}

void loop() { // odczytywanie wartosci w petli
  sensorValue = analogRead(analogInPin);            
                       
  Serial.println(sensorValue);      // wysylanie wartosci do komputera
  delay(2);                     
}
