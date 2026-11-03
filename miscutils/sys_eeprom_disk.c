#include "libbb.h"
#include "onie_tlvinfo.h"
#include "sys_eeprom.h"

/*
 * Storing the "eeprom" on "disk".
 *
 * This is not recommended practice.
 * In fact, this should be a last resort, for machines that
 * have no usable eeprom of any sort (ha! pun).
 *
 * The data is kept in the sectors just before the first GPT
 * partition.  It's part of the GPT area but currently unused.
 * The sector address is hard coded and must match that in
 * onie/installer/x86_64/install-arch (currently 2048).
 *
 * Also, the device name is configurable but fixed at build time,
 * and must match the actual boot device, which may have been
 * dynamically discovered at onie install time.
 */

/*
 * The disk layout constraints (offset == 0, size == max_size == 2048)
 * used to be enforced at build time; they are now checked at runtime by
 * syseeprom_cfg_validate().
 */

#define START_SECTOR (2048 - SYS_EEPROM_MAX_SIZE / 512)

static int open_dev(int offset)
{
    int fd;
    off64_t offset64;

    fd = open(SYS_EEPROM_DISK_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("ERROR: open(%s): %s\n",
               SYS_EEPROM_DISK_DEVICE, strerror(errno));
        return -1;
    }
    offset64 = (off64_t) 512 * START_SECTOR +
	       CONFIG_SYS_EEPROM_OFFSET + offset;
    if (lseek64(fd, offset64, SEEK_SET) < 0) {
        printf("ERROR: lseek64(%s): %s\n",
               SYS_EEPROM_DISK_DEVICE, strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

int disk_read_sys_eeprom(void *eeprom_data, int offset, int len)
{
    int fd;
    ssize_t n;
    int ret = -1;

    fd = open_dev(offset);
    if (fd < 0)
        goto out;
    n = read(fd, eeprom_data, len);
    if (n < 0) {
        printf("ERROR: read(%s): %s\n",
               SYS_EEPROM_DISK_DEVICE, strerror(errno));
        goto out;
    }
    if (n != len) {
	printf("ERROR: read(%s): read %zd byte(s) instead of %d bytes\n",
               SYS_EEPROM_DISK_DEVICE, n, len);
        goto out;
    }
    ret = 0;

out:
    if (fd >= 0)
        close(fd);
    return ret;
}

int disk_write_sys_eeprom(void *eeprom_data, int len)
{
    int fd;
    ssize_t n;
    int ret = -1;

    fd = open_dev(0);
    if (fd < 0)
        goto out;
    n = write(fd, eeprom_data, len);
    if (n < 0) {
        printf("ERROR: write(%s): %s\n",
               SYS_EEPROM_DISK_DEVICE, strerror(errno));
        goto out;
    }
    if (n != len) {
	printf("ERROR: write(%s): wrote %zd byte(s) instead of %d bytes\n",
               SYS_EEPROM_DISK_DEVICE, n, len);
        goto out;
    }
    ret = 0;

out:
    if (fd >= 0)
        close(fd);
    return ret;
}
