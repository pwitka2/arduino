/*

 Program do odczytywania stanu czujnika przechylenia
 i na tej podstawie zapalania lub gaszenia diodki
 
 
 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe
 translated 10 Oct 2013
 by Igor Zubrycki
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/czujnik
 */

// ustawienie czytelnych nazw
const int czujnikPin = 2;     // Pin do ktorego jest podlaczony czujnik przechylenia
const int ledPin =  13;      // Nr Pin'u do ktorego jest podlaczona dioda

// zmienna, gdzie zapisywany bedzie stan czujnika (WYSOKI lub NISKI)
int czujnikState = 0;         // stan czujnika

void setup() {
  // ustawienie nozki dla diody na WYJSCIE - OUTPUT
  pinMode(ledPin, OUTPUT);      
  // ustawienie nozki czujnika na WEJSCIE - INPUT
  pinMode(czujnikPin, INPUT);     
}

void loop(){
  // odczytaj stan przycisku
  czujnikState = digitalRead(czujnikPin);

  // sprawdz stan przycisku, jesli wysoki (przechylony, tak ze zwiera) zapal diodke
  if (czujnikState == HIGH) {     
    // zapal diode    
    digitalWrite(ledPin, HIGH);  
  } 
  else {
    // w innym przypadku stan diody ustaw na niski
    digitalWrite(ledPin, LOW); 
  }
}
