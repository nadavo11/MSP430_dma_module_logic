#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

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

const char keypad[KEYPAD_ROWS * KEYPAD_COLS] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};

void uart_start(){

    lcd_reset();
    while(state == 1){
        //let the keypad trigger its ISR
        enable_interrupts();

        //wait for keystroke
        enterLPM(0);

        //when we wake up:
        lcd_data(keypad[KB]);
    }

}

/*__________________________________________________________
 *                                                          *
 *          STATE 2 : DMA Row Swap                          *
 *__________________________________________________________*/

#define M 10
#define N 32
char data_matrix_in[M][N] = {
    "1.An apple a day keeps the ",
    "2.climb on the bandwagon",
    "3.Dot the i's and cross the",
    "4.He who pays the piper ca",
    "5.The pen is mightier tha",
    "6.The pot callin",
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


void rowSwapDMA(int j, int i)
{

    StopAllTimers();
    DMACTL0 = DMA0TSEL_0;   // Set software DMA trigger source

    //TIMER_A0_config();
    //TIMERB_config();

    lcd_reset();

    DMA0CTL |= DMADT_2 | DMASRCINCR_3 | DMADSTINCR_3 | DMAEN; // Enable DMA0, block transfer, no address increment
    DMA0CTL |= DMASRCBYTE | DMADSTBYTE;
    DMA0SA = (void(*)()) &data_matrix_in;
    DMA0SZ = M*N;
    DMA0DA = (void(*)()) &data_matrix_out;
    DMA0CTL |= DMAREQ;

    lcd_puts(data_matrix_out[0]);
    enable_interrupts();


}

void scroll(){
    int s = 0;
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
        lcd_puts(data_matrix_out[s]);
    }

}



/*__________________________________________________________
 *                                                          *
 *          STATE 3 : DMA Led Shift                          *
 *__________________________________________________________*/



void LED_shift(){
#include "msp430xG46x.h"

const char shift_val[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };


  P9DIR |= 0x003;                           // P5.0/5.1 output
  DMACTL0 = DMA0TSEL_8;                       // DMA channel 0 transfer select 8:  Timer_B (TBCCR0.IFG)
  DMA0CTL |= DMASRCBYTE | DMADSTBYTE;
  DMA0SA = (int)shift_val;                  // Source block address
  DMA0DA = (int)&P9OUT;                     // Destination single address
  DMA0SZ = 0x8;                            // Block size
  DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMAEN; // Rpt, inc src
  DMA0CTL|= DMASBDB;                         // DMA transfer: source word to destination byte
  TBCTL = TBSSEL_1 + MC_2 + ID_0;           // Clock Source: ACLK , contmode
  TBCCR0 = 0x200;                          // count to 2**12
  enable_interrupts();
  enterLPM(lpm_mode);       // Enter LPM0

 StopAllTimers();

 DMACTL0 = 0;
 DMA0CTL = 0;



}



