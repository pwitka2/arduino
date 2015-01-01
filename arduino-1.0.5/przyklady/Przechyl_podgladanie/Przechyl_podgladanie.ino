/*
Program do przekazywania stanu czujnika do komputera
*/

// nazwanie nozki do ktorej jest podlaczony czujnik przechylenia
int pushczujnik = 2;

// funkcja wywolywana raz na poczatku
void setup() {
  // ustawienie predkosci komunikacji na 9600 baudow:
  Serial.begin(9600);
  //ustawienie Pina przycisku na wejscie - INPUT
  pinMode(pushczujnik, INPUT);
}

// funkcja, petla
void loop() {
  // odczytaj stan czujnika
  int czujnikState = digitalRead(pushczujnik);
  // wyslij stan czujnika do komputera
  Serial.println(czujnikState);
  delay(1);        // krotkie opoznienie
}



