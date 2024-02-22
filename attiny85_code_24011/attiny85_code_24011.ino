
/*
  EL Wire MCU control code

  Operation: This code is designed to control each EL wire star in project 23242 using ATtiny85 microcontroller. One I/O pin is dedicated to turn each start ON/OFF
  depending on the lighting pattern used. It is possible to modify this code to introduce countless types of EL Wire flashing patterns with different blink sequences, ON/OFF times
  and repetition sequences.
  
  by circuitapps
  February 2024
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
// Circuit diagram pin definitions
#define ELW_1_EN PB2_PIN7
#define ELW_2_EN PB4_PIN3
#define ELW_3_EN PB3_PIN2
#define FADER_PWM_OUT PB1_PIN6

// Application pin mapping
#define PINK_STAR_OUT_PIN ELW_1_EN
#define YELLOW_STAR_OUT_PIN ELW_2_EN
#define BLUE_STAR_OUT_PIN ELW_3_EN

#define PWM_OUT_PIN FADER_PWM_OUT
#define PWM_REGISTER OCR1A  // This is where the high duration (out of a max of 255) needs to be written

#define FADE_OUT 0
#define FADE_IN 1
#define LED_EFFECT_WAIT_TIME 175  // Every LED_EFFECT_WAIT_TIME ms a new LED light intensity will be observed

void timer_counter_1_pwm_setup()
{
  TCCR1 = 1<<PWM1A | 1<<COM1A0 | 1<<CS10;
  // OC1A is the output to pin 6 of Attiny85 (as per datasheet)
  pinMode(PWM_OUT_PIN, OUTPUT);  // pwm is programmed to be output here
}

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


// the setup function runs once when you press reset or power the board
void setup() {

  timer_counter_1_pwm_setup();  // Sets PWM_OUT_PIN as PWM output

  pinMode(PINK_STAR_OUT_PIN, OUTPUT);
  pinMode(YELLOW_STAR_OUT_PIN, OUTPUT);
  pinMode(BLUE_STAR_OUT_PIN, OUTPUT);

  turn_off(PINK_STAR_OUT_PIN);
  turn_off(YELLOW_STAR_OUT_PIN);
  turn_off(BLUE_STAR_OUT_PIN);
}

// the loop function runs over and over again forever

void loop() {

  unsigned int count_state = FADE_IN;  // start with fade in effect
  //  Any maximum pwm_levels[] element can be 255. This determines the LED intensity level.
  unsigned int pwm_levels[] = {0,1,2,3,4,5,15,100,150,200,255};  // carefully selected through experimentation for best visual fading effect
  unsigned char current_level_idx = 0;

  unsigned int max_len = sizeof(pwm_levels) / sizeof(pwm_levels[0]);

  unsigned int el_wires_array[] = {PINK_STAR_OUT_PIN, YELLOW_STAR_OUT_PIN, BLUE_STAR_OUT_PIN};
  unsigned int current_elw_idx;
  unsigned int number_of_wires_to_activate;
  unsigned int elw_idx_arr[3];  // There are three EL wires we can activate simultaneously during any cycle
  bool new_idx_found;
  bool cycle_ongoing;

  while(1)
  {
    number_of_wires_to_activate = (unsigned int)random(1, 4);  // varies between 1 and 3 (inclusive) Number of EL wires to activate simultaneously in this cycle

    elw_idx_arr[0] = 255; elw_idx_arr[1] = 255; elw_idx_arr[2] = 255;  // buffer reset

    for(int i = 0; i < number_of_wires_to_activate; ++i)
    {
      new_idx_found = false;
      
      while(!new_idx_found)
      {
        current_elw_idx = (unsigned int)random(0, 3);  // select a target EL wire to illuminate randomly
        new_idx_found = true;  // assume a new EL wire index is found

        for(int each_buffer_pos = 0 ; each_buffer_pos < number_of_wires_to_activate; ++each_buffer_pos)
        {
          if(elw_idx_arr[each_buffer_pos] == current_elw_idx)
          {
            new_idx_found = false;  // not a new EL wire index!
            break;  // the for loop and generate a new random number
          }
        }

        if(new_idx_found)
        {
          // found a new wire that was not activated previously!
          elw_idx_arr[i] = current_elw_idx;  // update array with the new valid index found
          turn_on(el_wires_array[current_elw_idx]);  // activate the circuit for the selected EL wire
          delay(50);  // wait until wire turns on
        }

      }// end while

    }// enf for number_of_wires_to_activate

    // All target EL wires are now ON! Move to fading them in and out next.
    cycle_ongoing = true;

    while(cycle_ongoing)
    {
      PWM_REGISTER = pwm_levels[current_level_idx];
      delay(LED_EFFECT_WAIT_TIME);  // Wait time in ms before current LED light pattern is switched.

      if(count_state == FADE_IN)
      {// we are in FADE_IN phase
        if(current_level_idx == (max_len - 1))
        {// reached the FADE_IN limit
          count_state = FADE_OUT;  // time to switch to FADE_OUT state
          --current_level_idx;  // down one register value to kick off fade out next.
        }
        else
        {// FADE_IN limit not yet reached
          ++current_level_idx;  // up one register value for next fade in period.
        }
      }
      else
      {// we are in the FADE_OUT phase
        if(current_level_idx == 0)
        {// reached the FADE_OUT lower limit
          count_state = FADE_IN;  // time to switch to FADE_IN state
          ++current_level_idx;  // up one register value to kick off fade in next.
          cycle_ongoing = false;  // One fade in fade out cycle finished
        }
        else
        {// FADE_OUT lower limit not yet reached
          --current_level_idx;  // down one register value for next fade out period.
        }
      }

    }// end while cycle_ongoing

    for(int i = 0; i < number_of_wires_to_activate; ++i)
    {
      turn_off(el_wires_array[current_elw_idx]);  // DE-activate the circuit for the selected EL wires after one cycle of fading effect has been applied.
      wait_ms(50);  // give time to EL wire to turn off
    }

  }// end while


}
