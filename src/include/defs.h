#ifndef _DEFS_H_
#define _DEFS_H_

//
// File: defs.h                      -- Part of TempusMUD
//
// All modifications and additions are
// Copyright 1998 by John Watson, all rights reserved.
//

typedef signed char sbyte;
typedef unsigned char ubyte;
typedef signed short int sh_int;
typedef unsigned short int ush_int;
typedef char byte;

typedef long long int money_t;

typedef int room_num;
typedef int obj_num;

struct txt_block {
	char *text;
	int aliased;
	struct txt_block *next;
};

#endif
