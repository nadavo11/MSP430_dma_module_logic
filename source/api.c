#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include "msp430xG46x.h"
////UPDATE14;55
#include "stdio.h"


/*______________________________________
 *                                      *
 *          LCD API                     *
 *______________________________________*/


void lcd_reset(){
    lcd_clear();
    lcd_home();
}

/*__________________________________________________________
 *                                                          *
 *          STATE 1 : "UART" interface                      *
 *__________________________________________________________*/

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
int counter=0;
const char keypad[KEYPAD_ROWS * KEYPAD_COLS] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};

void uart_start(){
    counter=0;
    lcd_reset();
    while(state == 1){
        //let the keypad trigger its ISR
        enable_interrupts();
        
        //wait for keystroke
        enterLPM(0);
        lcd_data(keypad[KB]);
        counter++;
        if(counter==16)
           lcd_cmd(0xC0);
     
        else if(counter==32)
        {
          lcd_reset();
          
          counter=0;
          
        }
        //when we wake up:
        
    }

}

/*__________________________________________________________
 *                                                          *
 *          STATE 2 : DMA Row Swap                          *
 *__________________________________________________________*/

#define M 10
#define N 32
char data_matrix_in[M][N] = {
    "1.An apple a day keeps the docto",
    "2.climb on the bandwagon",
    "3.Dot the i's and cross the t's",
    "4.He who pays the piper calls th",
    "5.The pen is mightier than the",
    "6.The pot calling the kettle bla",
    "7.shed crocodile tears",
    "8.Close but no cigar",
    "9.Cut from the same cloth",
    "10.Strike while the iron's hot"
};
char data_matrix_out[M][N];

    // DMA configuration
    #define DMA_CHANNEL_JI 0 // DMA channel for row j to row i transfer
    #define DMA_CHANNEL_IJ 1 // DMA channel for row i to row j transfer
    #define ROW_LENGTH 32

//////////////////////////////////////////////////////////////////////////////////////////////

void rowSwapDMA(int j, int i)
{   
    enable_interrupts();

    int line=0,dst=0,num=0;
    lcd_reset();
    StopAllTimers();

    DMA_sw_trigger();

    DMA_config1();
    for(line = 0; line < M; line++){
        dst = line;
        if(line == i)
            dst = j;
        if(line == j)
            dst = i;
        DMA_set_adresses(data_matrix_in[line],data_matrix_out[dst], N );
        DMA0CTL |= DMAREQ | DMAEN;
        enterLPM(lpm_mode);
    }



}

void scroll(){


    int s = 0;
    lcd_puts(data_matrix_out[0]);
    while(state == 2){
        //let the keypad trigger its ISR
        enable_interrupts();

        //wait for keystroke
        enterLPM(0);

        //when we wake up:
        if(KB == 15) s ++;
        if(KB == 3) s--;
        KB = 0;
        s = s % 10;

        lcd_reset();
     //   print_mat(s);
        lcd_puts(data_matrix_out[s]);
    }

}


/*__________________________________________________________
 *                                                          *
 *          STATE 3 : DMA Led Shift                          *
 *__________________________________________________________*/



void LED_shift(){
//#include "msp430xG46x.h"

  const char shift_val[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
  //lcd_clear();
 // confDMA3();
   
  lcd_clear();////////////
  DMA_config3();
  enable_interrupts();
  enterLPM(lpm_mode);       // Enter LPM0

  StopAllTimers();

 DMACTL0 = 0;
 DMA0CTL = 0;



}


/*__________________________________________________________
 *                                                          *
 *          STATE 3 : str Swap                              *
 *__________________________________________________________*/

void scroll2(char dst[]){


    int s = 0;

    while(state == 3){
        //let the keypad trigger its ISR
        enable_interrupts();

        //wait for keystroke
        enterLPM(0);

        //when we wake up:
        if(KB == 15) s ++;
        if(KB == 3) s--;
        KB = 0;

        lcd_reset();
     //   print_mat(s);
        lcd_puts(&dst[s]);
    }

}

int stringLength(const char *str) {
    int length = 0;

    while (*str != '\0') {
        length++;
        str++;
    }

    return length;
}

//swap the first and second halves of a given string, using DMA
void s_swap(char str[],char dst[]){

    //let DMA wake us up at the end of the Xfer
    enable_interrupts();

    //find the length and the median
    int len = stringLength(str);
    int med = (int)(len/2);

    //config DMA- sw trigger, Block transfer (MEM 2 MEM)
    DMA_config1();
    DMA_sw_trigger();

    //move my_s[0:m] -> my_d[m:len]
    DMA_set_adresses(str, &dst[len - med], med);
    DMA0CTL |= DMAREQ | DMAEN;

    //move my_s[m:] -> my_d[0:m]
    DMA_set_adresses(&str[med], dst, med);
    DMA0CTL |= DMAREQ | DMAEN;
    //enterLPM(lpm_mode);

    lcd_reset();
    lcd_puts(dst);

    //stop DMA
    DMACTL0 = 0;
    DMA0CTL = 0;

    scroll2(dst);


}



