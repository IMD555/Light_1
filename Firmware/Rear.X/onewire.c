#include <xc.h>
#include <onewire.h>

#define _str(x)  #x
#define str(x)  _str(x)

#define PIN_PORT	C
#define PIN_N		0
#define	P(R,P,r,p)	R##P##bits.r##P##p
#define	Pin(x)		x##PIN_PORT##PIN_N

// #define	PIN_WRITE	P(LAT,C,LAT,0)
#define	PIN_WRITE	Pin(LAT)
#define	PIN_READ	P(PORT,C,R,0)
#define	PIN_TRIS	P(TRIS,C,TRIS,0)
#define	PIN_OD		P(ODCON,C,ODC,0)
#define	PIN_ANSEL	P(ANSEL,C,ANS,0)
#define	PIN_WPU		P(WPU,C,WPU,0)
#define	PIN_INLVL	P(INLVL,C,INLVL,0)
#define BCF()       asm("BCF LATC,0,0")
#define	BSF()       asm("BSF LATC,0,0")
#define	BTFSC()     asm("BTFSC PORTC,0,0")
#define	BTFSS()     asm("BTFSS PORTC,0,0")

void ow_init(void) {
	Pin(LAT) = 0;
    PIN_WRITE = 1;
    PIN_TRIS = 0;
    PIN_OD = 1;
	ODCA0 = 1;
//#if PIN_ANSEL<>""
//   PIN_ANSEL = 0;
//#endif
    PIN_WPU = 1;
    PIN_INLVL = 1;
}

unsigned char ow_reset(void) {
    unsigned char r = 0;
    PIN_WRITE = 0;
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
    PIN_WRITE = 1;
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();
    r = !PIN_READ;
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
	NOP();	NOP();	NOP();	NOP();	NOP();NOP();	NOP();	NOP();	NOP();	NOP();
    return r;
}

#define OW_BIT_OUT(x)   BCF();\
asm("BTFSC WREG,"x",0");BSF();\
asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");\
BSF();asm("NOP");asm("NOP");
void ow_write_word(unsigned short data) {
	WREG = data;
	OW_BIT_OUT("0");    OW_BIT_OUT("1");    OW_BIT_OUT("2");    OW_BIT_OUT("3");
    OW_BIT_OUT("4");    OW_BIT_OUT("5");    OW_BIT_OUT("6");    OW_BIT_OUT("7");

//	WREG = (unsigned char)(data >> 8);
	WREG = *(((unsigned char*)&data)+1);
	OW_BIT_OUT("0");    OW_BIT_OUT("1");    OW_BIT_OUT("2");    OW_BIT_OUT("3");
    OW_BIT_OUT("4");    OW_BIT_OUT("5");    OW_BIT_OUT("6");    OW_BIT_OUT("7");
}

#define OW_BIT_IN(x) BCF();NOP();BSF();\
NOP();\
BTFSC();asm("BSF WREG,"x"&7,0");\
NOP();NOP();NOP();NOP();NOP();NOP();NOP();\
asm("Loop"x":");BTFSS();asm("GOTO Loop"x);

//#define OW_BIT_IN(x) BCF();NOP();BSF();\
//BTFSC();asm("BSF WREG,"x"&7,0");\
//NOP();NOP();NOP();NOP();NOP();\
//asm("Loop"x":");BTFSS();asm("GOTO Loop"x);

unsigned short ow_read_word(void) {
    unsigned char H;
    unsigned char L;
    
	WREG = 0;
    OW_BIT_IN("0");    OW_BIT_IN("1");    OW_BIT_IN("2");    OW_BIT_IN("3");
    OW_BIT_IN("4");    OW_BIT_IN("5");    OW_BIT_IN("6");    OW_BIT_IN("7");
	L = WREG;
	
	WREG = 0;
    OW_BIT_IN("8");    OW_BIT_IN("9");    OW_BIT_IN("10");    OW_BIT_IN("11");
    OW_BIT_IN("12");    OW_BIT_IN("13");    OW_BIT_IN("14");    OW_BIT_IN("15");
	H = WREG;
	
    return ((H<<8) | L);
}
