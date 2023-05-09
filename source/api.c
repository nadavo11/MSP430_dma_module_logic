#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"



// Global Variables

//unsigned int REdge1, REdge2;
#define ADC_NUMBER_CAPTURES 100
unsigned int adcCaptureValues[ADC_NUMBER_CAPTURES];
unsigned int adcCapturePointer;

//                         CountDown


//                             CountUp
void CountDown(){
      int i;

      char const * startWatch ="01:00";
      lcd_puts(startWatch);
      wait_1_sec();

      for (i = 59 ; i >= 0 ; i--){
        if (state != state2){
            break;
        }

        lcd_clear();
        lcd_home();

        char const * minstr ="00:";
        lcd_puts(minstr);
        lcd_print_num(i);

        wait_1_sec();
     }
}


void CountUp(){
      int i;
      for (i = 0 ; i < 60 ; i++){
        if (state != state2){
            break;
        }

        lcd_clear();
        lcd_home();

        char const * minstr ="00:";
        lcd_puts(minstr);
        lcd_print_num(i);

        wait_1_sec();
     }
}

//              StartTimer For Count Down

void startTimerA0(){
    TACCR0 = 0xFFFF;  // Timer Cycles - max
    TA0CTL = TASSEL_2 + MC_1 + ID_3;  //  select: 2 - SMCLK ;
                                      //control: 3 - Up/Down  ; 
                                      //divider: 3 - /8
    // no ACLCK, we use SMCLK.
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

