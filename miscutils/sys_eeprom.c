/*
 * Applet / Kconfig / Kbuild glue for onie-syseeprom.
 * busybox >= 1.25 generates applets.h, Config.in and Kbuild from these
 * comments (scripts/gen_build_files.sh), so no shared template has to
 * be edited (and the patch cannot conflict on busybox upgrades).
 */

//applet:IF_SYS_EEPROM(APPLET_ODDNAME(onie-syseeprom, onie_syseeprom, BB_DIR_USR_BIN, BB_SUID_DROP, onie_syseeprom))

//config:config SYS_EEPROM
//config:	bool "ONIE system EEPROM"
//config:	default n
//config:	help
//config:	  Display and program the system EEPROM data block.
//config:	  The onie-syseeprom tool is included.
//config:
//config:	  The device type and its parameters can be changed at runtime
//config:	  (config file and/or command line options); the settings below
//config:	  provide the built-in defaults.  All four backends are always
//config:	  compiled in.
//config:
//config:menu "ONIE system EEPROM config"
//config:	depends on SYS_EEPROM
//config:
//config:config SYS_EEPROM_CONF_FILE
//config:	string "runtime config file"
//config:	default "/etc/onie-syseeprom.conf"
//config:	depends on SYS_EEPROM
//config:	help
//config:	  File read at startup for the runtime device configuration
//config:	  (backend, device, addr, addr_bits, offset, size, max_size).
//config:	  It can be overridden with -c and does not have to exist;
//config:	  command line options take precedence over it.
//config:
//config:choice
//config:	prompt "Choose the default device type of EEPROM"
//config:	depends on SYS_EEPROM
//config:	help
//config:	  Choose which device type of EEPROM.
//config:
//config:config SYS_EEPROM_DEVICE_I2C
//config:	bool "i2c"
//config:
//config:config SYS_EEPROM_DEVICE_MTD
//config:	bool "MTD"
//config:
//config:config SYS_EEPROM_DEVICE_DISK
//config:	bool "disk (not recommended)"
//config:
//config:config SYS_EEPROM_SYSFS_FILE
//config:	bool "sysfs file"
//config:
//config:endchoice
//config:
//config:config SYS_EEPROM_I2C_DEVICE
//config:	string "i2c bus"
//config:	default "/dev/i2c-1"
//config:	depends on SYS_EEPROM_DEVICE_I2C
//config:	help
//config:	  i2c bus name of the eeprom. e.g., /dev/i2c-1.
//config:
//config:config SYS_EEPROM_I2C_ADDR
//config:	hex "device address on the i2c bus"
//config:	default 0x0
//config:	depends on SYS_EEPROM_DEVICE_I2C
//config:	help
//config:	  i2c device address of the eeprom. e.g., 0x57.
//config:	  NB: write the value with a 0x prefix; the hex type needs it.
//config:
//config:config SYS_EEPROM_I2C_MEM_ADDR_BITS
//config:	int "eeprom address bits"
//config:	range 8 16
//config:	default 8
//config:	depends on SYS_EEPROM_DEVICE_I2C
//config:	help
//config:	  Number of bits needed to address a byte in the EEPROM.
//config:	  e.g., 8 or 16.
//config:
//config:config SYS_EEPROM_I2C_WRITE_1B_DELAY
//config:	int "delay time for i2c writing 1 byte"
//config:	range 10 1000000
//config:	default 10
//config:	depends on SYS_EEPROM_DEVICE_I2C
//config:	help
//config:	  Delay number of microseconds after writing 1 byte.
//config:
//config:config SYS_EEPROM_I2C_WRITE_2B_DELAY
//config:	int "delay time for i2c writing 2 bytes"
//config:	range 10 1000000
//config:	default 1000
//config:	depends on SYS_EEPROM_DEVICE_I2C
//config:	help
//config:	  Delay number of microseconds after writing 2 bytes.
//config:
//config:config SYS_EEPROM_I2C_WRITE_3B_DELAY
//config:	int "delay time for i2c writing 3 bytes"
//config:	range 10 1000000
//config:	default 10
//config:	depends on SYS_EEPROM_DEVICE_I2C
//config:	help
//config:	  Delay number of microseconds after writing 3 bytes.
//config:
//config:config SYS_EEPROM_MTD_DEVICE
//config:	string "MTD device"
//config:	default "/dev/mtd-hw-info"
//config:	depends on SYS_EEPROM_DEVICE_MTD
//config:	help
//config:	  MTD device name of the eeprom. e.g., /dev/mtd-hw-info.
//config:
//config:config SYS_EEPROM_DISK_DEVICE
//config:	string "disk device"
//config:	default "/dev/sda"
//config:	depends on SYS_EEPROM_DEVICE_DISK
//config:	help
//config:	  device name of the on-disk "eeprom". e.g., /dev/sda.
//config:
//config:config SYS_EEPROM_SYSFS_FILE_PATH
//config:	string "Path for sysfs eeprom file"
//config:	depends on SYS_EEPROM_SYSFS_FILE
//config:	help
//config:	  sysfs file path of the eeprom.
//config:	  e.g., /sys/bus/i2c/devices/1-0050/eeprom
//config:
//config:config SYS_EEPROM_OFFSET
//config:	int "offset"
//config:	range 0 0 if SYS_EEPROM_DEVICE_DISK
//config:	range 0 65536
//config:	default 0
//config:	depends on SYS_EEPROM
//config:	help
//config:	  Offset from where the ONIE header starts.
//config:
//config:config SYS_EEPROM_SIZE
//config:	int "usable eeprom size in byte"
//config:	range 2048 2048 if SYS_EEPROM_DEVICE_DISK
//config:	range 0 2048
//config:	default 2048 if SYS_EEPROM_DEVICE_DISK
//config:	default 0
//config:	depends on SYS_EEPROM
//config:	help
//config:	  Size of usable eeprom.
//config:
//config:config SYS_EEPROM_MAX_SIZE
//config:	int "total size in byte"
//config:	range 2048 2048 if SYS_EEPROM_DEVICE_DISK
//config:	range 256 65536
//config:	default 2048
//config:	depends on SYS_EEPROM
//config:	help
//config:	  Total size of the eeprom.
//config:
//config:endmenu

//kbuild:lib-$(CONFIG_SYS_EEPROM) += sys_eeprom.o onie_tlvinfo.o sys_eeprom_cfg.o
//kbuild:lib-$(CONFIG_SYS_EEPROM) += sys_eeprom_i2c.o 24cXX.o sys_eeprom_mtd.o sys_eeprom_disk.o sys_eeprom_sysfs_file.o

#include "libbb.h"
#include "onie_tlvinfo.h"
#include "sys_eeprom.h"
#include <getopt.h>

/* compile-time buffer cap; runtime max_size may not exceed it */
static u_int8_t eeprom[SYS_EEPROM_BUF_SIZE];

/* set once the runtime configuration has been validated */
static int cfg_ok;

/*
 *  Validate the runtime configuration the first time the EEPROM is
 *  actually touched, so that -h / -l work without a usable device.
 */
static int ensure_cfg(void)
{
    if (!cfg_ok) {
	if (syseeprom_cfg_validate())
	    return -1;
	cfg_ok = 1;
    }
    return 0;
}

/*
 *  This macro defines the onie-syseeprom command line command.
 */
//usage:#define SE_USAGE_STR_(value) #value
//usage:#define SE_USAGE_STR(value) SE_USAGE_STR_(value)
//usage:#define onie_syseeprom_trivial_usage
//usage:       "[OPTIONS]"
//usage:#define onie_syseeprom_full_usage "\n\n"
//usage:       "Display and program the system EEPROM data block.\n"
//usage:       "With no arguments, display the EEPROM contents.\n"
//usage:     "\nOptions:"
//usage:     "\n  -h, --help                  Show this help and exit"
//usage:     "\n  -l, --list                  List supported TLV codes and names"
//usage:     "\n  -e, --erase                 Reset the EEPROM data"
//usage:     "\n  -g, --get CODE              Print the value of a TLV code"
//usage:     "\n  -s, --set CODE=VALUE,...    Set TLV values; omit VALUE to delete a TLV"
//usage:     "\n                              Replace existing TLVs, except Vendor Extension"
//usage:     "\n                              TLVs, which may be added multiple times"
//usage:     "\n  -c, --config FILE           Read device configuration"
//usage:     "\n                              (default: " CONFIG_SYS_EEPROM_CONF_FILE ")"
//usage:     "\n  -b, --backend TYPE          Backend: i2c, mtd, disk, sysfs"
//usage:     "\n                              (default: "
//usage:       IF_SYS_EEPROM_DEVICE_I2C("i2c")
//usage:       IF_SYS_EEPROM_DEVICE_MTD("mtd")
//usage:       IF_SYS_EEPROM_DEVICE_DISK("disk")
//usage:       IF_SYS_EEPROM_SYSFS_FILE("sysfs") ")"
//usage:     "\n  -f, --device DEVICE         Device path for the selected backend"
//usage:     "\n                              (default: "
//usage:       IF_SYS_EEPROM_DEVICE_I2C(CONFIG_SYS_EEPROM_I2C_DEVICE)
//usage:       IF_SYS_EEPROM_DEVICE_MTD(CONFIG_SYS_EEPROM_MTD_DEVICE)
//usage:       IF_SYS_EEPROM_DEVICE_DISK(CONFIG_SYS_EEPROM_DISK_DEVICE)
//usage:       IF_SYS_EEPROM_SYSFS_FILE(CONFIG_SYS_EEPROM_SYSFS_FILE_PATH) ")"
//usage:     "\n  -a, --addr ADDR             I2C device address (default: "
//usage:       IF_SYS_EEPROM_DEVICE_I2C(SE_USAGE_STR(CONFIG_SYS_EEPROM_I2C_ADDR))
//usage:       IF_NOT_SYS_EEPROM_DEVICE_I2C("0") ")"
//usage:     "\n  -w, --addr-bits BITS        I2C address bits: 8 or 16 (default: "
//usage:       SE_USAGE_STR(CONFIG_SYS_EEPROM_I2C_MEM_ADDR_BITS) ")"
//usage:     "\n      --offset BYTES          ONIE header offset in bytes (default: "
//usage:       SE_USAGE_STR(CONFIG_SYS_EEPROM_OFFSET) ")"
//usage:     "\n      --size BYTES            Usable EEPROM size in bytes (default: "
//usage:       SE_USAGE_STR(CONFIG_SYS_EEPROM_SIZE) ")"
//usage:     "\n\nDefaults are build-time values; the device default is for the"
//usage:     "\nbuild-time default backend."
//usage:     "\nAn empty config path falls back to /etc/onie-syseeprom.conf."
//usage:     "\nPriority: command line > config file > build-time defaults."

static void cmd_usage(void) NORETURN;
static void cmd_usage(void)
{
    bb_show_usage();
}

/*
 *  do_onie_syseeprom
 *  This function implements the onie_syseeprom command.
 */
/* long-only options */
enum { OPT_OFFSET = 0x100, OPT_SIZE };

int onie_syseeprom_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int onie_syseeprom_main(int argc, char **argv)
{
    int count = 0;
    int err = 0;
    int update = 0;
    int conf_given = 0;
    char *value, *subopts, *tname;
    int index, c, i, option_index, tcode;
    char tlv_value[TLV_DECODE_VALUE_MAX_LEN];

    const size_t tlv_code_count = sizeof(tlv_code_list) /
	sizeof(tlv_code_list[0]);

    char *tokens[(tlv_code_count*2) + 1];
    const char *short_options = "hels:g:c:b:f:a:w:";
    const struct option long_options[] = {
	{"help",      no_argument,        0,    'h'},
	{"list",      no_argument,        0,    'l'},
	{"erase",     no_argument,        0,    'e'},
	{"set",       required_argument,  0,    's'},
	{"get",       required_argument,  0,    'g'},
	{"config",    required_argument,  0,    'c'},
	{"backend",   required_argument,  0,    'b'},
	{"device",    required_argument,  0,    'f'},
	{"addr",      required_argument,  0,    'a'},
	{"addr-bits", required_argument,  0,    'w'},
	{"offset",    required_argument,  0,    OPT_OFFSET},
	{"size",      required_argument,  0,    OPT_SIZE},
	{0,           0,                  0,      0},
    };

    /* Match BusyBox's --help handling before loading any configuration. */
    if (argc > 1 && (strcmp(argv[1], "-h") == 0
                  || strcmp(argv[1], "--help") == 0)) {
        xfunc_error_retval = 0;
        cmd_usage();
    }

    for (i = 0; i < tlv_code_count; i++) {
	    tokens[i*2] = malloc(6);
	    snprintf(tokens[(i*2)], 6,     "0x%x", tlv_code_list[i].m_code);
	    /* Allow for uppercase hex digits as well */
	    tokens[(i*2) + 1] = malloc(6);
	    snprintf(tokens[(i*2) + 1], 6, "0x%X", tlv_code_list[i].m_code);
    }
    tokens[tlv_code_count*2] = NULL;

    /* built-in defaults first, then the config file, then the command line */
    syseeprom_cfg_init();
    for (i = 1; i < argc; i++) {
	if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0)
	 && i + 1 < argc) {
	    if (syseeprom_cfg_load(argv[i + 1]) != 0)
		err = 1;
	    conf_given = 1;
	} else if (strncmp(argv[i], "--config=", 9) == 0) {
	    if (syseeprom_cfg_load(argv[i] + 9) != 0)
		err = 1;
	    conf_given = 1;
	}
    }
    if (!conf_given && syseeprom_cfg_load(NULL) < 0)
	err = 1;
    if (err)
	goto syseeprom_err;

    while (TRUE) {
	c = getopt_long(argc, argv, short_options,
			long_options, &option_index);
	if (c == EOF)
	    break;

	switch (c) {
	case 'c':
	    /* handled in the pre-scan above */
	    break;

	case 'b':
	    if (syseeprom_cfg_set_backend(optarg, SE_PRIO_CLI))
		err = 1;
	    break;

	case 'f':
	    syseeprom_cfg_set_device(optarg, SE_PRIO_CLI);
	    break;

	case 'a':
	    syseeprom_cfg.addr = strtoul(optarg, NULL, 0);
	    break;

	case 'w':
	    syseeprom_cfg.addr_bits = (int) strtoul(optarg, NULL, 0);
	    break;

	case OPT_OFFSET:
	    syseeprom_cfg.offset = (int) strtoul(optarg, NULL, 0);
	    break;

	case OPT_SIZE:
	    syseeprom_cfg.size = (int) strtoul(optarg, NULL, 0);
	    break;

	case 'h':
	    xfunc_error_retval = 0;
	    cmd_usage();
	    break;

	case 'l':
	    count++;
	    show_tlv_code_list();
	    break;

	case 'e':
	    count++;
	    if (ensure_cfg()) {
		err = 1;
		goto syseeprom_err;
	    }
	    if (read_eeprom(eeprom)) {
                err = 1;
		goto syseeprom_err;
	    }
	    update_eeprom_header(eeprom);
	    update = 1;
	    break;

	case 's':
	    count++;
	    subopts = optarg;
	    if (ensure_cfg()) {
		err = 1;
		goto syseeprom_err;
	    }
	    while (*subopts != '\0' && !err) {
		if ((index = getsubopt(&subopts, tokens, &value)) != -1) {
		    if (read_eeprom(eeprom)) {
                        err = 1;
			goto syseeprom_err;
		    }
		    tcode = strtoul(tokens[index], NULL, 0);
		    for (i = 0; i < tlv_code_count; i++) {
			if (tlv_code_list[i].m_code == tcode) {
			    tname = tlv_code_list[i].m_name;
			}
		    }
		    if (!tlvinfo_multiple_tcode_allowed(tcode) || !value) {
			if (tlvinfo_delete_tlv(eeprom, tcode) == TRUE) {
			    printf("Deleting TLV 0x%x: %s\n", tcode, tname);
			}
		    }
		    if (value) {
			if (!tlvinfo_add_tlv(eeprom, tcode, value)) {
                            err = 1;
			    goto syseeprom_err;
			} else {
			    printf("Adding   TLV 0x%x: %s\n", tcode, tname);
			}
		    }
		    update = 1;
		} else {
		    err = 1;
		    printf("ERROR: Invalid option: %s\n", value);
		    goto syseeprom_err;
		}
	    }
	break;

	case 'g':
	    count++;
	    if (ensure_cfg()) {
		err = 1;
		goto syseeprom_err;
	    }
            if (read_eeprom(eeprom)) {
                err = 1;
                goto syseeprom_err;
            }
            tcode = strtoul(optarg, NULL, 0);
            if (tlvinfo_decode_tlv(eeprom, tcode, tlv_value)) {
                printf("%s\n", tlv_value);
            } else {
                err = 1;
                printf("ERROR: TLV code not present in EEPROM: 0x%02x\n", tcode);
            }
            goto syseeprom_err;
	break;

	default:
	    count++;
	    cmd_usage();
            err = 1;
	    break;
	}
    }
    if (!count) {
	if (optind < argc) {
	    cmd_usage();
            err = 1;
	} else {
	    if (ensure_cfg()) {
		err = 1;
		goto syseeprom_err;
	    }
	    if (read_eeprom(eeprom)) {
                err = 1;
                goto syseeprom_err;
            }
	    show_eeprom(eeprom);
	}
    }
    if (update) {
	if (prog_eeprom(eeprom)) {
            err = 1;
            goto syseeprom_err;
        }
	show_eeprom(eeprom);
    }
syseeprom_err:
    for (i = 0; i < tlv_code_count; i++) {
	free(tokens[i*2]);
	free(tokens[(i*2) + 1]);
    }
    return  (err == 0) ? 0 : 1;
}
