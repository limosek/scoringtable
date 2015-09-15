

void serial_bitwait() {
  DELAY_SMALL_US(BAUDUS);
}
#define SERIAL_BITWAIT serial_bitwait()

void serial_halfbitwait() {
  DELAY_SMALL_US(BAUDUS/3);
}
#define SERIAL_BITWAIT serial_bitwait()
#define SERIAL_HALFBITWAIT serial_halfbitwait()

#ifdef SERIAL_PPOLARITY
#ifdef HAVE_SERIAL_SEND
void serial_send(char c) {
      GIE=0;
      PIN_TX=0; SERIAL_BITWAIT;  //Start bit
      if (c & 0x1) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x2) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x4) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x8) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x10) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x20) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x40) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (c & 0x80) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      PIN_TX=1; SERIAL_BITWAIT;
      GIE=1;
}
#endif
#ifdef HAVE_SERIAL_RECEIVE
#define TEST_STARTBIT (!PIN_RX)
int schar=-1;
int serial_receive(Uint8 wait) {
      Uint8 c=0,j=0;
      int i=0;
      
      GIE=0;
      while (PIN_RX && i<wait) { 
	i++;
	j=0;
	while (PIN_RX && j<254) {
	  j++;
	}
      }	//	Wait for start bit
      if (PIN_RX) {
	schar=-2
	return(schar); //timeout
      }
      
      SERIAL_BITWAIT; SERIAL_HALFBITWAIT;	//center of bit
      if (PIN_RX) c|=0x1;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x2;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x4;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x8;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x10;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x20;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x40;
      SERIAL_BITWAIT;
      if (PIN_RX) c|=0x80;
      SERIAL_BITWAIT;
      if (!PIN_RX) {
	schar=-1;
	return(schar); // Stop bit not found
      }
      GIE=1;
      schar=c;
      return(c);
}
#endif
void serial_init() {
#ifdef HAVE_SERIAL_SEND
  PIN_TX=1;
#endif
#ifdef HAVE_SERIAL_RECEIVE
  RBIE=0;
  PIN_RX=1;
  //RBPU=1;
#endif
}

#endif
      
#ifdef SERIAL_NPOLARITY
#ifdef HAVE_SERIAL_SEND
void serial_send(char c) {
      GIE=0;
      PIN_TX=1; SERIAL_BITWAIT;  //Start bit
      if (!(c & 0x1)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x2)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x4)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x8)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x10)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x20)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x40)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      if (!(c & 0x80)) PIN_TX=1; else PIN_TX=0;  SERIAL_BITWAIT; 
      PIN_TX=0; SERIAL_BITWAIT;
      GIE=1;
}
#endif
#ifdef HAVE_SERIAL_RECEIVE
#define TEST_STARTBIT (PIN_RX)
int schar=-1;
int serial_receive(Uint8 wait) {
      Uint8 c=0,j=0;
      int i=0;
      
      GIE=0;
      while (!PIN_RX && i<wait) {
	i++;
	j=0;
	while (!PIN_RX && j<254) {
	  j++;
	}
      }
      if (!PIN_RX) {
	schar=-2;
	return(schar); //timeout
      }
      SERIAL_BITWAIT; SERIAL_HALFBITWAIT;	//center of bit
      if (!PIN_RX) c|=0x1;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x2;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x4;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x8;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x10;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x20;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x40;
      SERIAL_BITWAIT;
      if (!PIN_RX) c|=0x80;
      SERIAL_BITWAIT;
      if (PIN_RX) {
	schar=-1;
	return(schar); // Stop bit not found
      }
      GIE=1;
      schar=c;
      return(c);
}
#endif
void serial_init() {
#ifdef HAVE_SERIAL_SEND
  PIN_TX=0;
#endif
#ifdef HAVE_SERIAL_RECEIVE
  RBIE=0;
  PIN_RX=1;
  //RBPU=1;
#endif

}
#endif

      
