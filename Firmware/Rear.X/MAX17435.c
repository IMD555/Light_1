#include <xc.h>
#include <stdio.h>

#define ERROR	1
#define OK		0

#define	MAX17435_WR	0x12
#define	MAX17435_RD	(MAX17435_WR | 1)

void i2c_start(void)
{
	SSP1CON2bits.SEN = 1;
	while(SSP1CON2bits.SEN);
}

void i2c_restart(void)
{
	SSP1CON2bits.RSEN = 1;
	while(SSP1CON2bits.RSEN);
}

void i2c_stop(void)
{
	SSP1CON2bits.PEN = 1;
	while(SSP1CON2bits.PEN);
}

void i2c_ack(void)
{
	SSP1CON2bits.ACKDT = 0;
	SSP1CON2bits.ACKEN = 1;
	while(SSP1CON2bits.ACKEN);
}

void i2c_nack(void)
{
	SSP1CON2bits.ACKDT = 1;
	SSP1CON2bits.ACKEN = 1;
	while(SSP1CON2bits.ACKEN);
}

unsigned char i2c_read_byte(unsigned char *data)
{
	SSP1CON2bits.RCEN = 1;
	while(SSP1CON2bits.RCEN);
	*data  = SSP1BUF;
	return OK;
}

unsigned char i2c_write_byte(unsigned char data)
{
	SSP1BUF = data;
	while(SSP1STATbits.RW);
	return SSP1CON2bits.ACKSTAT;
}

unsigned char MAX17435_read_word(unsigned short command, unsigned short *data)
{
	unsigned char r = 0;
	unsigned char dataH = 0;
	unsigned char dataL = 0;

	i2c_start();
	if (r = i2c_write_byte(MAX17435_WR))return r;
	if (r = i2c_write_byte(command))return r;
	i2c_restart();
	if (r = i2c_write_byte(MAX17435_RD))return r;
	if (r = i2c_read_byte(&dataL))return r;
	i2c_ack();
	if (r = i2c_read_byte(&dataH))return r;
	i2c_nack();
	i2c_stop();
	*data = dataL | ((dataH) << 8);
	return OK;
}

unsigned char MAX17435_write_word(unsigned short command, unsigned short data)
{
	unsigned char r = 0;

	i2c_start();
	if (r = i2c_write_byte(MAX17435_WR))return r;
	if (r = i2c_write_byte(command))return r;
	if (r = i2c_write_byte(data))return r;
	if (r = i2c_write_byte(data >> 8))return r;
	i2c_stop();
	return OK;
}

//unsigned char MAX17435_get_status(unsigned short *status)
//{
//
//}

unsigned char MAX17435_get_Dev_ID(unsigned short *id)
{
	unsigned char r = 0;

	r = MAX17435_read_word(0xFF, id);
	return r;
}

unsigned char MAX17435_get_IINPVoltage(unsigned short *iinpv)
{
	unsigned char r = 0;

	r = MAX17435_read_word(0x3E, iinpv);
	return r;
}

unsigned char MAX17435_set_charge_current(unsigned short charge_current)
{
	unsigned char r = 0;

	r = MAX17435_write_word(0x14, charge_current);
	return r;
}

unsigned char MAX17435_set_charge_voltage(unsigned short charge_voltage)
{
	unsigned char r = 0;

	r = MAX17435_write_word(0x15, charge_voltage);
	return r;
}

unsigned char MAX17435_set_input_current(unsigned short input_current)
{
	unsigned char r = 0;

	r = MAX17435_write_word(0x3F, input_current);
	return r;
}

unsigned char MAX17435_set_relearn_voltage(unsigned short relearn_voltage)
{
	unsigned char r = 0;

	r = MAX17435_write_word(0x3D, relearn_voltage);
	return r;
}

void MAX17435_dump(void)
{
    unsigned short data = 0;
	
//	MAX17435_set_charge_voltage(16400&0x7ff0);
//	MAX17435_set_charge_current((1024-128)&0x1f80);
	
    printf("\nMAX17435\n");
    if(!MAX17435_read_word(0x14,&data)){printf("ChargeCurrent  =%04x\n",data);}
    if(!MAX17435_read_word(0x15,&data)){printf("ChargeVoltage  =%04x\n",data);}
    if(!MAX17435_read_word(0x3d,&data)){printf("Relearn Voltage=%04x\n",data);}
    if(!MAX17435_read_word(0x3e,&data)){printf("IINPVoltage    =%04x %5d[mA]\n",data,data*80);}
    if(!MAX17435_read_word(0x3f,&data)){printf("InputCurrent   =%04x %5d[mA]\n",data,data*2);}
//    if(!MAX17435_read_word(0xfe,&data)){printf("ManufacturerID =%04x\n",data);}
//    if(!MAX17435_read_word(0xff,&data)){printf("DeviceID       =%04x\n",data);}
	
	// 10mR (12.8/16) V/LSB
// I = IINPVoltage()/16*12.8mV/10mR [A]
//   = IINPVoltage()/160*12.8 [A]
//   = IINPVoltage()/160*12.8*1000 [mA]
//   = IINPVoltage()*80 [mA]
}

unsigned char MAX17435_init(void)
{
	unsigned char r = 0;
	unsigned short data;
	
	// RC3:SCL
	// RC4:SDA
	RC3PPS = 0x19;
	RC4PPS = 0x1a;
	SSP1CLKPPS = 0x13;
	SSP1DATPPS = 0x14;
	WPUC |= 0x18;

	SSP1STAT = 0x40;
	SSP1CON1 = 0x08;
	SSP1CON2 = 0x00;
	SSP1CON3 = 0x00;
	SSP1ADD = 255;	
	SSP1CON1bits.SSPEN = 1;
	
	MAX17435_dump();
	
//	while(1);
	
	if((r = MAX17435_set_input_current(512)))return r; //1024mA
	if((r = MAX17435_set_charge_voltage(16400&0x7ff0)))return r;
//	if((r = MAX17435_set_charge_current((1024-128)&0x1f80)))return r;
	if((r = MAX17435_set_charge_current((512)&0x1f80)))return r;
//	if((r = MAX17435_set_charge_current((256)&0x1f80)))return r;
	if((r = MAX17435_set_relearn_voltage(16001&0x7ff1)))return r;
	
//	if((r = MAX17435_set_input_current(4096)))return r; //1024mA
//	if((r = MAX17435_set_charge_voltage(16400)))return r;
//	if((r = MAX17435_set_charge_current((128)&0x1f80)))return r;
//	if((r = MAX17435_set_relearn_voltage(16000&0x7ff1)))return r;
	
	MAX17435_dump();
	return OK;
}
