// include the library code:
#include <LiquidCrystal.h>

int kolumna = 0;
int czekanie = 1000;

//Inicjalizacja połączeń
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() { 
// Wybór rodzaju wyświetlacza  - 16x2
lcd.begin(16, 2); 
//Przesłanie do wyświetlania łańcucha znaków hello, world!
}
 
void loop(){ 
  czekanie = 1000;
  
  for (kolumna = 0; kolumna < 11 ; kolumna++)
  {
    lcd.clear();
    lcd.setCursor(kolumna, 0);
    lcd.print("Lukasz");
    delay(czekanie);
    czekanie = czekanie - 50;
  }
  for (kolumna = 9; kolumna > 0 ; kolumna--)
  {
    lcd.clear();
    lcd.setCursor(kolumna, 0);
    lcd.print("Lukasz");
    delay(czekanie);
    czekanie = czekanie + 50;
  }
}
