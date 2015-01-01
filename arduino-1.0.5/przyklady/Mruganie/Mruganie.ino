/*
 // przyklad zalaczania diody z Arduino
 */
int led = 13; //nozka do ktorej jest podlaczona dioda

// setup - funkcja, ktora zostanie wywolana raz
void setup() {                
  
  pinMode(led, OUTPUT); //ustawienie nozki o nazwie led tak, aby 
//mogla zapalac diode: tryb OUTPUT  
}

// loop, funkcja ktora po dojsciu do konca wywola sie
// znowu od poczatku - w petli
void loop() {
  digitalWrite(led, HIGH);   // ustawienie nozki o nazwie led w stan WYSOKI - HIGH
  // czyli 5V
  delay(1000);               // 1000ms - 1s czekanie
  digitalWrite(led, LOW);    // ustawienie nozki diodki w stan niski - 0V
  delay(1000);               // 1000ms - 1s czekanie
}
