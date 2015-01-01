// Ustalenie numerów pinów
#define DIRECTION_A 12
#define SPEED_A 3
#define BREAK_A 9
#define CURRENT 0

#define DIRECTION_B 13
#define SPEED_B 11
#define BREAK_B 8


// Zmienna przechowująca kierunek obrotów
byte dir = LOW;

void setup()
{
  // Ustawienie kierunku wyjść cyfrowych
  pinMode(DIRECTION_A, OUTPUT);
  pinMode(SPEED_A, OUTPUT);
  pinMode(BREAK_A, OUTPUT);
  
  pinMode(DIRECTION_B, OUTPUT);
  pinMode(SPEED_B, OUTPUT);
  pinMode(BREAK_B, OUTPUT);
  
  // Włączenie hamulca
  digitalWrite(BREAK_A, HIGH);
  digitalWrite(BREAK_B, HIGH);
  
  // Ustawienie kierunku
  digitalWrite(DIRECTION_A, dir);
  digitalWrite(DIRECTION_B, dir);
}

void Jedz(int predkosc, char kierunek)
{
  digitalWrite(BREAK_A, LOW);
  digitalWrite(BREAK_B, LOW);

  if(kierunek == HIGH)
  {
    digitalWrite(DIRECTION_A, HIGH);
    digitalWrite(DIRECTION_B, LOW);
  }
  else
  {
    digitalWrite(DIRECTION_A, LOW);
    digitalWrite(DIRECTION_B, HIGH);
  }

  analogWrite(SPEED_A, predkosc);
  analogWrite(SPEED_B, predkosc);
}

void Hamuj()
{
  digitalWrite(BREAK_A, HIGH);
  digitalWrite(BREAK_B, HIGH);
}

void Skrec(int kierunek, int predkosc)
{
  if(kierunek == HIGH)
  {
    digitalWrite(BREAK_A, HIGH);
    digitalWrite(BREAK_B, LOW);

    digitalWrite(DIRECTION_A, kierunek);

    analogWrite(SPEED_A, predkosc);
  }
  else
  {
    digitalWrite(BREAK_A, LOW);
    digitalWrite(BREAK_B, HIGH);

    digitalWrite(DIRECTION_B, ~kierunek);

    analogWrite(SPEED_B, predkosc);
  }
}
 
void loop()
{
  Jedz(150, LOW);
  delay(3000);
  Skrec(HIGH, 150);
  delay(1000);
}
