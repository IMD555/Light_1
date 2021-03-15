unsigned char MAX17320_init(void);
unsigned char MAX17320_get_SOC_percentage(unsigned char *SOC);
unsigned char MAX17320_get_capacity_remains(unsigned short *cap_rem);
unsigned char MAX17320_get_capacity_full(unsigned short *cap_full);
void MAX17320_dump(void);
void MAX17320_reset(void);

