#include "Motor.h"
#include <NewPing.h>

#include <IRremote.h>
#include <Voltage.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include "PinSetup.h"

#define FEATURE_MANUAL_SONAR

#ifndef FEATURE_MANUAL_SONAR
#include <NewPing.h>
#endif


#define TRUE 1
#define FALSE 0

/*
static unsigned long counterB= 0;

void intteruptHandlerB(void)
{
  counterB++;
}
*/

#define DEBUG_PRINT_DISTANCES
#define FEATURE_ENABLE_IR
#define FEATURE_ENABLE_SERVO
#define FEATURE_MOTOR


// Servo definitions
#define SERVO_POINT_ZERO 90
#define SERVO_ANGLE_HIGH 90
#define SERVO_ANGLE_LOW 40
#define SERVO_POINT_RIGHT(ANGLE) (SERVO_POINT_ZERO - ANGLE)
#define SERVO_POINT_LEFT(ANGLE) (SERVO_POINT_ZERO + ANGLE) 
#define SERVO_STEP 30

#ifdef FEATURE_ENABLE_SERVO
Servo myservo;  // create servo object to control a servo
#endif

struct servoCtx_s 
{
  int pos;
  int servoStep;
  int angleRange;
} servoCtx;

// Sonar definitions
#define SONAR_MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define SONAR_DELAY_AFTER 80 // 30ms
#define SONAR_READS_NO_NORMAL 1 // No of reads in one call
#define SONAR_READS_NO_HIGH 3 
#define SONAR_TRESHOLD_DISTANCE_DETECTION 20 // 30 cm 
#define SONAR_TRESHOLD_DISTANCE_RELEASE 25 // 30 cm 

//#define SERVO_NUMBER_OF_POSITIONS_ONE_SIDE ((SERVO_POINT_ZERO - SERVO_ANGLE_LOW) / SERVO_STEP ) + 1)
#define SONAR_GET_ANGLE_FROM_SERVO(SERVO_POS) (90 - SERVO_POS)

#define SONAR_INDEX_270 0
#define SONAR_INDEX_315 1
#define SONAR_INDEX_0_MINUS 2
#define SONAR_INDEX_0_PLUS 3
#define SONAR_INDEX_45 4
#define SONAR_INDEX_90 5

#define SONAR_NO_OF_DISTANCES 6

struct sonarCtx_s
{
  int sonarMinDistance;
  int sonarMinDistanceDirectionIndex;
  int sonarDistances[SONAR_NO_OF_DISTANCES];
} sonarCtx;

#ifndef FEATURE_MANUAL_SONAR
NewPing sonar(PIN_SONAR_TRIGGER, PIN_SONAR_ECHO, SONAR_MAX_DISTANCE); // NewPing setup of pins and maximum distance.
#endif

// LCD definitions
//LiquidCrystal lcd(PIN_LCDRS, PIN_LCDE, PIN_LCDDB4, PIN_LCDDB5, PIN_LCDDB6, PIN_LCDDB7);

// IR definitions
#define IR_PRZOD 0x4B3659A6
#define IR_TYL 0x4B36D926
#define IR_STOP 0x4B367986
#define IR_PRAWO 0x4B36B946
#define IR_LEWO 0x4B3639C6

#ifdef FEATURE_ENABLE_IR
IRrecv irrecv(PIN_IR);
decode_results irRawResults;
#endif

// Motor Definitions
#define MOTOR_DIRECTION_FORWARD LOW
#define MOTOR_DIRECTION_BACKWARD HIGH

#define MOTOR_DIRECTION_LEFT HIGH
#define MOTOR_DIRECTION_RIGHT LOW

#define MOTOR_SPEED_FAST 70
#define MOTOR_SPEED_SLOW 55

struct morotCtx_s
{
  int direction;
} motorCtx;

// Application globals
enum state_s 
{
  STATE_IDLE,
  STATE_MOVING,
  STATE_IMPEDIMENT,
  STATE_IMPEDIMENT_TURN
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

#ifdef FEATURE_MOTOR
void motorRun(int motorSpeed, char motorDirection)
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
  Serial.println("Motor stop");
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
      motorRun(MOTOR_SPEED_FAST, MOTOR_DIRECTION_FORWARD);
      break;
    case SIGNAL_IR_BACKWARD:
      motorRun(MOTOR_SPEED_FAST, MOTOR_DIRECTION_BACKWARD);
      break;
    case SIGNAL_IR_LEFT:
      motorTurn(MOTOR_DIRECTION_LEFT, MOTOR_SPEED_FAST);
      break;
    case SIGNAL_IR_RIGHT:
      motorTurn(MOTOR_DIRECTION_RIGHT, MOTOR_SPEED_FAST);
      break;
    case SIGNAL_IR_STOP:
      motorStop();
      break;
    case SIGNAL_IR_NO_SIGNAL:
    default:
      break;
  }      
};
#endif

void servoMove(struct servoCtx_s *ctx)
{
  //Serial.println(ctx->pos);
#ifdef FEATURE_ENABLE_SERVO
  if (!myservo.attached())
  { 
    return;
  }  

  if (ctx->pos == SERVO_POINT_LEFT(ctx->angleRange))
  {
    ctx->servoStep = -SERVO_STEP;
  }
  
  if (ctx->pos == SERVO_POINT_RIGHT(ctx->angleRange))
  {
    ctx->servoStep = SERVO_STEP;
  }
    
  ctx->pos = ctx->pos+ctx->servoStep;
  ctx->pos = max(ctx->pos, SERVO_POINT_RIGHT(ctx->angleRange)); 
  ctx->pos = min(ctx->pos, SERVO_POINT_LEFT(ctx->angleRange));
  
  myservo.write(ctx->pos);
#endif
}

#ifdef FEATURE_MANUAL_SONAR
unsigned long manualDistance()
{
  // static unsigned minDistance = 500;
  digitalWrite(PIN_SONAR_TRIGGER, LOW);
  delay(2);
  digitalWrite(PIN_SONAR_TRIGGER, HIGH);
  delay(10);
  digitalWrite(PIN_SONAR_TRIGGER, LOW);
  
  return pulseIn(PIN_SONAR_ECHO, HIGH);
}
#endif

static int sonarDistanceGet(int readTimes)
{
  unsigned long pingTime;
  unsigned int distance;
  
#ifdef FEATURE_MANUAL_SONAR
  pingTime = manualDistance();
#else
  pingTime = sonar.ping_median(readTimes); // Send ping, get ping time in microseconds (uS).
#endif
  distance = pingTime / US_ROUNDTRIP_CM;

  if (distance == 0)
  {
    distance = SONAR_MAX_DISTANCE;
  }
  delay(SONAR_DELAY_AFTER);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  return distance;
}

static int sonarGetIndexFromAngle(float angle)
{
  int index;
  
  if (angle >= 0)
  {
    if (angle < (45.0/2))
    {
      index = SONAR_INDEX_0_PLUS;
    }
    else if ((angle <= 45) || (angle <= (90.0 - 45.0/2)))
    {
      index = SONAR_INDEX_45;
    }
    else
    {
      index = SONAR_INDEX_90;
    }
  }
  else // angle < 0
  {
    if (angle > (-45.0/2))
    {
      index = SONAR_INDEX_0_MINUS;
    }
    else if ((angle >= -45) || (angle >= (-90.0 + 45.0/2)))
    {
      index = SONAR_INDEX_315;
    }
    else
    {
      index = SONAR_INDEX_270;
    }
  }
  return index;
}

static void sonarPrintDistances(void)
{
#ifdef DEBUG_PRINT_DISTANCES
  Serial.print("270 = "); Serial.println(sonarCtx.sonarDistances[SONAR_INDEX_270]);
  Serial.print("315 = "); Serial.println(sonarCtx.sonarDistances[SONAR_INDEX_315]);
  Serial.print("0- = "); Serial.println(sonarCtx.sonarDistances[SONAR_INDEX_0_MINUS]);
  Serial.print("0+ = "); Serial.println(sonarCtx.sonarDistances[SONAR_INDEX_0_PLUS]);
  Serial.print("45 = "); Serial.println(sonarCtx.sonarDistances[SONAR_INDEX_45]);
  Serial.print("90 = "); Serial.println(sonarCtx.sonarDistances[SONAR_INDEX_90]);
  
  Serial.print("Min distance = "); Serial.println(sonarCtx.sonarMinDistance);
#endif
}

void sonarDistanceUpdate(int servoPos)
{
  int angle = SONAR_GET_ANGLE_FROM_SERVO(servoPos);
  int index;
  int distance;
  
  index = sonarGetIndexFromAngle(angle);
  
  distance = sonarDistanceGet(SONAR_READS_NO_NORMAL);
  
  if (distance < SONAR_TRESHOLD_DISTANCE_DETECTION)
  {
    distance = sonarDistanceGet(SONAR_READS_NO_HIGH);
  }
  
  sonarCtx.sonarDistances[index] = distance;
  
  // Update Min Distance
  sonarCtx.sonarMinDistance = SONAR_MAX_DISTANCE;
  
  for (int i = 0; i < SONAR_NO_OF_DISTANCES; i++)
  {
    if (sonarCtx.sonarDistances[i] < sonarCtx.sonarMinDistance)
    {
      sonarCtx.sonarMinDistance = sonarCtx.sonarDistances[i];
      sonarCtx.sonarMinDistanceDirectionIndex = i;
    }
  }
  
  sonarPrintDistances();
}

int getDirectionFromSonar(struct sonarCtx_s *ctx)
{
  if (ctx->sonarMinDistanceDirectionIndex <= SONAR_INDEX_0_MINUS)
  {
    return MOTOR_DIRECTION_LEFT;
  }
  
  return MOTOR_DIRECTION_RIGHT;
}

/*
void lcdPrint(void)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current: ");
  lcd.print(sonarCtx.sonarMinDistance );
}
*/

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
  void (*irHandler)(void);
  
  // Enable pull up resistors for unused analog inputs
  //pinMode(A0, INPUT_PULLUP);
  //pinMode(A1, INPUT_PULLUP);
  
  Serial.begin(9600);
   
#ifdef FEATURE_MOTOR
  // Motor setup
  pinMode(PIN_MOTOR_DIRECTION_A, OUTPUT);
  pinMode(PIN_MOTOR_SPEED_A, OUTPUT);
  pinMode(PIN_MOTOR_BREAK_A, OUTPUT);
  
  pinMode(PIN_MOTOR_DIRECTION_B, OUTPUT);
  pinMode(PIN_MOTOR_SPEED_B, OUTPUT);
  pinMode(PIN_MOTOR_BREAK_B, OUTPUT);
  
  digitalWrite(PIN_MOTOR_BREAK_A, HIGH);
  digitalWrite(PIN_MOTOR_BREAK_B, HIGH);
#endif

#ifdef FEATURE_ENABLE_IR
  // IR setup
  irHandler = irrecv.enableIRIn();
#endif
  
  // Wybór rodzaju wyświetlacza  - 16x2
  //lcd.begin(16, 2);
  
  // Servo setup
#ifdef FEATURE_ENABLE_SERVO
  myservo.attach(PIN_SERVO);
  myservo.attachFunctionTimer1B(irHandler, 50);
#endif
  servoCtx.pos = SERVO_POINT_ZERO;
  servoCtx.servoStep = SERVO_STEP;
  servoCtx.angleRange = SERVO_ANGLE_LOW;
  
  // Sonar setup
#ifdef FEATURE_MANUAL_SONAR
  pinMode(PIN_SONAR_TRIGGER, OUTPUT);
  pinMode(PIN_SONAR_ECHO, INPUT);
#endif
  for (int i = 0 ; i < SONAR_NO_OF_DISTANCES ; i++)
  {
    sonarCtx.sonarDistances[i] = SONAR_MAX_DISTANCE;
  }
  sonarCtx.sonarMinDistance = SONAR_MAX_DISTANCE;
  
  // Application setup
  state = STATE_IDLE;
  signal.irSignals = SIGNAL_IR_NO_SIGNAL;
  signal.sonarSignal = FALSE;
}

/*
void printVoltage()
{
  static long minVoltage = 10000;
  static long voltage;
  voltage = readVcc();
  if (voltage < minVoltage)
  {
    minVoltage = voltage;
  }
  Serial.print("Voltage "); Serial.print(voltage); Serial.print(" "); Serial.println(minVoltage);
}
*/

void loop()
{
  sonarDistanceUpdate(servoCtx.pos);
  delay(SONAR_DELAY_AFTER);
#ifdef FEATURE_ENABLE_IR
  if (irrecv.decode(&irRawResults)) {
    irDecodeResults(irRawResults);
    Serial.println(irRawResults.value, HEX);
    Serial.println(signal.irSignals);
    irrecv.resume();
  }
#endif
    
#ifdef FEATURE_ENABLE_SERVO
  servoMove(&servoCtx);  
#endif
  
  if (sonarCtx.sonarMinDistance < SONAR_TRESHOLD_DISTANCE_DETECTION)
  {
    signal.sonarSignal = TRUE;
  }
 
  //lcdPrint();
  if (signal.irSignals == SIGNAL_IR_STOP)
  {
    state = STATE_IDLE;
    #ifdef FEATURE_ENABLE_SERVO
    myservo.detach();
    #endif
  }
  
  switch(state)
  {
    case STATE_IDLE:
      if (signal.irSignals != SIGNAL_IR_NO_SIGNAL)
      {
#ifdef FEATURE_MOTOR
        motorMoveOnSignal(signal.irSignals);
#endif
        if (signal.irSignals != SIGNAL_IR_STOP)
        {
          state = STATE_MOVING;
#ifdef FEATURE_ENABLE_SERVO
          if (!myservo.attached())
          {
            myservo.attach(PIN_SERVO);
          }
#endif
        }
        signal.irSignals = SIGNAL_IR_NO_SIGNAL;
        signal.sonarSignal = FALSE;
      }
      break;
      
    case STATE_MOVING:
      if(signal.sonarSignal == TRUE)
      {
        motorStop();
        state = STATE_IMPEDIMENT;
        //signal.sonarSignal = FALSE;
        signal.irSignals = SIGNAL_IR_NO_SIGNAL;
      }      
      else if (signal.irSignals != SIGNAL_IR_NO_SIGNAL)
      {
#ifdef FEATURE_MOTOR
        motorMoveOnSignal(signal.irSignals);
#endif
        signal.irSignals = SIGNAL_IR_NO_SIGNAL;          
      }
      break;
      
    case STATE_IMPEDIMENT:
#ifdef FEATURE_MOTOR
       motorRun(MOTOR_SPEED_SLOW, MOTOR_DIRECTION_BACKWARD);
       delay(300);
       motorStop();
       motorTurn(getDirectionFromSonar(&sonarCtx), MOTOR_SPEED_SLOW);
#endif
   
       servoCtx.angleRange = SERVO_ANGLE_HIGH;
       state = STATE_IMPEDIMENT_TURN;
       break;
       
    case STATE_IMPEDIMENT_TURN:
      if (sonarCtx.sonarMinDistance > SONAR_TRESHOLD_DISTANCE_RELEASE)
      {
        state = STATE_MOVING;
        signal.sonarSignal = FALSE;
        servoCtx.angleRange = SERVO_ANGLE_LOW;
#ifdef FEATURE_MOTOR
        motorStop();
        motorRun(MOTOR_SPEED_FAST, MOTOR_DIRECTION_FORWARD);
#endif
      }
      break; 
  }
}
