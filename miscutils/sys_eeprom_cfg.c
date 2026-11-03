/*
 * ONIE system EEPROM - runtime configuration and backend dispatch.
 *
 * Priority: command line > config file > Kconfig build-time defaults.
 *
 * Config file format (blank lines and '#' comments ignored):
 *
 *	backend   = i2c|mtd|disk|sysfs
 *	device    = /dev/i2c-1
 *	addr      = 0x57
 *	addr_bits = 8
 *	offset    = 0
 *	size      = 256
 *	max_size  = 2048
 */

#include "libbb.h"
#include "sys_eeprom.h"

struct syseeprom_cfg syseeprom_cfg;

static char cfg_dev_buf[256];

/* Kconfig choice selects the backend used when nothing is specified.
 * NB: the choice members are #undef'ed by busybox's autoconf.h when not
 * selected, hence the #ifdef (a plain #if would trip -Wundef). */
#ifdef CONFIG_SYS_EEPROM_DEVICE_I2C
#define SE_DEFAULT_BACKEND SE_BACKEND_I2C
#elif defined(CONFIG_SYS_EEPROM_DEVICE_MTD)
#define SE_DEFAULT_BACKEND SE_BACKEND_MTD
#elif defined(CONFIG_SYS_EEPROM_DEVICE_DISK)
#define SE_DEFAULT_BACKEND SE_BACKEND_DISK
#elif defined(CONFIG_SYS_EEPROM_SYSFS_FILE)
#define SE_DEFAULT_BACKEND SE_BACKEND_SYSFS
#else
#define SE_DEFAULT_BACKEND SE_BACKEND_I2C
#endif

/*
 * busybox's kconfig writes a hex symbol that carries no value as plain
 * "0x" (it looks for the 'x' in the stored string), which is not a valid
 * C constant - so only reference the value when the i2c backend was the
 * one actually selected at build time.
 */
#ifdef CONFIG_SYS_EEPROM_DEVICE_I2C
#define SE_DFLT_I2C_ADDR CONFIG_SYS_EEPROM_I2C_ADDR
#else
#define SE_DFLT_I2C_ADDR 0
#endif

static const char *backend_names[] = { "i2c", "mtd", "disk", "sysfs" };

const char *syseeprom_cfg_backend_name(int backend)
{
	if (backend < 0 || backend >= (int)ARRAY_SIZE(backend_names))
		return "unknown";
	return backend_names[backend];
}

int syseeprom_cfg_set_backend(const char *name, int prio)
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(backend_names); i++) {
		if (strcmp(name, backend_names[i]) == 0) {
			/*
			 * The device belongs to the backend: drop a device that
			 * came from a lower priority source (e.g. the config file
			 * said sysfs, the command line now asks for disk).
			 * A device given at the same or higher priority wins and
			 * is kept, so order on the command line does not matter.
			 */
			if (prio > syseeprom_cfg.dev_prio) {
				syseeprom_cfg.dev = NULL;
				syseeprom_cfg.dev_prio = SE_PRIO_NONE;
			}
			syseeprom_cfg.backend = i;
			return 0;
		}
	}
	fprintf(stderr, "ERROR: unknown backend '%s' (use i2c, mtd, disk or sysfs)\n", name);
	return -1;
}

void syseeprom_cfg_set_device(const char *dev, int prio)
{
	snprintf(cfg_dev_buf, sizeof(cfg_dev_buf), "%s", dev);
	syseeprom_cfg.dev = cfg_dev_buf;
	syseeprom_cfg.dev_prio = prio;
}

/* build-time default device for a given backend */
static const char *default_dev(int backend)
{
	switch (backend) {
	case SE_BACKEND_I2C:
		return CONFIG_SYS_EEPROM_I2C_DEVICE;
	case SE_BACKEND_MTD:
		return CONFIG_SYS_EEPROM_MTD_DEVICE;
	case SE_BACKEND_DISK:
		return CONFIG_SYS_EEPROM_DISK_DEVICE;
	case SE_BACKEND_SYSFS:
		return CONFIG_SYS_EEPROM_SYSFS_FILE_PATH;
	}
	return "";
}

void syseeprom_cfg_init(void)
{
	memset(&syseeprom_cfg, 0, sizeof(syseeprom_cfg));
	syseeprom_cfg.backend = SE_DEFAULT_BACKEND;
	syseeprom_cfg.dev = NULL;
	syseeprom_cfg.addr = SE_DFLT_I2C_ADDR;
	syseeprom_cfg.addr_bits = CONFIG_SYS_EEPROM_I2C_MEM_ADDR_BITS;
	syseeprom_cfg.offset = CONFIG_SYS_EEPROM_OFFSET;
	syseeprom_cfg.size = CONFIG_SYS_EEPROM_SIZE;
	syseeprom_cfg.max_size = CONFIG_SYS_EEPROM_MAX_SIZE;
	syseeprom_cfg.dev_prio = SE_PRIO_NONE;
}

/* trim leading/trailing blanks, returns the string itself */
static char *se_trim(char *s)
{
	char *e;

	while (*s == ' ' || *s == '\t')
		s++;
	e = s + strlen(s);
	while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\n' || e[-1] == '\r'))
		*--e = 0;
	return s;
}

static int cfg_apply(const char *key, const char *val)
{
	if (strcmp(key, "backend") == 0) {
		return syseeprom_cfg_set_backend(val, SE_PRIO_FILE);
	}
	if (strcmp(key, "device") == 0) {
		syseeprom_cfg_set_device(val, SE_PRIO_FILE);
		return 0;
	}
	if (strcmp(key, "addr") == 0) {
		syseeprom_cfg.addr = strtoul(val, NULL, 0);
		return 0;
	}
	if (strcmp(key, "addr_bits") == 0) {
		syseeprom_cfg.addr_bits = (int)strtoul(val, NULL, 0);
		return 0;
	}
	if (strcmp(key, "offset") == 0) {
		syseeprom_cfg.offset = (int)strtoul(val, NULL, 0);
		return 0;
	}
	if (strcmp(key, "size") == 0) {
		syseeprom_cfg.size = (int)strtoul(val, NULL, 0);
		return 0;
	}
	if (strcmp(key, "max_size") == 0) {
		syseeprom_cfg.max_size = (int)strtoul(val, NULL, 0);
		return 0;
	}
	fprintf(stderr, "ERROR: unknown config key '%s'\n", key);
	return -1;
}

/*
 * Returns 0 on success, 1 if the file does not exist, -1 on error.
 */
int syseeprom_cfg_load(const char *path)
{
	FILE *fp;
	char line[256];
	int lineno = 0;
	int ret = 0;

	if (!path || !*path) {
		path = CONFIG_SYS_EEPROM_CONF_FILE;
		/* 'make defconfig' leaves string symbols that were invisible
		 * at the time empty, so keep a sane fallback */
		if (!*path)
			path = "/etc/onie-syseeprom.conf";
	}

	fp = fopen(path, "r");
	if (!fp)
		return (errno == ENOENT) ? 1 : -1;

	while (fgets(line, sizeof(line), fp)) {
		char *s, *eq, *key, *val;

		lineno++;
		s = se_trim(line);
		if (*s == 0 || *s == '#')
			continue;
		eq = strchr(s, '=');
		if (!eq) {
			fprintf(stderr, "ERROR: %s:%d: missing '='\n", path, lineno);
			ret = -1;
			break;
		}
		*eq = 0;
		key = se_trim(s);
		val = se_trim(eq + 1);
		if (cfg_apply(key, val)) {
			fprintf(stderr, "ERROR: %s:%d: bad line\n", path, lineno);
			ret = -1;
			break;
		}
	}
	fclose(fp);
	return ret;
}

int syseeprom_cfg_validate(void)
{
	/* device not given explicitly: use the build-time default of the
	 * backend that is in effect now */
	if (syseeprom_cfg.dev_prio == SE_PRIO_NONE) {
		snprintf(cfg_dev_buf, sizeof(cfg_dev_buf), "%s",
				default_dev(syseeprom_cfg.backend));
		syseeprom_cfg.dev = cfg_dev_buf;
	}

	if (!syseeprom_cfg.dev || !syseeprom_cfg.dev[0]) {
		fprintf(stderr, "ERROR: no device specified for backend '%s'\n",
			syseeprom_cfg_backend_name(syseeprom_cfg.backend));
		return -1;
	}
	if (syseeprom_cfg.backend == SE_BACKEND_I2C) {
		if (syseeprom_cfg.addr == 0) {
			fprintf(stderr, "ERROR: i2c address not specified\n");
			return -1;
		}
		if (syseeprom_cfg.addr_bits != 8 && syseeprom_cfg.addr_bits != 16) {
			fprintf(stderr, "ERROR: addr_bits must be 8 or 16\n");
			return -1;
		}
	}
	if (syseeprom_cfg.size <= 0) {
		fprintf(stderr, "ERROR: size not specified\n");
		return -1;
	}
	if (syseeprom_cfg.offset < 0) {
		fprintf(stderr, "ERROR: bad offset\n");
		return -1;
	}
	if (syseeprom_cfg.max_size < syseeprom_cfg.size + syseeprom_cfg.offset) {
		fprintf(stderr, "ERROR: size (%d) + offset (%d) exceeds max_size (%d)\n",
			syseeprom_cfg.size, syseeprom_cfg.offset,
			syseeprom_cfg.max_size);
		return -1;
	}
	if (syseeprom_cfg.max_size > SYS_EEPROM_BUF_SIZE) {
		fprintf(stderr, "ERROR: max_size (%d) exceeds the build-time buffer size (%d)\n",
			syseeprom_cfg.max_size, SYS_EEPROM_BUF_SIZE);
		return -1;
	}
	if (syseeprom_cfg.backend == SE_BACKEND_DISK) {
		if (syseeprom_cfg.offset != 0 || syseeprom_cfg.size != 2048 ||
		    syseeprom_cfg.max_size != 2048) {
			fprintf(stderr, "ERROR: disk backend requires offset=0, size=2048 and max_size=2048\n");
			return -1;
		}
	}
	return 0;
}

/*
 * Runtime dispatcher: all backends are always compiled in.
 */
int read_sys_eeprom(void *eeprom_data, int offset, int len)
{
	switch (syseeprom_cfg.backend) {
	case SE_BACKEND_I2C:
		return i2c_read_sys_eeprom(eeprom_data, offset, len);
	case SE_BACKEND_MTD:
		return mtd_read_sys_eeprom(eeprom_data, offset, len);
	case SE_BACKEND_DISK:
		return disk_read_sys_eeprom(eeprom_data, offset, len);
	case SE_BACKEND_SYSFS:
		return sysfs_read_sys_eeprom(eeprom_data, offset, len);
	}
	return -1;
}

int write_sys_eeprom(void *eeprom_data, int len)
{
	switch (syseeprom_cfg.backend) {
	case SE_BACKEND_I2C:
		return i2c_write_sys_eeprom(eeprom_data, len);
	case SE_BACKEND_MTD:
		return mtd_write_sys_eeprom(eeprom_data, len);
	case SE_BACKEND_DISK:
		return disk_write_sys_eeprom(eeprom_data, len);
	case SE_BACKEND_SYSFS:
		return sysfs_write_sys_eeprom(eeprom_data, len);
	}
	return -1;
}