#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------

void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LCD configuration//////////////////////////////////
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7
  
  // Generator Setup
  //From the table at CCIx p2.4
  GenPortDir &=  ~BIT4;               // P2.4 Input Capture = '1'
  GenPortSel |=  BIT4;              // P2.4 Select = '1'

  // Buzzer Setup
  BuzzPortDir |= BIT2;             // P2.2 Output compare - '1'
  BuzzPortSel |= BIT2;             // P2.2 Select = '1'
  BuzzPortOut &= ~BIT2;             // P2.2 out = '0'

  // PushButtons Setup
  PBsArrPortSel &= ~0x07;           //
  PBsArrPortOut &= ~0x07;            // Set P1Out to '0'
  PBsArrPortDir &= ~0x07;            // P1.0-2 - Input ('0')
  PBsArrPortDir |= 0x08;             // P1.3 - Output ('1')
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode   P1.0-P1.1 - '1'
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode  P1.2 - '0'
  PBsArrIntEn |= 0x07;               // P1.0-2 - '1'
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts P1.0-P1.3 all P1
  
  // PushButton 3 Setup For Main Lab
   PB3sArrPortOut &= ~BIT0;            // Set P2Out to '0'


   //Keypad config
   P10DIR = 0xF0;     // Set P10.7..4 as input / P10.3..0 as output
   //P10IES = 0xFF;     // Enable pull-up/pull-down resistors on P10
   P10OUT = 0x00;     // Set pull-up mode for P10
   P2OUT &= ~ BIT1;     // Set pull-up mode for P2.1

   //keypad irq config
   P2SEL &= ~BIT1;
   P2DIR &= ~(BIT1);  // Set P2.1 as input
   //P2REN |= BIT1;     // Enable pull-up/pull-down resistor on P2.1
   P2IFG &= ~BIT1;
   P2IES &= ~BIT1;    // Interrupt on falling edge (key press)
   P2IE  |= BIT1;    // Enable interrupt on P2.1
   __enable_interrupt(); // Enable global interrupts

   //PIN 2.1 is the measurement pin
    meas_DIR            |= BIT0;
    meas_SEL            &= ~BIT0;

    meas_OUT            &= ~BIT0;
    P2IE                &= ~BIT0;

  _BIS_SR(GIE);                     // enable interrupts globally


}


//------------------------------------------------------------------------------------- 
//            Timer 1sec configuration
//-------------------------------------------------------------------------------------
void TIMER0_A0_config(void){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    TA0CCTL0 = CCIE;
    TACCR0 = 0xFFFF;
    TA0CTL = TASSEL_2 + MC_0 + ID_3;  //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
//    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
} 
