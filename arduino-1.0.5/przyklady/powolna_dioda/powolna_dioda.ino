/*
Program powoli zmieniajacy jasnosc diody
 z wykorzystaniem funkcji analogWrite 
 
 
 This example code is in the public domain.
 */

int led = 9;           // gdzie jest podlaczona dioda
int brightness = 0;    // zmienna okreslajaca jasnosc diody
int fadeAmount = 5;    // co ile ma sie zmieniac jasnosc

// ustawienie Arduina (wywolywane raz)
void setup()  { 
  // pin diody ustawiony na wyjscie
  pinMode(led, OUTPUT);
} 

// funkcja pracujaca w petli
void loop()  { 
  // ustaw jasnosc diody na wartosc okreslona przez brightness
  analogWrite(led, brightness);    

  // zmien wartosc brightness o wartosc ustalona w fadeAmount
  brightness = brightness + fadeAmount;

  // jesli dioda sie w pelni zapali albo zgasnie, niech teraz
 // zmienia sie jasnosc w drugim kierunku 
  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount ; 
  }     
  // odczekaj 30ms    
  delay(30);                            
}

