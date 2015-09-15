

#include "tabule.h"
#include "timer.h"
#include "serial.h"

// Set the __CONFIG word:
#define CONFIG_WORD _WDT_OFF&_PWRTE_OFF&_XT_OSC
Uint16 __at 0x2007  __CONFIG = CONFIG_WORD;

#ifdef NUM_CHARS
static const Uint8 num2s[]={
    SA|SB|SC|SD|SE|SF, 		//0
    SB|SC, 			//1
    SA|SB|SG|SE|SD,		//2
    SA|SB|SG|SC|SD, 		//3
    SF|SG|SC|SB, 		//4
    SA|SF|SG|SC|SD, 		//5
    SA|SF|SG|SE|SD|SC, 		//6
    SA|SB|SC, 			//7
    SA|SB|SC|SD|SE|SF|SG, 	//8
    SA|SB|SF|SG|SC 		//9
};
#endif

#ifdef ALPHA_CHARS
static const Uint8 char2s[]={
    SE|SF|SG|SA|SB|SC, 		//a
    SF|SE|SG|SC|SD, 		//b
    SG|SE|SD,			//c
    SA|SB|SG|SC|SD, 		//d
    SA|SF|SG|SE|SD, 		//E
    SA|SF|SG|SE, 		//f
    U_CHAR,	 		//g
    SG|SE|SC|SF, 		//h
    SC,			 	//i
    SB|SC|SD|SE,		//J
    U_CHAR,			//k
    SF|SE|SD,			//L
    U_CHAR,			//m
    SE|SG|SC,			//n
    SG|SE|SD|SC,		//o
    SE|SF|SA|SB|SG,		//p
    U_CHAR,			//q
    SA|SF|SG|SC|SD,		//S
    SA|SB|SC,			//T
    SE|SD|SC,			//u
    U_CHAR,			//v
    U_CHAR,			//w
    U_CHAR,			//x
    SF|SB|SG|SC,		//y
    U_CHAR			//z
};
#endif

#define A_s	SA|SF|SG|SC|SD
#define A_y	SF|SB|SG|SC
#define A_n	SE|SG|SC
#define A_c	SG|SE|SD
#define A_l	SF|SE|SD
#define A_i	SC
#define A_o	SG|SE|SD|SC

#ifdef HAVE_MESSAGES
static const Uint8 msgs[]={
     'E','n','d',' '
};
#endif

// Score 
Uint8 score1=MAXSCORE+1; // Boot score, will be reset to zero after message
Uint8 score2=0;

// Segments data
#ifdef IS_SENDER
char segs[4]={'l','i','n','o'};
#endif
#ifdef IS_RECEIVER
char segs[4]={0,0,A_n,A_c};
#endif

// Actual segment selected
volatile Uint8 actseg;

Uint8 setsegments(Uint8 s) {
  if (s & SA) PIN_A=1; else PIN_A=0;
  if (s & SB) PIN_B=1; else PIN_B=0;
  if (s & SC) PIN_C=1; else PIN_C=0;
  if (s & SD) PIN_D=1; else PIN_D=0;
  if (s & SE) PIN_E=1; else PIN_E=0;
  if (s & SF) PIN_F=1; else PIN_F=0;
  if (s & SG) PIN_G=1; else PIN_G=0;
  return(s);
}

Uint8 char2segment(char c) {
#ifdef NUM_CHARS
    if (c<=9) {
      return(setsegments(num2s[c]));
    }
    if (c>='0' && c<='9') {
      return(char2segment(c-'0'));
    }
#endif
    switch (c) {
      case ' ': return(setsegments(0));
      case '-': return(setsegments(SG));
#ifdef EXTRA_CHARS
      case '_': return(setsegments(SD));
      case '|': return(setsegments(SB|SC));
      case '^': return(setsegments(SA));
#endif
     }
#ifdef ALPHA_CHARS
      if (c>='a' && c<='z') {
	return(setsegments(char2s[c-'a']));
      }
      if (c>='A' && c<='Z') {
	return(setsegments(char2s[c-'A']));
      }
#endif
      return(U_CHAR);
 }

void writesegment() {
    Uint8 s=actseg/16+1;
    
    selectsegment(0);
    if (!(actseg % 16) && actseg<64) {      
#ifdef IS_SENDER
      Uint8 seg;
      seg=char2segment(segs[s-1]);
      selectsegment(s);
      if (score1<MAXSCORE) {
        if (actseg) {
	  serial_send(seg);
        } else {
	  serial_send(seg | 0x80);	// First segment - bit 7 set
        }
      }
      usleep8(3);
#endif
#ifdef IS_RECEIVER
      setsegments(segs[s-1]);
      selectsegment(s);
#endif
    }
}

#ifdef HAVE_EXTRA_MENU
volatile Uint8 mode=0;
#endif

// Time functions
#ifdef HAVE_TIME

volatile Uint16 ms=0;
volatile Uint8 seconds=0;
volatile Uint8 minutes=0;
volatile Uint8 hours=0;

void timer_1ms () {
    ms++;
    if (ms==1000) {
	ms=0;
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
#endif //HAVE_TIME

void leftscore2segments() {  
  if (score1<10) {
    segs[0]=score1;
    segs[1]=' ';
  } else {
    segs[0]=score1/10;
    segs[1]=score1 % 10; 
  }
}
void rightscore2segments() {  
  if (score2<10) {
    segs[3]=score2;
    segs[2]=' ';
  } else {
    segs[2]=score2/10;
    segs[3]=score2 % 10;
  }
}

#ifdef HAVE_TIME
void time2segments() {
   segs[0]='0';
   segs[1]='h';
   segs[2]=minutes/10;
   segs[3]=minutes % 10;
}
#endif

#ifdef HAVE_EXTRA_MENU
void extramenu() {
    mode++;
    if (mode>MAXMODES) mode=0;
    char2segment('n');
    while (SCORE1_DOWN || SCORE1_UP) { }
}
#endif

void buttondelay(Uint8 ms) {
  char2segment(' ');
  usleep(ms);
}

#ifdef IS_SENDER
void checkbuttons() {
  if (SCORE1_UP) {
    buttondelay(BTN_DELAY);
    S_SCORE1_DOWN;
    buttondelay(BTN_DELAY);
    if (SCORE1_DOWN) {
#ifdef HAVE_EXTRA_MENU
      extramenu();
#endif
    } else {
      S_SCORE1_UP;
      buttondelay(BTN_DELAY);
      while (SCORE1_UP) { }
    }
    if (score1<MAXSCORE) score1++;
  }
  if (SCORE1_DOWN) {
    buttondelay(BTN_DELAY);
    S_SCORE1_UP;
    buttondelay(BTN_DELAY);
    if (SCORE1_UP) {
#ifdef HAVE_EXTRA_MENU
      extramenu();
#endif
    } else {
      S_SCORE1_DOWN;
      buttondelay(BTN_DELAY);
      while (SCORE1_DOWN) { }
    }
    if (score1>0) score1--;
  }
  if (SCORE2_UP) {
    buttondelay(BTN_DELAY);
    while (SCORE2_UP) { }
    if (score2<MAXSCORE) score2++;
  }
  if (SCORE2_DOWN) {
    buttondelay(BTN_DELAY);
    while (SCORE2_DOWN) { }
    if (score2>0) score2--;
  }
}
#endif //IS_SENDER

#ifdef HAVE_MESSAGES
void message(Uint8 id) {
  id *= 4;
  segs[0]=msgs[id];
  segs[1]=msgs[id+1];
  segs[2]=msgs[id+2];
  segs[3]=msgs[id+3];
}
#endif

#ifdef IS_SENDER
void main () {
    Uint8 i;
    
    SETTRIS;    
    tmr0_init();
    serial_init();

    for (i=1;i<254;i++) {
      writesegment();
      actseg+=16;
      if (actseg>64) {
	actseg=0;
      }
    }
    score1=0;

    while (1) {

#ifdef HAVE_EXTRA_MENU
      rightscore2segments();
      leftscore2segments();
#endif
    
#ifdef HAVE_EXTRA_MENU
      switch (mode) {
	case 0:
	    rightscore2segments();
	    leftscore2segments();
	    break;
#ifdef HAVE_TIME
	case 1:
	    time2segments();
	    break;
	case 2:
	    time2segments();
	    break;
#endif //HAVE_TIME
	case 3:
	case 4:
	case 5:
#ifdef HAVE_MESSAGES
	    message(mode-2);
	    break;
#endif //HAVE_MESSAGES
      }
#endif //EXTRA_MENU

#ifndef HAVE_EXTRA_MENU
    rightscore2segments();
    leftscore2segments();
#endif
      writesegment();
      actseg+=16;
      if (actseg>64) {
	actseg=0;
      }
      checkbuttons();
    }
}
#endif // IS_SENDER

#ifdef IS_RECEIVER

char receive_char(Uint8 wait) {
  int c;
  
  while ((c=serial_receive(wait))<0) {
    writesegment();
    actseg+=16;
    if (actseg>64) {
      actseg=0;
    }
  }
  return((char) c);
}

void main () {
    Uint8 c;
    
    SETTRIS;    
    //tmr0_init();
    serial_init();
    
    while (1) {
      c=receive_char(10); // Try to receive char but do not wait for it
      if (c>=0x80) {
	segs[0]=c-0x80;
	segs[1]=receive_char(SERIAL_TIMEOUT); // Receive rest of chars and wait for them
	segs[2]=receive_char(SERIAL_TIMEOUT);
	segs[3]=receive_char(SERIAL_TIMEOUT);
      }
    }
}
#endif
