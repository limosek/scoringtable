

/*
 * tabule

 5.0 spol. 4segment
5.1 spol. 3segment
5.2 spol. 2segment
5.3 spol. 1segment POČÍTÁNO Z LEVA

5.4 tlačítka vstup (5.0 + 5.4 - ubrání skóré černí)
                                        (5.1 + 5.4 - přidání skóré černí)
                                        (5.2 + 5.4 - ubrání skóré zelení)
                                        (5.3 + 5.4 - přidání skóré zelení)

6.7 výstup ser.linka pro velký display (sender)
6.7 vstup ser. linka (receiver)

6.0 - G
6.1 - C
6.2 - D
6.3 - E
6.4 - B
6.5 - F
6.6 - A 

Skóre do 9 by mělo být zobrazeno na segmentu 1 .  Nad 10 klasicky  (platí pro sóré černých)

 */

#ifdef p16f84
#define __16f84a
#include <pic16f84a.h>
#ifdef IS_SENDER
# define NUM_CHARS
# define ALPHA_CHARS
#endif
#endif

#ifdef p16f628
#define __16f628
#include <pic16f628a.h>
#define NUM_CHARS
#define ALPHA_CHARS
#define EXTRA_CHARS
#ifdef IS_SENDER
# define HAVE_TIME
# define HAVE_EXTRA_MENU
# define HAVE_MESSAGES
# define MAXMODES 2
# endif
#endif

#include "tsmtypes.h"
#include "tsmee.h"

void timer_1ms();
void tmr0();
void timer_100ms();
int schar;
int serial_receive(Uint8 wait);

#ifdef HAVE_SLEEP
#define SLEEP() __asm sleep __endasm
#else
#define SLEEP()
#endif

#define BREAK() __asm sleep __endasm
#define CLRWDT() __asm clrwdt __endasm
#define NOP() __asm nop __endasm

  /*
    This nice explanation and chart from 
    http://code.google.com/p/picrobotcode/source/browse/trunk/cylon_basic.c?r=4

    The TMR0 interupt will occur when TMR0 overflows from 0xFF to
    0x00.  Without a prescaler, TMR0 will increment every clock
    cycle resulting in an interrupt every 256 cycles.  However, 
    using a prescaler, we can force that interrupt to occure at
    less frequent intervals.
    
    Each clock cycle is 1/4 the external clock.  Using that, and
    knowing the prescaler, we can determine the time interval for
    our interrupt.  
    
    PS2 PS1 PS0 Ratio   Cycles  4MHz        10MHz
    0   0   0   1:2     512      512.0 uS    204.8 uS    
    0   0   1   1:4     1024     1.024 mS    409.6 uS
    0   1   0   1:8     2048     2.048 mS    819.2 uS
    0   1   1   1:16    4096     4.096 mS    1.638 mS
    1   0   0   1:32    8192     8.192 mS    3.276 mS
    1   0   1   1:64    16384   16.384 mS    6.553 mS
    1   1   0   1:128   32768   32.768 mS   13.107 mS
    1   1   1   1:256   65536   65.536 mS   26.214 mS 
    */

#define TMR0_PRESCALER 2
//#define TMR0_CORRECTION 0x100-195
#define TMR0_USECS 819LU
//#define HAVE_ROUTINE_TMR0_1MS
//#define ROUTINE_TMR0_1MS timer_1ms()

/*
 * Segments bits 
 */
#define SA 1
#define SB 2
#define SC 4
#define SD 8
#define SE 16
#define SF 32
#define SG 64

/*
 * Define segment ports and bits
 */
#define PIN_A RB6
#define PIN_B RB4
#define PIN_C RB1
#define PIN_D RB2
#define PIN_E RB3
#define PIN_F RB5
#define PIN_G RB0

// Unknown character segments
#define U_CHAR SF|SC

/*
 * Segment select
 */
#define PIN_S1 RA3
#define PIN_S2 RA2
#define PIN_S3 RA1
#define PIN_S4 RA0

void selectsegment( Uint8 s) {
    PIN_S1=0; PIN_S2=0; PIN_S3=0; PIN_S4=0;
    switch (s) {
      case 0: return;
      case 1: PIN_S1=1; PIN_S2=0; PIN_S3=0; PIN_S4=0;
	return;
      case 2: PIN_S1=0; PIN_S2=1; PIN_S3=0; PIN_S4=0;
	return;
      case 3: PIN_S1=0; PIN_S2=0; PIN_S3=1; PIN_S4=0;
	return;
      case 4: PIN_S1=0; PIN_S2=0; PIN_S3=0; PIN_S4=1;
	return;
    }
}

/*
 * Input switches
 */
#define PIN_BTN RA4

/*
 * Input switch combinations
 */
#define SCORE1_UP (PIN_BTN && PIN_S3)
#define S_SCORE1_UP selectsegment(3)

#define SCORE1_DOWN (PIN_BTN && PIN_S4)
#define S_SCORE1_DOWN selectsegment(4)

#define SCORE2_UP (PIN_BTN && PIN_S1)
#define S_SCORE2_UP selectsegment(1)

#define SCORE2_DOWN (PIN_BTN && PIN_S2)
#define S_SCORE2_DOWN selectsegment(2)

// Number of miliseconds to wait for press/unpress
#define BTN_DELAY 5

#define MAXSCORE 99

/*
 * Serial pins
 */

#define SERIAL_NPOLARITY
#define BAUDUS 104 //104 microseconds for 9600bps
#define SERIAL_TIMEOUT 100LU	// Timeout loops 

#ifdef IS_RECEIVER
#define HAVE_SERIAL_RECEIVE
#define PIN_RX RB7
#define SERIAL_RX_TRIS 1<<7
#endif

#ifdef IS_SENDER
#define HAVE_SERIAL_SEND
#define PIN_TX RB7
#define SERIAL_RX_TRIS 0
#endif


#ifdef p16f84
#define SETTRIS TRISA=(1<<4); TRISB=(0 | SERIAL_RX_TRIS);
#endif

#ifdef p16f628
#define SETTRIS TRISA=(1<<4); TRISB=( 0 | SERIAL_RX_TRIS); CMCON=0x7;
#endif

