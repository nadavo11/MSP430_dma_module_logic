#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Keypad matrix definition
const char keypad[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


//                        Wait 1 sec
void wait_1_sec(){
  startTimerA0();
  startTimerA0();
}




//System Configuration

void sysConfig(void){ 
    GPIOconfig();
    TIMER0_A0_config();

}


//              Set Byte to Port

void SetByteToPort(char ch){
    PBsArrPortOut |= ch;
} 

//              Clear Port Byte

void clrPortByte(char ch){
    PBsArrPortOut &= ~ch;
} 

//            Polling based Delay function

void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}


//            Enter from LPM0 mode

void enterLPM(unsigned char LPM_level){
    if (LPM_level == 0x00)
      _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
      _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
      _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
    else if(LPM_level == 0x03)
      _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
      _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}

//            Enable interrupts

void enable_interrupts(){
  _BIS_SR(GIE);
}

//            Disable interrupts

void disable_interrupts(){
  _BIC_SR(GIE);
}

//            LCD


//             send a command to the LCD

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

//                       send data to the LCD
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

//                write a string of chars to the LCD

void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}

//    write frequency template to LCD

void write_freq_tmp_LCD(){
   lcd_clear();
   lcd_home();
    const char SquareWaveFreq[] = "fin=";
    const char Hz[] = "Hz";
     lcd_puts(SquareWaveFreq);
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_puts(Hz);
}
//    write signal shape template to LCD

void write_signal_shape_tmp_LCD(){
   lcd_clear();
   lcd_home();
    const char signal_shape[] = "signal shape: ";
     lcd_puts(signal_shape);
     lcd_new_line;
}

// initialize the LCD

/******************************************************************
 * Displays a two-digit integer on the LCD screen.
 * If the integer is less than 10, a leading zero is displayed.
 *
 * @param num The integer to display (should be between 0 and 99).
 */
void lcd_print_num(int num) {
  // Extract the tens and ones digits
  int tens = num / 10 + 0x30;
  int ones = num % 10 + 0x30;

  // Display the tens digit

  lcd_data((char)tens);


  // Display the ones digit
  lcd_data((char)ones);
}




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

// lcd strobe functions

void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}




// The " DelayUs" function  Delay usec functions
//parameter and outputs the corresponding number on an LCD display.
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}

// Delay msec functions

void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}


/******************************************************************************
 *
 *                          get ADC
 *
 *____________________________________________________________________________
* @brief Reads the value from the ADC10 module and returns it.
*
* @return The converted value from the ADC10 module.
*
* @note This function enables the ADC10 module, starts a conversion, waits for the
*       conversion to complete, and then disables the ADC10 module. The converted
*       value is read from the ADC10MEM register and returned.
******************************************************************************/

/******************************************************************************
 *
 *                          TimerA0 ISR
 *
 *____________________________________________________________________________

******************************************************************************/

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

    LPM0_EXIT;
    TACTL = MC_0+TACLR;
}

/******************************************************************************
 *
 *                          ADC ISR
 *
 *____________________________________________________________________________

******************************************************************************/
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}


/******************************************************************************
 *
 *                          Buttons ISR
 *
 *____________________________________________________________________________

******************************************************************************/
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
   
    delay(debounceVal);

//            selector of transition between states

    if(PBsArrIntPend & PB0){
      state = state1;
      PBsArrIntPend &= ~PB0;
        }
        else if(PBsArrIntPend & PB1){
      state = state2;
      PBsArrIntPend &= ~PB1;
        }
    else if(PBsArrIntPend & PB2){
      state = state3;
      PBsArrIntPend &= ~PB2;
        }

//            Exit from a given LPM 

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

  /******************************************************************************
   *
   *                          Keypad ISR
   *
   *____________________________________________________________________________

   *      Port2 Interrupt Service Routine
  ******************************************************************************/



  // Function to handle keypad interrupt
  #pragma vector=PORT2_VECTOR
  __interrupt void Port_2(void) {
      // Delay for debounce
      __delay_cycles(1000);

      // Read the keypad value
      int row, col;
      char key = 0;
      for (row = 0; row < KEYPAD_ROWS; row++) {
          P10OUT = 0xFF;     // Set all rows high
          P10OUT &= ~(0x10 << row); // Set current row as output low
          __delay_cycles(100);  // Delay for settling
          col = P10IN & 0x0F; // Read column values
          if (col != 0x0F) {
              // Key pressed
              key = keypad[row][col];
              break;

          }
      }

      // Process the key value
      if (key != 0) {
          // Do something with the key value
          // Example: print the key value
          lcd_data(key);
      }

      // Clear the interrupt flag
      P2IFG &= ~BIT1;
      P10OUT = 0;
  }

