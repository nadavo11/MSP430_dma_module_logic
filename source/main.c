#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>

enum FSMstate state;
unsigned int KB;
enum SYSmode lpm_mode;
unsigned int i = 0;
char index[2];


void main(void){

  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Stop Timers, Init LCD

  while(1){

      switch(state){
	  case state0: //idle - Sleep
	      enterLPM(mode0);
	      break;
	  case state1: //PB0 recorder
	      uart_start();
	    break;

	  case state2: //PB1 Audio player
	      rowSwapDMA(1,2);
	      scroll();
	      break;
		
      case state3: ; // PB2/3 For final lab
          LED_shift();
          break;

      case state4: //
		break;
		
	}
  }
}

  
  
  
  
  
  
