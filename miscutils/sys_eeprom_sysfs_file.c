#include "libbb.h"
#include "onie_tlvinfo.h"
#include "sys_eeprom.h"
#include <mtd/mtd-user.h>

/*
 * read_sys_eeprom - read the hwinfo from MTD EEPROM
 */
int sysfs_read_sys_eeprom(void *eeprom_data, int offset, int len)
{
	int rc;
	int ret = 0;
	int fd;
	u_int8_t *c;

	c = eeprom_data;

	fd = open(SYS_EEPROM_SYSFS_FILE_PATH, O_RDONLY);
	if (fd < 0) {
		fprintf (stderr, "Can't open %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	if (lseek (fd, offset + SYS_EEPROM_OFFSET, SEEK_SET) == -1) {
		fprintf (stderr, "Seek error on %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	rc = read (fd, c, len);
	if (rc != len) {
		fprintf (stderr, "Read error on %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	if (close (fd)) {
		fprintf (stderr, "I/O error on %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}
	return ret;
}

/*
 * write_sys_eeprom - write the hwinfo to MTD EEPROM
 */
int sysfs_write_sys_eeprom(void *eeprom_data, int len)
{
	int rc;
	int ret = 0;
	int fd;
	u_int8_t *c;

	c = eeprom_data;

	fd = open(SYS_EEPROM_SYSFS_FILE_PATH, O_RDWR);
	if (fd < 0) {
		fprintf (stderr, "Can't open %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	if (lseek (fd, SYS_EEPROM_OFFSET, SEEK_SET) == -1) {
		fprintf (stderr, "Seek error on %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	rc = write (fd, c, len);
	if (rc != len) {
		fprintf (stderr, "Write error on %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	if (close (fd)) {
		fprintf (stderr, "I/O error on %s: %s\n",
			SYS_EEPROM_SYSFS_FILE_PATH, strerror (errno));
		return -1;
	}

	return ret;
}
