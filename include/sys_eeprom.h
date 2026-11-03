/*
 * ONIE system EEPROM - runtime device configuration.
 *
 * The backend (i2c / MTD / disk / sysfs) and its parameters used to be
 * fixed at build time via Kconfig.  They are now runtime values:
 *
 *   命令行  >  配置文件  >  Kconfig 编译期默认值
 *
 * See syseeprom_cfg.c for the config file parser and the backend dispatcher.
 */

#ifndef SYS_EEPROM_H
#define SYS_EEPROM_H

#define SE_BACKEND_I2C   0
#define SE_BACKEND_MTD   1
#define SE_BACKEND_DISK  2
#define SE_BACKEND_SYSFS 3

/* where a setting came from; a higher priority overrides a lower one */
#define SE_PRIO_NONE 0  /* not set: use the build-time default */
#define SE_PRIO_FILE 1  /* config file                       */
#define SE_PRIO_CLI  2  /* command line                      */

struct syseeprom_cfg {
	int backend;        /* SE_BACKEND_*                                  */
	const char *dev;    /* /dev/i2c-N | mtd dev | disk dev | sysfs file  */
	unsigned long addr; /* i2c device address (e.g. 0x57)                */
	int addr_bits;      /* i2c: number of bits addressing a byte (8/16)  */
	int offset;         /* offset where the ONIE header starts           */
	int size;           /* usable eeprom size in bytes                   */
	int max_size;       /* total eeprom size in bytes                    */
	int dev_prio;       /* SE_PRIO_* of the device setting               */
};

extern struct syseeprom_cfg syseeprom_cfg;

/*
 * Compile-time size of the in-memory eeprom buffer.  Runtime max_size
 * may not exceed it.  Normally it is the Kconfig total size; the
 * fallback keeps the buffer sane when Kconfig carries no value.
 */
#if CONFIG_SYS_EEPROM_MAX_SIZE >= 256
#define SYS_EEPROM_BUF_SIZE CONFIG_SYS_EEPROM_MAX_SIZE
#else
#define SYS_EEPROM_BUF_SIZE 2048
#endif

void syseeprom_cfg_init(void);
int syseeprom_cfg_load(const char *path);
int syseeprom_cfg_validate(void);
int syseeprom_cfg_set_backend(const char *name, int prio);
void syseeprom_cfg_set_device(const char *dev, int prio);
const char *syseeprom_cfg_backend_name(int backend);

/*
 * Backend entry points (implemented in sys_eeprom_<backend>.c) and the
 * runtime dispatcher (implemented in syseeprom_cfg.c).
 */
int read_sys_eeprom(void *eeprom_data, int offset, int len);
int write_sys_eeprom(void *eeprom_data, int len);

int i2c_read_sys_eeprom(void *eeprom_data, int offset, int len);
int i2c_write_sys_eeprom(void *eeprom_data, int len);
int mtd_read_sys_eeprom(void *eeprom_data, int offset, int len);
int mtd_write_sys_eeprom(void *eeprom_data, int len);
int disk_read_sys_eeprom(void *eeprom_data, int offset, int len);
int disk_write_sys_eeprom(void *eeprom_data, int len);
int sysfs_read_sys_eeprom(void *eeprom_data, int offset, int len);
int sysfs_write_sys_eeprom(void *eeprom_data, int len);

#endif /* SYS_EEPROM_H */