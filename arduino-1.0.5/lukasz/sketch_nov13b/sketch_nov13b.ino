int laserPin = 7;

void setup()
{
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, HIGH);
}

void loop()
{
  digitalWrite(laserPin, HIGH);
  delay(2000);
  digitalWrite(laserPin, LOW);
  delay(2000);
}
