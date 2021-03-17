#include <xc.h>
#include <stdio.h>

#define ONEWIRE
#ifdef	ONEWIRE
#include <onewire.h>
#endif

#include "MAX17320.h"

#define ERROR	1
#define OK		0

#define	NET_SKIP	0x00CC
#define	F_READ		0x6900
#define	F_WRITE		0x6C00

#define LED		LATD7

void SendCharUSB(unsigned char d);

void putch(unsigned char data){
    SendCharUSB(data);
}

unsigned char MAX17320_read_word(unsigned short address, unsigned short *data) {
#ifdef	ONEWIRE
    if (ow_reset()) {
        // ack
        ow_write_word(NET_SKIP | F_READ);
        ow_write_word(address);
        *data = ow_read_word();
        return OK;
    } else {
        // nack
        return ERROR;
    }
#endif
    return ERROR;
}

unsigned char MAX17320_write_word(unsigned short address, unsigned short data) {
#ifdef	ONEWIRE
    if (ow_reset()) {
        // ack
        ow_write_word(NET_SKIP | F_WRITE);
        ow_write_word(address);
        ow_write_word(data);
        return OK;
    } else {
        // nack
        return ERROR;
    }
#endif
    return ERROR;
}

unsigned char MAX17320_init(void) {
volatile    unsigned short x = 0x500a;
    unsigned char r = 0;
    unsigned short data;

#ifdef	ONEWIRE
    ow_init();
#endif
	
	WPUE = 0xff;
//    putchar('c');
    printf("boot!");
    
    MAX17320_dump();

//    while (1) {
//	    if(!MAX17320_read_word(0x1b5,&data)){printf("nPackCfg   =%04x\n",data);}
////        ow_write_word(0x0aff);
////        ow_write_word(x);
////        x = ow_read_word();
////        NOP();
//    }

    if ((r = MAX17320_read_word(0x01B5, &data)))return r;
    if (data != 0x4102) {
//    if (1) {
//    if (0) {
        // if NVM programming is needed

        // Unlock
        if ((r = MAX17320_write_word(0x061, 0x0000)))return r;
        if ((r = MAX17320_write_word(0x061, 0x0000)))return r;
		
		// nPackCfg 4-Cells No-Thrms AOLDO:3.4V
        if ((r = MAX17320_write_word(0x01B5, 0x4102)))return r;

        // nUVPrtTh UVP:2.88V UOCVP:3.00V UVShdn:2.80V
        if ((r = MAX17320_write_word(0x01D0, 0x443E)))return r;

        // nJEITAV 4.1V
        if ((r = MAX17320_write_word(0x01D9, 0xEC59)))return r;

        // nOVPrtTh Default

        // nBalTh Zener 10.0mV 11.7mR 30mV
        if ((r = MAX17320_write_word(0x01D4, 0x2C63)))return r;

        // nODSCTh CHG:5.75A Short:7A DIS:5.5A
        if ((r = MAX17320_write_word(0x01DD, 0x2314)))return r;

        // nODSCCfg Default

        // nIPrtTh1 Default(OCP ODP)

//        // nJEITAC Charge current:5A
//        if ((r = MAX17320_write_word(0x01D8, 0x7DFF)))return r;
        // nJEITAC Charge current:1.5A
        if ((r = MAX17320_write_word(0x01D8, 0x25FF)))return r;

        // nTPrtTh1 TooHot:45C TooCold:0C
        if ((r = MAX17320_write_word(0x01D1, 0x2D00)))return r;

        // nTPrtTh2 Hot:35C Cold:5C
        if ((r = MAX17320_write_word(0x01D5, 0x2305)))return r;

        // nTPrtTh3 PFail:85C Warm:32C
        if ((r = MAX17320_write_word(0x01D2, 0x5520)))return r;

        // nProtMiscTh Default
        // nDelayCfg Default
        // nProtCfg2 Default
        // nTCurve Default
		
        // nProtCfg Default
        if ((r = MAX17320_write_word(0x1d7, 0x0900)))return r;
		
        // nDesignCap 10000mAh
        if ((r = MAX17320_write_word(0x01B3, 10000)))return r;
		
//        // nNVCfg0 enDC:1
//        if ((r = MAX17320_write_word(0x01B8, 0x0a10)))return r;
		
		// clear NV error
        if ((r = MAX17320_write_word(0x061, 0x0000)))return r;
		
		LED = 0;
        // TODO: writetoNVM
    }
	
//    MAX17320_reset();
//	while(1){
    MAX17320_dump();
//	}
    return OK;
}

//unsigned char MAX17320_get_status(unsigned short *status)
//{
//
//}

unsigned char MAX17320_get_SOC_percentage(unsigned char *SOC) {
    unsigned char r = 0;
    unsigned short SOC_16 = 0;

    r = MAX17320_read_word(0x0006, &SOC_16);

    *SOC = SOC_16 >> 8;

    return r;
}

unsigned char MAX17320_get_capacity_remains(unsigned short *cap_rem) {
    unsigned char r = 0;

    r = MAX17320_read_word(0x0006, cap_rem);

    return r;
}

unsigned char MAX17320_get_capacity_full(unsigned short *cap_full) {
    unsigned char r = 0;

    r = MAX17320_read_word(0x0010, cap_full);
    return r;
}

void MAX17320_reset(void)
{
	unsigned short data;
	MAX17320_write_word(0x0ab,0x8000);
	while(MAX17320_read_word(0x0ab,&data) != OK || (data&0x8000)!=0);
}

void MAX17320_dump(void)
{
    unsigned short data = 0;

    if(!MAX17320_read_word(0x005,&data)){printf("RC %5d[mAh]  ",data);}
    if(!MAX17320_read_word(0x018,&data)){printf("DC %5d[mAh]  ",data);}
    if(!MAX17320_read_word(0x006,&data)){printf("RepSOC %3d[%%]  ",data>>8);}
    if(!MAX17320_read_word(0x00e,&data)){printf("AvSOC  %3d[%%]  ",data>>8);}
    if(!MAX17320_read_word(0x01c,&data)){printf("%5d[mA]  ",(signed int)((signed int)data*0.3125));}
    if(!MAX17320_read_word(0x0da,&data)){printf("%5d[mV]  \n",(int)(data*0.3125));}

//    printf("\nRAM\n");
    if(!MAX17320_read_word(0x000,&data)){printf("Status    =%04x\n",data);}
    if(!MAX17320_read_word(0x0b0,&data)){printf("Status2   =%04x\n",data);}
    if(!MAX17320_read_word(0x0af,&data)){printf("ProtAlert =%04x\n",data);}
    if(!MAX17320_read_word(0x0d9,&data)){printf("ProtStatus=%04x\n",data);}
    if(!MAX17320_read_word(0x00b,&data)){printf("Config    =%04x\n",data);}
    if(!MAX17320_read_word(0x0ab,&data)){printf("Config2   =%04x\n",data);}
    if(!MAX17320_read_word(0x0a0,&data)){printf("RelaxCfg  =%04x\n",data);}
    if(!MAX17320_read_word(0x005,&data)){printf("RepCap    =%04x  %5d[mAh]\n",data,data);}
    if(!MAX17320_read_word(0x018,&data)){printf("DesignCap =%04x  %5d[mAh]\n",data,data);}
    if(!MAX17320_read_word(0x006,&data)){printf("RepSOC    =%04x  %3d[%%]\n",data,data>>8);}
    if(!MAX17320_read_word(0x00e,&data)){printf("AvSOC     =%04x  %3d[%%]\n",data,data>>8);}
    if(!MAX17320_read_word(0x0d8,&data)){printf("CELL1     =%04x  %5d[mV]\n",data,(int)(data*0.078125));}
    if(!MAX17320_read_word(0x0d7,&data)){printf("CELL2     =%04x  %5d[mV]\n",data,(int)(data*0.078125));}
    if(!MAX17320_read_word(0x0d6,&data)){printf("CELL3     =%04x  %5d[mV]\n",data,(int)(data*0.078125));}
    if(!MAX17320_read_word(0x0d5,&data)){printf("CELL4     =%04x  %5d[mV]\n",data,(int)(data*0.078125));}
    if(!MAX17320_read_word(0x0da,&data)){printf("Batt      =%04x  %5d[mV]\n",data,(int)(data*0.3125));}
    if(!MAX17320_read_word(0x0db,&data)){printf("PCKP      =%04x  %5d[mV]\n",data,(int)(data*0.3125));}
    if(!MAX17320_read_word(0x01b,&data)){printf("Temp      =%04x  %3d[degC]\n",data,data>>8);}
    if(!MAX17320_read_word(0x01c,&data)){printf("Current   =%04x  %5d[mA]\n",data,(signed int)((signed int)data*0.3125));}
    if(!MAX17320_read_word(0x028,&data)){printf("ChgingCurr=%04x  %5d[mA]\n",data,(int)(data*0.3125));}
    if(!MAX17320_read_word(0x02a,&data)){printf("ChgingVolt=%04x  %5d[mV]\n",data,(int)(data*0.078125));}
    if(!MAX17320_read_word(0x1f1,&data)){printf("HProtCfg2 =%04x\n",data);}
    if(!MAX17320_read_word(0x061,&data)){printf("CommStat  =%04x\n",data);}
//
//    printf("\nNVM\n");
//    if(!MAX17320_read_word(0x19f,&data)){printf("nLearnCfg  =%04x\n",data);}
//    if(!MAX17320_read_word(0x1a9,&data)){printf("nFullCapRep=%04x  %5d[mAh]\n",data,data);}
//    if(!MAX17320_read_word(0x1a5,&data)){printf("nFullCapNom=%04x  %5d[mAh]\n",data,data);}
//    if(!MAX17320_read_word(0x1b3,&data)){printf("nDesignCap =%04x  %5d[mAh]\n",data,data);}
//    if(!MAX17320_read_word(0x1a8,&data)){printf("nBattStatus=%04x\n",data);}
//    if(!MAX17320_read_word(0x1b0,&data)){printf("nConfig    =%04x\n",data);}
//    if(!MAX17320_read_word(0x1b6,&data)){printf("nRelaxCfg  =%04x\n",data);}
//    if(!MAX17320_read_word(0x1b5,&data)){printf("nPackCfg   =%04x\n",data);}
//    if(!MAX17320_read_word(0x1c2,&data)){printf("nChgCfg    =%04x\n",data);}
//    if(!MAX17320_read_word(0x1c3,&data)){printf("nChgCtrl   =%04x\n",data);}
//    if(!MAX17320_read_word(0x1b7,&data)){printf("nConvgCfg  =%04x\n",data);}
//    if(!MAX17320_read_word(0x1bb,&data)){printf("nHibCfg    =%04x\n",data);}
//    if(!MAX17320_read_word(0x1ca,&data)){printf("nThermCfg  =%04x\n",data);}
//    if(!MAX17320_read_word(0x1d7,&data)){printf("nProtCfg   =%04x\n",data);}
//    if(!MAX17320_read_word(0x1df,&data)){printf("nProtCfg2  =%04x\n",data);}
//    if(!MAX17320_read_word(0x1d8,&data)){printf("nJEITAC    =%04x\n",data);}
//    if(!MAX17320_read_word(0x1d9,&data)){printf("nJEITAV    =%04x\n",data);}
//    if(!MAX17320_read_word(0x1b8,&data)){printf("nNVCfg0    =%04x\n",data);}
//    if(!MAX17320_read_word(0x1fd,&data)){printf("NVM_USED   =%04x\n",data);}
}
