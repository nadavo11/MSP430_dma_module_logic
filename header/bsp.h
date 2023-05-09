#ifndef _bsp_H_
#define _bsp_H_

//include  <msp430G2553.h>          // MSP430x2xx
#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal      250

// realtime
#define meas_OUT            P2OUT
#define meas_DIR            P2DIR
#define meas_SEL            P2SEL


#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

//   Generator abstraction
#define GenPort            P2IN
#define GenPortSel         P2SEL
#define GenPortDir         P2DIR
#define GenPortOut         P2OUT

//   Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT

// PushButton 3 abstraction for Main Lab
#define PB3sArrPort         P2IN
#define PB3sArrIntPend      P2IFG
#define PB3sArrIntEn        P2IE
#define PB3sArrIntEdgeSel   P2IES
#define PB3sArrPortSel      P2SEL
#define PB3sArrPortDir      P2DIR
#define PB3sArrPortOut      P2OUT

// PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PBsArrPortOut      P1OUT
#define PB0                0x01   // P1.0
#define PB1                0x02  // P1.1
#define PB2                0x04  // P1.2
#define PB3                0x10  // P2.0


extern void GPIOconfig(void);
extern void TIMER0_A0_config(void);

#endif



