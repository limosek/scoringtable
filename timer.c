
//#include "tabule.h"

volatile Uint8 __at 0xcd loop_x;
volatile Uint8 __at 0xce loop_b;
volatile Uint8 __at 0xcf loop_c;
volatile Uint16 microseconds=0;
volatile Uint16 miliseconds=0;

/*
 * Internal sleep routine. Used primarily for fine tuning usec sleeps
 */
void cycle_eater(void)
{
	__asm	banksel	_loop_x	// mumblegrumble
		nop
delay_big:	movlw	191		// Calibrated for b*764 cycles
		movwf	_loop_x	// Load W into reg
delay_inner:	nop	// To make the inner loop take 4 cycles per
		decfsz	_loop_x, 1
		goto	delay_inner
		decfsz	_loop_b,1
		goto	delay_big
correction:	decfsz	_loop_c, 1
		goto	correction
	__endasm;
}

/*
 * Initialize timer0
 * usecs - number of useconds between ticks
 */
void tmr0_init() {
    T0CS = 0;	// Clear to enable timer mode.
    PSA = 0;	// Clear to assign prescaler to Timer 0.
   
    PS2 = (TMR0_PRESCALER & 4)>>2;
    PS1 = (TMR0_PRESCALER & 2)>>1;
    PS0 = (TMR0_PRESCALER & 1);
    
    INTCON = 0;	// Clear interrupt flag bits.
    GIE = 1;	// Enable all interrupts.
    INTE = 1;
    T0IE = 1;	// Set Timer 0 to 0.
    TMR0 = 0;	// Enable peripheral interrupts
}

void tmr0_1ms() {
  miliseconds++;
#ifdef HAVE_ROUTINE_1MS
  ROUTINE_1MS();
#endif
}

static void tmr0_interrupt() __interrupt 0 {
  if (T0IF) {
#ifdef HAVE_ROUTINE_TMR0
  ROUTINE_TMR0();
#endif
    T0IF = 0;
    microseconds+=TMR0_USECS;
    while  (microseconds>1000 && microseconds<64000 ) {
      microseconds-=1000;
      tmr0_1ms();
    }
  }
  CLRWDT();
}

/*
 * For longer sleeps where exact timing is not needed
 * Argument is number of miliseconds (8bit version)
 */
void usleep8(Uint8 msec) {
    Uint8 oldms;
    
    if (msec==0) {
      while (1) SLEEP();
    }
    while (msec>0) {
      oldms=miliseconds;
      while (oldms==miliseconds) {
	SLEEP();
      }
      msec--;
    }
}

/*
 * For longer sleeps where exact timing is not needed
 * Argument is number of miliseconds
 */
void usleep(Uint16 msec) {    
    if (msec==0) {
      while (1) SLEEP();
    }
    while (msec>200) {
      usleep8(200);
      msec-=200;
    }
    usleep8(msec);
}
