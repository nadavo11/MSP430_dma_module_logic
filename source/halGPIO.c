#include  "../header/halGPIO.h"     // private library - HAL layer

// Global Variables
unsigned int EndOfRecord = 0;
int Octava6Freqs[] = {note0, note1, note2, note3, note4, note5, note6, note7, note8, note9, note10, note11, note12};
int j=0;
unsigned int KBIFG = 0;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	StopAllTimers();
	lcd_init();
	lcd_clear();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
// 				Set Byte to Port
//--------------------------------------------------------------------
void SetByteToPort(char ch){
	PBsArrPortOut |= ch;  
} 
//--------------------------------------------------------------------
// 				Clear Port Byte
//--------------------------------------------------------------------
void clrPortByte(char ch){
	PBsArrPortOut &= ~ch;
} 
//---------------------------------------------------------------------
//            General Function - No need
//---------------------------------------------------------------------
void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}
//---------------------------------------------------------------------
//                     Polling delays
//---------------------------------------------------------------------
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
//            Polling based Delay function
//******************************************************************
void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}
//---------------**************************----------------------------
//               Interrupt Services Routines
//---------------**************************----------------------------

//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR  // For Push Buttons
  __interrupt void PBs_handler(void){
	delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	if(PBsArrIntPend & PB0){
	    state = state1;
	    PBsArrIntPend &= ~PB0;
	}
    else if(PBsArrIntPend & PB1){
        state = state2;
        lcd_clear();
        lcd_home();
	    PBsArrIntPend &= ~PB1;
    }
	// If PB2, Show LCD Menu according the scroll value
    else if(PBsArrIntPend & PB2){

	    PBsArrIntPend &= ~PB2;
        }
//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
        switch(lpm_mode){
		case mode0:
		 LPM0_EXIT; // must be called from ISR only
		 break;
		 
		case mode1:
		 LPM1_EXIT; // must be called from ISR only
		 break;
		 
		case mode2:
		 LPM2_EXIT; // must be called from ISR only
		 break;
                 
        case mode3:
		 LPM3_EXIT; // must be called from ISR only
		 break;
                 
        case mode4:
		 LPM4_EXIT; // must be called from ISR only
		 break;
	}
        
}


//*********************************************************************
//            Port2 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT2_VECTOR  // For KeyPad
  __interrupt void PBs_handler_P2(void){
      delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
      if(KeypadIRQIntPend & BIT1){    // if keypad has been pressed find value
          KB = 75;
          KeypadPortOUT = 0x0E;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  KB = 15;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  KB = 14;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  KB = 13;
          else if ( ( KeypadPortIN & 0x80 ) == 0 ) KB = 12;

          KeypadPortOUT = 0x0D;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  KB = 11;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  KB = 10;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  KB = 9;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  KB = 8;

          KeypadPortOUT = 0x0B;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  KB = 7;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  KB = 6;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  KB = 5;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  KB = 4;

          KeypadPortOUT = 0x07;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  KB = 3;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  KB = 2;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  KB = 1;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  KB = 0;
//----------------Statement1--------------------------------------

//-----------------Statement2--------------------------------------

//-----------------------------------------------------------------
          delay(15000);   // For keypad debounce
          KeypadPortOUT &= ~0x0F;  // Reset Row1-4
          KeypadIRQIntPend &= ~BIT1; // Reset Flag
      }


//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
      switch(lpm_mode){
      case mode0:
          LPM0_EXIT; // must be called from ISR only
          break;

      case mode1:
          LPM1_EXIT; // must be called from ISR only
          break;

      case mode2:
          LPM2_EXIT; // must be called from ISR only
          break;

      case mode3:
          LPM3_EXIT; // must be called from ISR only
          break;

      case mode4:
          LPM4_EXIT; // must be called from ISR only
          break;
      }
  }



//*********************************************************************
//            DMA ISR
//*********************************************************************
//#pragma vector = DMA_VECTOR
//__interrupt void DMA_ISR (void){
//    StopAllTimers();
//}
