#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
  
  
  state = state0;
  lpm_mode = mode0;
  sysConfig();
  lcd_init();
  wait_1_sec();
  lcd_clear();
  lcd_home();
  lcd_data(0x37);
  enable_interrupts();


  while(1){
	switch(state){
	  case state0: //idle
	      enterLPM(mode0);
	      break;
		 
	  case state1: //PB0 
	    break;

	  case state2: //PB1
	    lcd_clear();
        CountUp();
	    CountDown();

            lcd_clear();
            if (state == state2)
               state = state0;
            break;
                    
          case state3: ; //PB2
            lcd_clear();
            enable_interrupts();
            
            break;
                
	}
  }
}

  ////updated 9:55
  
  
  
  
  
