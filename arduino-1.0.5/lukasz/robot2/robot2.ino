#include "Motor.h"
#include <NewPing.h>

#include <IRremote.h>
#include <Voltage.h>
#include <LiquidCrystal.h>
#include <NewPing.h>
#include <Servo.h>
#include "PinSetup.h"

#define TRUE 1
#define FALSE 0

// Servo definitions
#define SERVO_INITIAL_POS 30
#define SERVO_STEP 15

Servo myservo;  // create servo object to control a servo
struct servoCtx_s 
{
  int pos;
  int servoStep;
} servoCtx;

// Sonar definitions
#define SONAR_MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define SONAR_DELAY_AFTER 5 // 30ms
#define SONAR_READS_NO 5 // No of reads in one call 
#define SONAR_TRESHOLD_DISTANCE 20 // 30 cm 

int sonarDistance;
NewPing sonar(PIN_SONAR_TRIGGER, PIN_SONAR_ECHO, SONAR_MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// LCD definitions
LiquidCrystal lcd(PIN_LCDRS, PIN_LCDE, PIN_LCDDB4, PIN_LCDDB5, PIN_LCDDB6, PIN_LCDDB7);

// IR definitions
#define IR_PRZOD 0x4B3659A6
#define IR_TYL 0x4B36D926
#define IR_STOP 0x4B367986
#define IR_PRAWO 0x4B36B946
#define IR_LEWO 0x4B3639C6

IRrecv irrecv(PIN_IR);
decode_results irRawResults;

// Motor Definitions
#define MOTOR_DIRECTION_FORWARD LOW
#define MOTOR_DIRECTION_BACKWARD HIGH
#define MOTOR_DIRECTION_LEFT HIGH
#define MOTOR_DIRECTION_RIGHT LOW
#define MOTOR_SPEED 100

// Application globals
enum state_s 
{
  STATE_IDLE,
  STATE_MOVING
} state;

#define SIGNAL_IR_NO_SIGNAL 0x0
#define SIGNAL_IR_LEFT 0x1
#define SIGNAL_IR_RIGHT 0x2
#define SIGNAL_IR_STOP 0x4
#define SIGNAL_IR_FORWARD 0x8
#define SIGNAL_IR_BACKWARD 0x10

struct signals_s
{
  int irSignals;
  int sonarSignal;
} signal;


void motorForward(int motorSpeed, char motorDirection)
{
  digitalWrite(PIN_MOTOR_BREAK_A, LOW);
  digitalWrite(PIN_MOTOR_BREAK_B, LOW);

  if(motorDirection == HIGH)
  {
    digitalWrite(PIN_MOTOR_DIRECTION_A, HIGH);
    digitalWrite(PIN_MOTOR_DIRECTION_B, LOW);
  }
  else
  {
    digitalWrite(PIN_MOTOR_DIRECTION_A, LOW);
    digitalWrite(PIN_MOTOR_DIRECTION_B, HIGH);
  }

  analogWrite(PIN_MOTOR_SPEED_A, motorSpeed);
  analogWrite(PIN_MOTOR_SPEED_B, motorSpeed);
}

void motorStop()
{
  digitalWrite(PIN_MOTOR_BREAK_A, HIGH);
  digitalWrite(PIN_MOTOR_BREAK_B, HIGH);
}

void motorTurn(int motorDirection, int motorSpeed)
{
  if(motorDirection == HIGH)
  {
    digitalWrite(PIN_MOTOR_BREAK_A, HIGH);
    digitalWrite(PIN_MOTOR_BREAK_B, LOW);

    digitalWrite(PIN_MOTOR_DIRECTION_A, motorDirection);

    analogWrite(PIN_MOTOR_SPEED_A, motorSpeed);
  }
  else
  {
    digitalWrite(PIN_MOTOR_BREAK_A, LOW);
    digitalWrite(PIN_MOTOR_BREAK_B, HIGH);

    digitalWrite(PIN_MOTOR_DIRECTION_B, ~motorDirection);

    analogWrite(PIN_MOTOR_SPEED_B, motorSpeed);
  }
}

void motorMoveOnSignal(int signal)
{
  switch(signal)
  {
    case SIGNAL_IR_FORWARD:
      motorForward(MOTOR_SPEED, MOTOR_DIRECTION_FORWARD);
      break;
    case SIGNAL_IR_BACKWARD:
      motorForward(MOTOR_SPEED, MOTOR_DIRECTION_BACKWARD);
      break;
    case SIGNAL_IR_LEFT:
      motorTurn(MOTOR_DIRECTION_LEFT, MOTOR_SPEED);
      break;
    case SIGNAL_IR_RIGHT:
      motorTurn(MOTOR_DIRECTION_RIGHT, MOTOR_SPEED);
      break;
    case SIGNAL_IR_STOP:
      motorStop();
      break;
    case SIGNAL_IR_NO_SIGNAL:
    default:
      break;
  }      
};

void motorRecoveryMove(void)
{
  motorStop();
  motorForward(MOTOR_SPEED, MOTOR_DIRECTION_BACKWARD);
  delay(1000);
  motorTurn(MOTOR_DIRECTION_RIGHT, MOTOR_SPEED);
  delay(1000);
  motorStop();
  motorForward(MOTOR_SPEED, MOTOR_DIRECTION_FORWARD);
}

void servoMove(struct servoCtx_s *ctx)
{
  if (ctx->pos == 135)
  {
    ctx->servoStep = -SERVO_STEP;
    //minDistance = 500;
  }
  
  if (ctx->pos == 30)
  {
    ctx->servoStep = SERVO_STEP;
  }
  
  myservo.write(ctx->pos);              // tell servo to go to position in variable 'pos'
  
  ctx->pos = ctx->pos+ctx->servoStep; 
}

int sonarDistanceGet(void)
{
  unsigned int pingTime = sonar.ping_median(SONAR_READS_NO); // Send ping, get ping time in microseconds (uS).
  unsigned int distance = pingTime / US_ROUNDTRIP_CM;  
  delay(SONAR_DELAY_AFTER);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  return distance;
}

void lcdPrint(void)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current: ");
  lcd.print(sonarDistance );
}

void irDecodeResults(decode_results results)
{
  switch (results.value)  
  {
    case IR_PRZOD:
      signal.irSignals = SIGNAL_IR_FORWARD;
      break;
    case IR_TYL:
      signal.irSignals = SIGNAL_IR_BACKWARD;
      break;
    case IR_STOP:
      signal.irSignals = SIGNAL_IR_STOP;
      break;
    case IR_PRAWO:
      signal.irSignals = SIGNAL_IR_RIGHT;
      break;
    case IR_LEWO:
      signal.irSignals = SIGNAL_IR_LEFT;
      break;
  }  
}

void setup()
{
  // Ustawienie kierunku wyjść cyfrowych
  pinMode(PIN_MOTOR_DIRECTION_A, OUTPUT);
  pinMode(PIN_MOTOR_SPEED_A, OUTPUT);
  pinMode(PIN_MOTOR_BREAK_A, OUTPUT);
  
  pinMode(PIN_MOTOR_DIRECTION_B, OUTPUT);
  pinMode(PIN_MOTOR_SPEED_B, OUTPUT);
  pinMode(PIN_MOTOR_BREAK_B, OUTPUT);
  
  // Włączenie hamulca
  digitalWrite(PIN_MOTOR_BREAK_A, HIGH);
  digitalWrite(PIN_MOTOR_BREAK_B, HIGH);
  
  irrecv.enableIRIn();
  
  // Wybór rodzaju wyświetlacza  - 16x2
  lcd.begin(16, 2);
  
  // Servo setup
  servoCtx.pos = SERVO_INITIAL_POS;
  servoCtx.servoStep = SERVO_STEP;
  myservo.attach(PIN_SERVO);
  
  // Application setup
  state = STATE_IDLE;
  signal.irSignals = SIGNAL_IR_NO_SIGNAL;
  signal.sonarSignal = FALSE;
}

void loop()
{
  //    long voltage;
  // static unsigned minDistance = 500;
  
  servoMove(&servoCtx);
  sonarDistance = sonarDistanceGet();
  if (sonarDistance < SONAR_TRESHOLD_DISTANCE)
  {
    signal.sonarSignal = TRUE;
  }
 
  lcdPrint();


  if (irrecv.decode(&irRawResults)) {
    irDecodeResults(irRawResults);
    irrecv.resume();
  }  
    
  switch(state)
  {
    case STATE_IDLE:
      if (signal.irSignals != SIGNAL_IR_NO_SIGNAL)
      {
        motorMoveOnSignal(signal.irSignals);
        if (signal.irSignals != SIGNAL_IR_STOP)
        {
          state = STATE_MOVING;
        }
        signal.irSignals = SIGNAL_IR_NO_SIGNAL;          
      }
      break;
      
    case STATE_MOVING:
      if(signal.sonarSignal == TRUE)
      {
        motorRecoveryMove();
        signal.sonarSignal = FALSE;
        signal.irSignals = SIGNAL_IR_NO_SIGNAL;
      }      
      else if (signal.irSignals)
      {
        motorMoveOnSignal(signal.irSignals);
        if (signal.irSignals == SIGNAL_IR_STOP)
        {
          state = STATE_IDLE;
        }
        signal.irSignals = SIGNAL_IR_NO_SIGNAL;          
      }
      break; 
  }
}

