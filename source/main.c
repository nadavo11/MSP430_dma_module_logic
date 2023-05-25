#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>
////UPDATE14;55
enum FSMstate state;
unsigned int KB;
enum SYSmode lpm_mode;
unsigned int i = 0;
char index[2];



void main(void){

   //realtime
  char my_s[100]  ="Wise machines are dramatically more valuable than machines that just store andretrieve information";
  char my_d[100];


  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Stop Timers, Init LCD
 // lcd_data(0x43);
  while(1){

      switch(state){
	  case state0: //idle - Sleep
	      enterLPM(mode0);
	      break;
	  case state1: //PB0 recorder
	      uart_start();
	    break;

	  case state2: //PB1 Audio player
	      rowSwapDMA(1,7);
	      scroll();
	      enterLPM(mode0);       // CPU off
	      break;
		
      case state3: ; // PB2/3 For final lab
          //this is the state3 task:
          //LED_shift();


          // this is the realtime task:
          s_swap(my_s,my_d);

          enterLPM(mode0);       // CPU off
          break;

      case state4:
          lcd_reset();
          lcd_data(0x34);
          __bis_SR_register(LPM0_bits + GIE);       // CPU off

		break;
		
	}
  }
  
}

  
  
  
  
  
  
