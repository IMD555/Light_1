/*
 * File:   newmain.c
 * Author: imada
 *
 * Created on 2021/03/02, 0:01
 */

#include <xc.h>
#include "MAX17320.h"
#include "MAX17345.h"

// PIC18LF67K40 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config FEXTOSC = OFF    // External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_1MHZ// Power-up default value for COSC bits (HFINTOSC with HFFRQ = 4 MHz and CDIV = 4:1)

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // Master Clear Enable bit (If LVP = 0, MCLR pin function is port defined function; If LVP =1, RG5 pin fuction is MCLR)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (Power up timer disabled)
#pragma config LPBOREN = ON     // Low-power BOR enable bit (ULPBOR enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled)

// CONFIG2H
#pragma config BORV = VBOR_190  // Brown Out Reset Voltage selection bits (Brown-out Reset Voltage (VBOR) set to 1.90V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = OFF    // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be set and cleared repeatedly (subject to the unlock sequence))
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)
#pragma config WRT4 = OFF       // Write Protection Block 3 (Block 4 (010000-013FFFh) not write-protected)
#pragma config WRT5 = OFF       // Write Protection Block 3 (Block 5 (014000-017FFFh) not write-protected)
#pragma config WRT6 = OFF       // Write Protection Block 3 (Block 6 (018000-01BFFFh) not write-protected)
#pragma config WRT7 = OFF       // Write Protection Block 3 (Block 7 (01C000-01FFFFh) not write-protected)

// CONFIG4H
#pragma config WRTC = ON        // Configuration Register Write Protection bit (Configuration registers (300000-30000Bh) write-protected)
#pragma config WRTB = ON        // Boot Block Write Protection bit (Boot Block (000000-0007FFh) write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config SCANE = OFF      // Scanner Enable bit (Scanner module is NOT available for use, SCANMD bit is ignored)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (HV on MCLR/VPP must be used for programming)

// CONFIG5L
#pragma config CP = OFF         // UserNVM Program Memory Code Protection bit (UserNVM code protection disabled)
#pragma config CPD = OFF        // DataNVM Memory Code Protection bit (DataNVM code protection disabled)

// CONFIG5H

// CONFIG6L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR4 = OFF      // Table Read Protection Block 4 (Block 4 (010000-013FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR5 = OFF      // Table Read Protection Block 5 (Block 5 (014000-017FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR6 = OFF      // Table Read Protection Block 6 (Block 6 (018000-01BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR7 = OFF      // Table Read Protection Block 7 (Block 7 (01C000-01FFFFh) not protected from table reads executed in other blocks)

// CONFIG6H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)



#define	USB_CBUS0		RF6
#define	USB_CBUS1		RF7
#define	USB_CBUS3		RF5
#define	USB_PWREN		!USB_CBUS0	//PWREN#
#define	USB_SLEEP		!USB_CBUS1	//SLEEP#
#define	USB_BCD_CHGR	USB_CBUS3	//BCD_Charger
#define USB_Host		(USB_PWREN&&!USB_SLEEP&&!USB_BCD_CHGR)
#define	USB_SetReady(x)	(LATF0=x)
#define	USB_Ready		RF0

#define LED		LATD7

char hello[] = "\r\nHello!\r\n";

void SendCharUSB(unsigned char d);
void showError(unsigned char* t);
void showText(unsigned char* t);


void main(void)
{
	//Reset RN42
	TRISAbits.TRISA4 = 0;
	
	//LED ON
	TRISDbits.TRISD7 = 0;
	LED = 1;

	//PAUSE FT230X
	ODCONFbits.ODCF0 = 1;
	TRISFbits.TRISF0 = 0;
	ANSELFbits.ANSELF0 = 0;
	USB_SetReady(0);

	// Initialize the device to bootloader
	// NOSC HFINTOSC; NDIV 1; 
	OSCCON1 = 0x60;
	// HFFRQ 4_MHz; 
	OSCFRQ = 0x02;

	ANSELF &= 0x1f;

	ANSELGbits.ANSELG2 = 0;
	RX5PPSbits.RX5PPS = 0x32; //RG2->EUSART5:RX5;

	// ABDOVF no_overflow; SCKP Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
	BAUD5CON = 0x18;
	// SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
	RC5STA = 0x90;
	// TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
	TX5STA = 0x24;
	// SP5BRGL	9600bps
	SP5BRGL = 103;
	//	// SP5BRGH
	//	SP5BRGH = 0;

	RG3PPS = 0x14; //RG3->EUSART5:TX5;
	TRISGbits.TRISG3 = 0;

	//Start receiving
	USB_SetReady(1);

	showText(hello);
	
	MAX17345_init();
//	MAX17320_init();
	
	T2PR = 0x3f;
	T2CON = 0x80;
	T2CLKCON = 0x01;
	CCPTMRS1bits.P7TSEL = 0;
	PWM7DC = 0x03c0;
	PWM7CON = 0x80;
	RE0PPS = 0x0b;
	RE1PPS = 0x0b;
	TRISEbits.TRISE0 = 0;
	TRISEbits.TRISE1 = 0;
			
	while (1) {
//    MAX17320_dump();
	MAX17345_dump();
	}
}

void SendCharUSB(unsigned char d)
{
	while (!PIR4bits.TX5IF);

	if (USB_Host)
	{
		//		if (!USB_Ready)
		//		{
		//			unsigned char cccc = 0;
		//			for (; cccc < 10; cccc++)
		//			{
		//				Nop();
		//			}
		//		}

		TX5REG = d;
	}

	//	while (!TX5STAbits.TRMT);
}

void showText(unsigned char* t)
{
	TBLPTRU = 0;
	while (*t)
	{
		SendCharUSB(*t++);
	}
}

//void showHex2(unsigned char n)
//{
//	SendCharUSB('0');
//	SendCharUSB('x');
//	SendCharUSB(hex[n / 0x10]);
//	SendCharUSB(hex[ n % 0x10]);
//	SendCharUSB(',');
//}
//
//void showHex4(unsigned short n)
//{
//	SendCharUSB('0');
//	SendCharUSB('x');
//	SendCharUSB(hex[n / 0x1000]);
//	SendCharUSB(hex[n / 0x100 % 0x10]);
//	SendCharUSB(hex[n / 0x10 % 0x10]);
//	SendCharUSB(hex[n % 0x10]);
//	SendCharUSB(',');
//}
