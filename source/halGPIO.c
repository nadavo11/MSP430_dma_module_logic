#include  "../header/halGPIO.h"     // private library - HAL layer
////UPDATE14;55
// Global Variables
unsigned int EndOfRecord = 0;
int Octava6Freqs[] = {note0, note1, note2, note3, note4, note5, note6, note7, note8, note9, note10, note11, note12};
int j=0;
unsigned int KBIFG = 0;
char LED_STATE = 0x1;

/*__________________________________________________________
 *                                                          *
 *             System Configuration                       *
 *__________________________________________________________*/
void sysConfig(void){ 
	GPIOconfig();
	StopAllTimers();
	lcd_init();
	lcd_clear();
}
//____________________________________________________________
/*__________________________________________________________
 *                                                          *
 *          Set Byte to Port                       *
 *__________________________________________________________*/

void SetByteToPort(char ch){
	PBsArrPortOut |= ch;  
} 
/*__________________________________________________________
 *                                                          *
 *            Clear Port Byte                       *
 *__________________________________________________________*/

void clrPortByte(char ch){
	PBsArrPortOut &= ~ch;
} 
/*__________________________________________________________
 *                                                          *
 *            General Function - No need                       *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *            Enter from LPM0 mode                      *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *             Enable interrupts                       *
 *__________________________________________________________*/

void enable_interrupts(){
  _BIS_SR(GIE);
}
/*__________________________________________________________
 *                                                          *
 *             Disable interrupts                      *
 *__________________________________________________________*/

void disable_interrupts(){
  _BIC_SR(GIE);
}
/*__________________________________________________________
 *                                                          *
 *            LCD                       *
 *__________________________________________________________*/
/*__________________________________________________________
 *                                                          *
 *           send a command to the LCD                       *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *          send data to the LCD                      *
 *__________________________________________________________*/

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
/*__________________________________________________________
 *                                                          *
 *          write a string of chars to the LCD                     *
 *__________________________________________________________*/

void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}
/*__________________________________________________________
 *                                                          *
 *          initialize the LCD                     *
 *__________________________________________________________*/

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

/*__________________________________________________________
 *                                                          *
 *         lcd strobe functions                     *
 *__________________________________________________________*/
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}

/*__________________________________________________________
 *                                                          *
 *         Polling delays                     *
 *__________________________________________________________*/
/*__________________________________________________________
 *                                                          *
 *         Delay usec functions                     *
 *__________________________________________________________*/
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
/*__________________________________________________________
 *                                                          *
 *          Delay msec functions                      *
 *__________________________________________________________*/

void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}

/*__________________________________________________________
 *                                                          *
 *           Polling based Delay function                       *
 *__________________________________________________________*/  
void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}

/*__________________________________________________________
 *                                                          *
 *         DMA                       *
 *__________________________________________________________*/
void DMA0 (data_matrix_in,data_matrix_out){
    DMA0SA = (unsigned int )data_matrix_in;
    DMA0DA = (unsigned int )data_matrix_out;
    DMA0SZ =32;
}
/*__________________________________________________________
 *                                                          *
 *         DMA                       *
 *__________________________________________________________*/
    void DMA1 (data_matrix_in,data_matrix_out){
     DMA1SA = (unsigned int)data_matrix_in;
     DMA1DA = (unsigned int )data_matrix_out;
     DMA1SZ =32;    
    }
 /*__________________________________________________________
 *                                                          *
 *         Port1 Interrupt Service Routine                        *
 *__________________________________________________________*/

#pragma vector=PORT1_VECTOR  // For Push Buttons
  __interrupt void PBs_handler(void){
	delay(debounceVal);

/*__________________________________________________________
 *                                                          *
 *           selector of transition between states                       *
 *__________________________________________________________*/        
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
    else if(PBsArrIntPend & PB2){

        state = state3;
	    PBsArrIntPend &= ~PB2;
    }

    else if(PBsArrIntPend & PB3){

           state = state4;
           PBsArrIntPend &= ~PB3;
       }

/*__________________________________________________________
 *                                                          *
 *            Exit from a given LPM                       *
 *__________________________________________________________*/
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
/*__________________________________________________________
 *                                                          *
 *           Port2 Interrupt Service Routine                       *
 *__________________________________________________________*/
#pragma vector=PORT2_VECTOR  // For KeyPad
  __interrupt void PBs_handler_P2(void){
      delay(debounceVal);

/*__________________________________________________________
 *                                                          *
 *           selector of transition between states                        *
 *__________________________________________________________*/
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
//_________Statement1____________________________________

//__________Statement2____________________________________

//_________________________________________________________
          delay(15000);   // For keypad debounce
          KeypadPortOUT &= ~0x0F;  // Reset Row1-4
          KeypadIRQIntPend &= ~BIT1; // Reset Flag
      }
/*__________________________________________________________
 *                                                          *
 *            Exit from a given LPM                       *
 *__________________________________________________________*/
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
  #pragma vector = DMA_VECTOR
  __interrupt void DMA_ISR (void){
      LPM0_EXIT;
      _BIC_SR(DMAIV);
  }

