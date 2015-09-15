# 1 "tabule.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "tabule.c"
# 29 "tabule.c"
# 1 "tabule.h" 1



# 1 "/usr/local/sdcc/share/sdcc/non-free/include/pic14/pic16f84a.h" 1 3 4
# 99 "/usr/local/sdcc/share/sdcc/non-free/include/pic14/pic16f84a.h" 3 4
extern __sfr __at (0x0000) INDF;
extern __sfr __at (0x0001) TMR0;
extern __sfr __at (0x0002) PCL;
extern __sfr __at (0x0003) STATUS;
extern __sfr __at (0x0004) FSR;
extern __sfr __at (0x0005) PORTA;
extern __sfr __at (0x0006) PORTB;
extern __sfr __at (0x0008) EEDATA;
extern __sfr __at (0x0009) EEADR;
extern __sfr __at (0x000A) PCLATH;
extern __sfr __at (0x000B) INTCON;

extern __sfr __at (0x0081) OPTION_REG;
extern __sfr __at (0x0085) TRISA;
extern __sfr __at (0x0086) TRISB;
extern __sfr __at (0x0088) EECON1;
extern __sfr __at (0x0089) EECON2;
# 158 "/usr/local/sdcc/share/sdcc/non-free/include/pic14/pic16f84a.h" 3 4
typedef union {
  struct {
    unsigned char RD:1;
    unsigned char WR:1;
    unsigned char WREN:1;
    unsigned char WRERR:1;
    unsigned char EEIF:1;
    unsigned char :1;
    unsigned char :1;
    unsigned char :1;
  };
} __EECON1bits_t;
extern volatile __EECON1bits_t __at(0x0088) EECON1bits;


typedef union {
  struct {
    unsigned char RBIF:1;
    unsigned char INTF:1;
    unsigned char T0IF:1;
    unsigned char RBIE:1;
    unsigned char INTE:1;
    unsigned char T0IE:1;
    unsigned char EEIE:1;
    unsigned char GIE:1;
  };
} __INTCONbits_t;
extern volatile __INTCONbits_t __at(0x000B) INTCONbits;


typedef union {
  struct {
    unsigned char PS0:1;
    unsigned char PS1:1;
    unsigned char PS2:1;
    unsigned char PSA:1;
    unsigned char T0SE:1;
    unsigned char T0CS:1;
    unsigned char INTEDG:1;
    unsigned char NOT_RBPU:1;
  };
} __OPTION_REGbits_t;
extern volatile __OPTION_REGbits_t __at(0x0081) OPTION_REGbits;


typedef union {
  struct {
    unsigned char RA0:1;
    unsigned char RA1:1;
    unsigned char RA2:1;
    unsigned char RA3:1;
    unsigned char RA4:1;
    unsigned char :1;
    unsigned char :1;
    unsigned char :1;
  };
} __PORTAbits_t;
extern volatile __PORTAbits_t __at(0x0005) PORTAbits;


typedef union {
  struct {
    unsigned char RB0:1;
    unsigned char RB1:1;
    unsigned char RB2:1;
    unsigned char RB3:1;
    unsigned char RB4:1;
    unsigned char RB5:1;
    unsigned char RB6:1;
    unsigned char RB7:1;
  };
} __PORTBbits_t;
extern volatile __PORTBbits_t __at(0x0006) PORTBbits;


typedef union {
  struct {
    unsigned char C:1;
    unsigned char DC:1;
    unsigned char Z:1;
    unsigned char NOT_PD:1;
    unsigned char NOT_TO:1;
    unsigned char RP0:1;
    unsigned char RP1:1;
    unsigned char IRP:1;
  };
} __STATUSbits_t;
extern volatile __STATUSbits_t __at(0x0003) STATUSbits;


typedef union {
  struct {
    unsigned char TRISA0:1;
    unsigned char TRISA1:1;
    unsigned char TRISA2:1;
    unsigned char TRISA3:1;
    unsigned char TRISA4:1;
    unsigned char :1;
    unsigned char :1;
    unsigned char :1;
  };
} __TRISAbits_t;
extern volatile __TRISAbits_t __at(0x0085) TRISAbits;


typedef union {
  struct {
    unsigned char TRISB0:1;
    unsigned char TRISB1:1;
    unsigned char TRISB2:1;
    unsigned char TRISB3:1;
    unsigned char TRISB4:1;
    unsigned char TRISB5:1;
    unsigned char TRISB6:1;
    unsigned char TRISB7:1;
  };
} __TRISBbits_t;
extern volatile __TRISBbits_t __at(0x0086) TRISBbits;
# 5 "tabule.h" 2
# 1 "tsmtypes.h" 1



typedef unsigned char Uint8;
typedef unsigned int Uint16;
typedef unsigned long Uint32;

typedef char Sint8;
typedef int Sint16;
typedef long Sint32;
# 6 "tabule.h" 2
# 30 "tabule.c" 2
# 1 "timer.h" 1







void cycle_eater(void);
void tmr0_init();
void usleep8(Uint8 msec);
void usleep(Uint16 msec);

extern volatile Uint8 __at 0xcd loop_x;
extern volatile Uint8 __at 0xce loop_b;
extern volatile Uint8 __at 0xcf loop_c;
extern volatile Uint16 microseconds=0;
extern volatile Uint16 miliseconds=0;
# 31 "tabule.c" 2



Uint16 __at 0x2007 __CONFIG = 0x3FFB&0x3FF7;

static const Uint8 num2segment[]={
    1|2|4|8|16|32,
    2|4,
    1|2|64|16|8,
    1|2|64|4|8,
    32|64|4,
    1|32|64|4|8,
    1|32|64|16|8|4,
    1|2|4,
    1|2|4|8|16|32|64,
    1|2|32|64|4
};


Uint8 score1;
Uint8 score2;


Uint8 seconds=0;
Uint8 minutes=0;
Uint8 hours=0;


char seg1='_';
char seg2='-';
char seg3='|';
char seg4='/';


Uint8 seg1s=0x40;
Uint8 seg2s=0x80;
Uint8 seg3s=0xc0;
Uint8 seg4s=0xf0;


volatile Uint8 actual=1;

void setsegments(Uint8 s) {
  if (s & 1) PORTBbits.RB6=1; else PORTBbits.RB6=0;
  if (s & 2) PORTBbits.RB4=1; else PORTBbits.RB4=0;
  if (s & 4) PORTBbits.RB1=1; else PORTBbits.RB1=0;
  if (s & 8) PORTBbits.RB2=1; else PORTBbits.RB2=0;
  if (s & 16) PORTBbits.RB3=1; else PORTBbits.RB3=0;
  if (s & 32) PORTBbits.RB5=1; else PORTBbits.RB5=0;
  if (s & 64) PORTBbits.RB0=1; else PORTBbits.RB0=0;
}

int char2segment(char c) {
    if (c<=9) {
      setsegments(num2segment[c]);
      return(1);
    } else {
      switch (c) {
      case ' ': setsegments(0); return(1);
      case '_': setsegments(8); return(1);
      case '-': setsegments(64); return(1);
      case '|': setsegments(2|4); return(1);
      case '\\': setsegments(32|64|4); return(1);
      case 'n': setsegments(16|64|4); return(1);
      case 'r': setsegments(16|64); return(1);
      case 'p': setsegments(16|32|1|2|64); return(1);
      case 'o': setsegments(64|16|8|4); return(1);
      case 'L': setsegments(32|16|8); return(1);
      case '/': setsegments(2|64|16); return(1);
      default: return(32|4);
      }
    }
}

void selectsegment( Uint8 s) {
    PORTAbits.RA3=0; PORTAbits.RA2=0; PORTAbits.RA1=0; PORTAbits.RA0=0;
    switch (s) {
      case 0: return;
      case 1: PORTAbits.RA3=1; PORTAbits.RA2=0; PORTAbits.RA1=0; PORTAbits.RA0=0;
 return;
      case 2: PORTAbits.RA3=0; PORTAbits.RA2=1; PORTAbits.RA1=0; PORTAbits.RA0=0;
 return;
      case 3: PORTAbits.RA3=0; PORTAbits.RA2=0; PORTAbits.RA1=1; PORTAbits.RA0=0;
 return;
      case 4: PORTAbits.RA3=0; PORTAbits.RA2=0; PORTAbits.RA1=0; PORTAbits.RA0=1;
 return;
    }
}

void writesegment() {
    selectsegment(0);
    if (actual<seg1s) {
      char2segment(seg1);
      selectsegment(1);
      return;
    }
    if (actual<seg2s) {
      char2segment(seg2);
      selectsegment(2);
      return;
    }
    if (actual<seg3s) {
      char2segment(seg3);
      selectsegment(3);
      return;
    }
    if (actual<seg4s) {
      char2segment(seg4);
      selectsegment(4);
      return;
    }
}

static void tmr0(void) {
    writesegment();
    actual+=10;
}

void timer_1ms () {
    if (miliseconds>999) {
 miliseconds=0;
 seconds++;
    }
}

void timer_1s () {
    seconds++;
    if (seconds>59) {
 seconds=0;
 minutes++;
    }
}

void timer_1m () {
    minutes++;
    if (minutes>59) {
 minutes=0;
 hours++;
    }
}

void timer_1h () {
    hours++;
}

void test() {
     seg1='r';
}

void main () {


    TRISB=0x00;
    TRISA=0x0;

    tmr0_init();

    while (1) {
      usleep8(1);
      seg1='r';
      usleep8(1);
      __asm sleep __endasm;
      seg2='r';
      usleep(1);
      seg3='r';
      usleep(1);
      seg4='r';
    }
}
# 1 "timer.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "timer.c"



volatile Uint8 __at 0xcd loop_x;
volatile Uint8 __at 0xce loop_b;
volatile Uint8 __at 0xcf loop_c;
volatile Uint16 microseconds=0;
volatile Uint16 miliseconds=0;




void cycle_eater(void)
{
 __asm banksel _loop_x
  nop
delay_big: movlw 191
  movwf _loop_x
delay_inner: nop
  decfsz _loop_x, 1
  goto delay_inner
  decfsz _loop_b,1
  goto delay_big
correction: decfsz _loop_c, 1
  goto correction
 __endasm;
}





void tmr0_init() {
    T0CS = 0;
    PSA = 0;

    PS2 = (TMR0_PRESCALER & 4)>>2;
    PS1 = (TMR0_PRESCALER & 2)>>1;
    PS0 = (TMR0_PRESCALER & 1);

    INTCON = 0;
    GIE = 1;
    INTE = 1;
    T0IE = 1;
    TMR0 = 0;
}

void tmr0_1ms() {
  miliseconds++;



}

static void tmr0_interrupt() __interrupt 0 {
  if (T0IF) {



    T0IF = 0;
    microseconds+=TMR0_USECS;
    while (microseconds>1000 && microseconds<64000 ) {
      microseconds-=1000;
      tmr0_1ms();
    }
  }
  CLRWDT();
}





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
