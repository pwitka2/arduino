/*
 // przyklad zalaczania diody z Arduino
 */
int led = 13; //nozka do ktorej jest podlaczona dioda
int led2 = 9;

// setup - funkcja, ktora zostanie wywolana raz
void setup() {                

  pinMode(led, OUTPUT); //ustawienie nozki o nazwie led tak, aby 
  pinMode(led2, OUTPUT); //ustawienie nozki o nazwie led tak, aby 
  //mogla zapalac diode: tryb OUTPUT  
}

// loop, funkcja ktora po dojsciu do konca wywola sie
// znowu od poczatku - w petli
void loop() {
  digitalWrite(led2, LOW);
  digitalWrite(led, HIGH);   // ustawienie nozki o nazwie led w stan WYSOKI - HIGH
  // czyli 5V
  delay(1000);               // 1000ms - 1s czekanie
  digitalWrite(led, LOW);    // ustawienie nozki diodki w stan niski - 0V
  digitalWrite(led2, HIGH);
  delay(1000);               // 1000ms - 1s czekanie
}

