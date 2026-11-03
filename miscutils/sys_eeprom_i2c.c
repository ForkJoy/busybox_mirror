#include "libbb.h"
#include "onie_tlvinfo.h"
#include "sys_eeprom.h"
#include "24cXX.h"

/* address width is a runtime setting now */
static int eeprom_type(void)
{
    return (SYS_EEPROM_I2C_MEM_ADDR_BITS == 16) ? EEPROM_TYPE_16BIT_ADDR
						: EEPROM_TYPE_8BIT_ADDR;
}

/*
 * read_sys_eeprom - read the hwinfo from i2c EEPROM
 */
int i2c_read_sys_eeprom(void *eeprom_data, int offset, int len)
{
    int ret = 0;
    struct eeprom e;
    int i = 0;
    u_int8_t *c;
    int addr = SYS_EEPROM_OFFSET + offset;

    c = eeprom_data;
    if (eeprom_open(SYS_EEPROM_I2C_DEVICE, SYS_EEPROM_I2C_ADDR,
		    eeprom_type(), &e)) {
	printf("ERROR: Cannot open I2C device\n");
	return -1;
    }
    for (i = 0; i < len; i++) {
	*c = eeprom_read_byte(&e, addr);
	c++; addr++;
    }
    eeprom_close(&e);
    return ret;
}

/*
 * write_sys_eeprom - write the hwinfo to i2c EEPROM
 */
int i2c_write_sys_eeprom(void *eeprom_data, int len)
{
    int ret = 0;
    struct eeprom e;
    int i = 0;
    u_int8_t *c;
    u_int16_t  addr = SYS_EEPROM_OFFSET;

    c = eeprom_data;
    for (i = 0; i < len; i++) {
	if (eeprom_open(SYS_EEPROM_I2C_DEVICE, SYS_EEPROM_I2C_ADDR,
			eeprom_type(), &e)) {
	    printf("ERROR: Cannot open I2C device\n");
	    return -1;
	}
	eeprom_write_byte(&e, addr, *c);
	eeprom_close(&e);
	c++; addr++;
    }

    return ret;
}
