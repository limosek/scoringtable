
#define TIMER_H

#ifndef KHZ
#error "You must define KHZ variable!"
#endif

#ifndef TMR0_PRESCALER
#error "You must define TMR0_PRESCALER!"
#endif

#if (TMR0_PRESCALER==0)
#  define TMR0_RATIO 2
#endif
#if (TMR0_PRESCALER==1)
#  define TMR0_RATIO 4
#endif
#if (TMR0_PRESCALER==2)
#  define TMR0_RATIO 8
#endif
#if (TMR0_PRESCALER==3)
#  define TMR0_RATIO 16
#endif
#if (TMR0_PRESCALER==4)
#  define TMR0_RATIO 32
#endif
#if (TMR0_PRESCALER==5)
#  define TMR0_RATIO 64
#endif
#if (TMR0_PRESCALER==6)
#  define TMR0_RATIO 128
#endif
#if (TMR0_PRESCALER==7)
#  define TMR0_RATIO 256
#endif
#ifndef TMR0_RATIO
# error "TMR0_PRESCALER Not set!"
#endif

#define TMR0_CYCLES_FULL (256*TMR0_RATIO) 
#define TMR0_USECS_FULL ((TMR0_CYCLES_FULL)*(KHZ))/4000LU 
#ifndef TMR0_USECS
# define TMR0_USECS TMR0_CYCLES_FULL
#endif

void cycle_eater(void);
void tmr0_init();
void usleep8(Uint8 msec);
void usleep(Uint16 msec);
void tmr0_clock();
void nanosleep(Uint8 usec);

volatile Uint8 __at 0xcd loop_x;
volatile Uint8 __at 0xce loop_b;
volatile Uint8 __at 0xcf loop_c;

#ifdef HAVE_TIME
volatile Uint16 microseconds=0;
volatile Uint16 miliseconds=0;
#endif

// This is how long cycle_eater takes for values of X, B, and C.
#define LOOP_CYCLES(X, B, C)	((X*B*4)+(C*3)+13)

/**
 *	Use this macro for delays over 750 cycles and under
 *	190,000 cycles.
 */
#define CYCLES_BIG(X)	do {					\
		loop_b=((X)-16LU)/764LU;			\
		loop_c=((((X)-16LU)%764LU)/3LU)+1;		\
		cycle_eater();					\
	} while(0)

/**
 *	Use this macro for delays under 750 cycles and over 13 cycles.
 *	It uses an assembly trick to jump direct to a branch halfway
 *	through the cycle eater function.
 */
#define CYCLES_SMALL(X) do {					\
		loop_c=((X)-11LU)/3LU;				\
		__asm	BANKSEL	_loop_c	__endasm;		\
		__asm	CALL	correction	__endasm;	\
	} while(0)

/**
 *	Calculates cycles from microsecnds based on clock speed relative 
 *	to 4MHz.
 */
#define DELAY_SMALL_US(X)	CYCLES_SMALL( ((X)*(KHZ))/4000LU )
#define DELAY_BIG_US(X)		CYCLES_BIG( ((X)*(KHZ))/4000LU	)

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
#ifdef HAVE_TIME
#if (TMR0_USECS<10000)
void tmr0_1ms() {
   miliseconds++;
#ifdef HAVE_ROUTINE_TMR0_1MS
   ROUTINE_TMR0_1MS;
#endif
}
#endif
#endif //have_time

#if (TMR0_USECS>=10000)
void tmr0_10ms() {
   miliseconds+=10;
   if ((miliseconds % 100)==0) {
     tmr0_100ms();
   }   
}
#endif

#ifdef HAVE_ROUTINE_TMR0_100MS
void tmr0_100ms() {
  ROUTINE_TMR0_100MS;
}
#endif

static void tmr0_interrupt() __interrupt 0 {
  
  INTCON=0;
  T0IE=1;

  if (RBIF) {
	RBIF=0;
  }

#ifdef TMR0_CORRECTION
  TMR0=TMR0_CORRECTION;
#endif

#ifndef TMR0_CORRECTION
  TMR0=0;
#endif
  
#ifdef HAVE_ROUTINE_TMR0
  ROUTINE_TMR0;
#endif

#ifdef HAVE_TIME
  microseconds+=TMR0_USECS;
  
#if (TMR0_USECS>=10000)
  while (microseconds>=10000) {
    tmr0_10ms();
    microseconds-=10000;
  }
#endif
#if (TMR0_USECS<10000)
  while (microseconds>=1000) {
    tmr0_1ms();
    microseconds-=1000;
  }
#endif

#endif //have_time
}

/*
 * For longer sleeps where exact timing is not needed
 * Argument is number of miliseconds (8bit version)
 */
void usleep8(Uint8 msec) {
    Uint8 i;
    
    if (msec==0) {
      while (1) SLEEP();
    } else {
#ifdef HAVE_TIME
      i=(Uint8) miliseconds+msec;
      while ((Uint8) miliseconds != i) { }
#endif
#ifndef HAVE_TIME
      for (i=0;i<msec;i++) {
	DELAY_BIG_US(1000LU);
      }
#endif
    }
}

/*
 * For longer sleeps where exact timing is not needed
 * Argument is number of miliseconds
 */
void usleep(Uint16 msec) {
    while (msec>200) {
      usleep8(200);
      msec-=200;
    }
    usleep8(msec);
}

