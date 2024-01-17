
/*
  EL Wire MCU control code

  Operation: This code is designed to control each EL wire star in project 23242 using ATtiny85 microcontroller. One I/O pin is dedicated to turn each start ON/OFF
  depending on the lighting pattern used. It is possible to modify this code to introduce countless types of EL Wire flashing patterns with different blink sequences, ON/OFF times
  and repetition sequences.
  
  by circuitapps
  January 2024
*/
#include <avr/io.h>

// PORT B DEFINITIONS
#define PB5_PIN1 PB5
#define PB4_PIN3 PB4
#define PB3_PIN2 PB3
#define PB2_PIN7 PB2
#define PB1_PIN6 PB1
#define PB0_PIN5 PB0

// Application pin definitions
#define PINK_STAR_OUT_PIN PB2_PIN7
#define YELLOW_STAR_OUT_PIN PB1_PIN6
#define BLUE_STAR_OUT_PIN PB0_PIN5

void turn_on(unsigned int pin_name)
{
  digitalWrite(pin_name, HIGH);
}

void turn_off(unsigned int pin_name)
{
  digitalWrite(pin_name, LOW);
}

void wait_ms(unsigned int delay_ms)
{
  delay(delay_ms);
}

// EL WIRE ILLUMINATION PATTERN 1:
void (*pattern_funcs[])(unsigned int) = {&turn_on,          &wait_ms, &turn_off,          &turn_on,             &wait_ms, &turn_off,            &turn_on,           &wait_ms, &turn_off};
unsigned int func_args[] =              {PINK_STAR_OUT_PIN, 200,      PINK_STAR_OUT_PIN,  YELLOW_STAR_OUT_PIN,  200,      YELLOW_STAR_OUT_PIN,  BLUE_STAR_OUT_PIN,  200,      BLUE_STAR_OUT_PIN};

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(PINK_STAR_OUT_PIN, OUTPUT);
  pinMode(YELLOW_STAR_OUT_PIN, OUTPUT);
  pinMode(BLUE_STAR_OUT_PIN, OUTPUT);

  digitalWrite(PINK_STAR_OUT_PIN, LOW);
  digitalWrite(YELLOW_STAR_OUT_PIN, LOW);
  digitalWrite(BLUE_STAR_OUT_PIN, LOW);
}

// the loop function runs over and over again forever

void loop() {

  int num_pattern_elements = sizeof(pattern_funcs) / sizeof(pattern_funcs[0]);

  for(int i = 0 ; i < num_pattern_elements; ++i)
  {// Cycling through each pattern defined by the user.

    pattern_funcs[i](func_args[i]);  // calling the function via function pointers and the specified function argument.

  }


}
