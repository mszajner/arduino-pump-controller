/**
 * PUMP CONTROLLER FOR THE WELLS
 * 
 * http://github.com/mszajner/arduino-pump-controller
 * 
 * @author Miroslaw Szajner <mszajner@rexoft.pl>
 */

#include <EEPROM.h>
#include <MsTimer2.h>

// pinout
#define PIN_PUMP1_RELAY       8
#define PIN_PUMP1_LED         6
#define PIN_PUMP2_RELAY       9
#define PIN_PUMP2_LED         7
#define PIN_BUTTON            2
#define PIN_LED_DAY           5
#define PIN_LED_WEEK          4
#define PIN_LED_MONTH         3

// button states
#define BTN_PUSHED            1
#define BTN_RELEASED          0

// interval values
#define INTERVAL_DAY          86400
#define INTERVAL_WEEK         (INTERVAL_DAY*7)
#define INTERVAL_MONTH        (INTERVAL_WEEK*4)

// addresses in eeprom
#define EEPROM_PUMP           0x00
#define EEPROM_INTERVAL       0x10

// button variables
byte buttonCurrentState = 0;
byte buttonPreviousState = 0;
byte buttonNextIntervalRun = 0;
unsigned int buttonCounter = 0;

// seconds counter
unsigned long secondsCounter = 0;

// selected interval
unsigned long interval = INTERVAL_DAY;

// selected pump
byte pump = 1;

// state of interval led
byte intervalLedState = 1;

// eeprom read/write buffer
byte eepromBuffer = 0;

byte counter = 0;

void count()
{
  counter++;
  
  // count button counter
  buttonCounter++;
  
  if ((counter%10) == 0) {
    
    // count seconds counter
    secondsCounter++;
    
    intervalLedState = (intervalLedState == LOW) ? HIGH : LOW;
    counter = 0;
  }
}

// initialization
void setup() {
  
  pinMode(PIN_PUMP1_RELAY, OUTPUT);
  pinMode(PIN_PUMP1_LED, OUTPUT);
  pinMode(PIN_PUMP2_RELAY, OUTPUT);
  pinMode(PIN_PUMP2_LED, OUTPUT);
  
  pinMode(PIN_LED_DAY, OUTPUT);
  pinMode(PIN_LED_WEEK, OUTPUT);
  pinMode(PIN_LED_MONTH, OUTPUT);
  
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  
  // read eeprom settings
  pump = EEPROM.read(EEPROM_PUMP);
  
  interval = 0;
  eepromBuffer = EEPROM.read(EEPROM_INTERVAL);
  interval |= eepromBuffer;
  interval <<= 8;
  eepromBuffer = EEPROM.read(EEPROM_INTERVAL+1);
  interval |= eepromBuffer;
  interval <<= 8;
  eepromBuffer = EEPROM.read(EEPROM_INTERVAL+2);
  interval |= eepromBuffer;
  interval <<= 8;
  eepromBuffer = EEPROM.read(EEPROM_INTERVAL+3);
  interval |= eepromBuffer;
  
  // filter eeprom settings (for first read from eeprom)
  interval = this_interval();
  pump = this_pump();
  
  // turn all leds on
  digitalWrite(PIN_PUMP1_LED, HIGH);
  digitalWrite(PIN_PUMP2_LED, HIGH);
  digitalWrite(PIN_LED_DAY, HIGH);
  digitalWrite(PIN_LED_WEEK, HIGH);
  digitalWrite(PIN_LED_MONTH, HIGH);
  
  delay(2000);
  
  MsTimer2::set(100, count);
  MsTimer2::start();
}

unsigned long this_interval()
{
  if ((interval == INTERVAL_DAY) || (interval == INTERVAL_WEEK) || (interval = INTERVAL_MONTH)) {
    return interval;
  }
  return INTERVAL_WEEK; // default interval
}

void next_interval()
{
  if (interval == INTERVAL_DAY) {
    interval = INTERVAL_WEEK;
  } else if (interval == INTERVAL_WEEK) {
    interval = INTERVAL_MONTH;
  } else {
    interval = INTERVAL_DAY;
  }
  eepromBuffer = interval >> 24;
  EEPROM.write(EEPROM_INTERVAL, eepromBuffer);
  eepromBuffer = interval >> 16;
  EEPROM.write(EEPROM_INTERVAL+1, eepromBuffer);
  eepromBuffer = interval >> 8;
  EEPROM.write(EEPROM_INTERVAL+2, eepromBuffer);
  eepromBuffer = interval;
  EEPROM.write(EEPROM_INTERVAL+3, eepromBuffer);
}

byte this_pump()
{
  if ((pump == 0) || (pump == 1) || (pump == 2)) {
    return pump;
  }
  return 1; // default pump 1 turned on
}

void next_pump()
{
  if (pump > 0) {
    pump = pump == 1 ? 2 : 1;
    EEPROM.write(EEPROM_PUMP, pump);
  }
}

void next_pump_button()
{
  pump = (++pump) % 3;
  EEPROM.write(EEPROM_PUMP, pump);
}

void turn_on_pump()
{
  digitalWrite(PIN_PUMP1_RELAY, (pump == 1) ? LOW : HIGH);
  digitalWrite(PIN_PUMP1_LED, (pump == 1) ? HIGH : LOW);
  digitalWrite(PIN_PUMP2_RELAY, (pump == 2) ? LOW : HIGH);
  digitalWrite(PIN_PUMP2_LED, (pump == 2) ? HIGH LOW: LOW);
}

void blink_interval_led()
{
  digitalWrite(PIN_LED_DAY, (interval == INTERVAL_DAY) ? intervalLedState : LOW);
  digitalWrite(PIN_LED_WEEK, (interval == INTERVAL_WEEK) ? intervalLedState : LOW);
  digitalWrite(PIN_LED_MONTH, (interval == INTERVAL_MONTH) ? intervalLedState : LOW);
}

// glowna petla
void loop() {
  
  // get button state
  if (digitalRead(PIN_BUTTON) == LOW) {
    buttonCurrentState = BTN_PUSHED;
  } else {
    buttonCurrentState = BTN_RELEASED;
  }
  
  if ((buttonCurrentState == BTN_PUSHED) && (buttonPreviousState == BTN_PUSHED)) {
    if (buttonCounter > 20) {
      if (!buttonNextIntervalRun) {
        buttonNextIntervalRun = 1;
        next_interval();
      }
    }
  }
  
  // detect button release
  if ((buttonCurrentState == BTN_RELEASED) && (buttonPreviousState == BTN_PUSHED)) {
    if ((buttonCounter > 1) && (buttonCounter <= 20)) {
      next_pump_button();
    }
  }
  
  // clear button counter
  if (buttonPreviousState != buttonCurrentState) {
    buttonCounter = 0;
    buttonNextIntervalRun = 0;
  }
  
  // store current button state
  buttonPreviousState = buttonCurrentState;
  
  // if seconds counter reach interval, then toggle pumps
  if (secondsCounter > interval) {
    next_pump();
    secondsCounter = 0;
  }
  
  turn_on_pump();
  blink_interval_led();
  
  delay(50);
}

