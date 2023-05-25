#include  "../header/bsp.h"    // private library - BSP layer
////UPDATE14;55
/*______________________________________
 *                                      *
 *         GPIO configuration                    *
 *______________________________________*/
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7
  
  // Buzzer Setup
  BuzzPortDir |= BIT2;             // P2.2 Output compare - '1'
  BuzzPortSel |= BIT2;             // P2.2 Select = '1'
  BuzzPortOut &= ~BIT2;             // P2.2 out = '0'

  // Keypad Setup
  KeypadPortSel &= ~0xFF;
  KeypadPortDIR = 0x0F; //10.0-10.3 output, 10.4-10.7 input
  KeypadPortOUT = 0x00; // CLR output

  // Keypad IRQ Setup
  KeypadIRQPortSel &= ~BIT1;
  KeypadIRQPortDir &= ~BIT1;             // P2.1 input
  KeypadIRQIntEdgeSel |= BIT1;         // pull-up mode  P2.1 - '1'
  KeypadIRQIntEn |= BIT1;               // P2.1 - '1'
  KeypadIRQIntPend &= ~0xFF;            // clear pending interrupts P2.1

  // PushButtons Setup
  PBsArrPortSel &= ~0x0F;           //
  PBsArrPortOut &= ~0x0F;            // Set P1Out to '0'
  PBsArrPortDir &= ~0x0F;            // P1.0-2 - Input ('0')
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode   P1.0-P1.1 - '1'
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode  P1.2 - '0'
  PBsArrIntEn |= 0x0F;               // P1.0-2 - '1'
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts P1.0-P1.3 all P1
  

  //LED Setup
  LEDsArrPortDir |= 0xFF;
  LEDsArrPortSel  = 0;

//  // PushButton 3 Setup For Main Lab
//   PB3sArrPortSel &= ~BIT0;           //
//   PB3sArrPortOut &= ~BIT0;            // Set P2Out to '0'
//   PB3sArrPortDir &= ~BIT0;            // P2.0 - Input ('0')
//   PB3sArrIntEdgeSel &= ~BIT0;         // pull-down mode  P2.0 - '0'
//   PB3sArrIntEn |= BIT0;               // P1.0-2 - '1'
//   PB3sArrIntPend &= ~BIT0;            // clear pending interrupts P2.0

  _BIS_SR(GIE);                     // enable interrupts globally


}

/*______________________________________
 *                                      *
 *         Timer A 345msec configuration                     *
 *______________________________________*/
void TIMER_A0_config(void){
   // WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    TACCR0 = 0xFFFF; // (2^20/8)*345m = 45219 -> 0xB0A3
    TACCTL2 = 0;
    TACCR2 = 0;
    TACCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC_1 + ID_3;  //  select: 2 - SMCLK ; control: 1 - Up ; divider: 3 - /8
}

/*______________________________________
 *                                      *
 *         Timer B PWM configuration                     *
 *______________________________________*/
void TIMERB_config(void){
 //   FLL_CTL0 |= XCAP14PF;         // Configure load caps remove later
    TBCCTL1 = OUTMOD_4; // TACCR1 toggle
    TBCCR0 = 0xFFFF;
    TBCTL = TBSSEL_2 + MC_1 + ID_3;   //SMCLK, up-mode
}

/*______________________________________
 *                                      *
 *          DMA configuration                     *
 *______________________________________*/
void DMA_config(void){
    DMA0CTL = DMAEN + DMASRCINCR_3 + DMASWDW + DMAIE; //repeated-single, source inc, word-word trans, Interupt enable
    DMACTL0 = DMA0TSEL_1; //TACCR2_IFG trigger
}



/*______________________________________
 *                                      *
 *          Stop All Timers                     *
 *______________________________________*/


void StopAllTimers(void){

    TACTL = MC_0; // halt timer A
    TBCTL = MC_0; // halt timer B
    TBCCTL0 = 0x00; // stop PWM
    TBCCR0 = 0;
    DMA0CTL = 0; // Stop DMA0


}

void DMA_sw_trigger(){
    DMACTL0 = DMA0TSEL_0;   // Set software DMA trigger source
}
void DMA_config1(){
    DMA0CTL |= DMADT_2 | DMAIE | DMASRCINCR_3 | DMADSTINCR_3 ; // Enable DMA0, block transfer, no address increment
    DMA0CTL |= DMASRCBYTE | DMADSTBYTE;
}
void DMA_set_adresses(char src[],char dst[], int size){
    DMA0SA = (void(*)()) src;
    DMA0DA = (void(*)()) dst;
    DMA0SZ = size;
}
const char shift_val[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
void DMA_config3(){
    P9DIR |= 0x003;                           // P9.0/5.1 output
    DMACTL0 = DMA0TSEL_8;                       // DMA channel 0 transfer select 8:  Timer_B (TBCCR0.IFG)
    DMA0CTL |= DMASRCBYTE | DMADSTBYTE;
    DMA0SA = (int)shift_val;                  // Source block address
    DMA0DA = (int)&P9OUT;                     // Destination single address
    DMA0SZ = 0x8;                            // Block size
    DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMAEN; // Rpt, inc src
    DMA0CTL|= DMASBDB;                         // DMA transfer: source word to destination byte
    TBCTL = TBSSEL_1 + MC_2 + ID_0;           // Clock Source: ACLK , contmode
    TBCCR0 = 0x200;                    // count to 2**12
}


/*
void confDMA3(void)
{
  const char shift_val[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
  //lcd_clear();
  P9DIR |= 0x003;                           // P5.0/5.1 output
  DMACTL0 = DMA0TSEL_8;                       // DMA channel 0 transfer select 8:  Timer_B (TBCCR0.IFG)
  DMA0CTL |= DMASRCBYTE | DMADSTBYTE;
 // DMA0SA = (unsigned int)shift_val;                  // Source block address
  DMA0DA = (unsigned int)&P9OUT;                     // Destination single address
  DMA0SZ = 0x8;                            // Block size
  DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMAEN; // Rpt, inc src
  DMA0CTL|= DMASBDB;                         // DMA transfer: source word to destination byte
  TBCTL = TBSSEL_1 + MC_2 + ID_0;           // Clock Source: ACLK , contmode
  TBCCR0 = 0x200;    










}*/
/*void DMA_DATA(){

    DMA0CTL |= DMADT_2 | DMASRCINCR_3 | DMADSTINCR_3 | DMAEN; // Enable DMA0, block transfer, no address increment
    DMA0CTL |= DMASRCBYTE | DMADSTBYTE;
    DMA0SA = (unsigned int )&data_matrix_in;
    DMA0SZ = M*N;
    DMA0DA = (unsigned int )&data_matrix_out;
    DMA0CTL |= DMAREQ;


}*/


