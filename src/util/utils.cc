/* ************************************************************************
*   File: utils.c                                       Part of CircleMUD *
*  Usage: various internal functions of a utility nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

//
// File: utils.c                      -- Part of TempusMUD
//
// All modifications and additions are
// Copyright 1998 by John Watson, all rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/telnet.h>
#include <netinet/in.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "screen.h"
#include "spells.h"
#include "handler.h"
#include "interpreter.h"
#include "security.h"
#include "db.h"
#include "char_class.h"
#include "tmpstr.h"

extern struct follow_type *order_next_k;
char ANSI[20];

void
safe_exit(int mode)
{
	Security::shutdown();

	touch("../pause");
	slog("Exiting with status %d from safe_exit().", mode);
	exit(mode);
}

char *
VT_GOPOS(int x, int y)
{
	sprintf(ANSI, "\x1B[%d;%dH", x, y);
	return (ANSI);
}

char *
VT_RPPOS(int x, int y)
{
	sprintf(ANSI, "\x1B[%d;%dr", x, y);
	return (ANSI);
}

void
enable_vt100(struct Creature *ch)
{
	char level[4];
	char seperator[161];
	int rows = 25, cols = 80, i = 0;

	if (GET_ROWS(ch) != -1)
		rows = GET_ROWS(ch);
	if (GET_COLS(ch) != -1)
		cols = GET_COLS(ch);

	sprintf(level, "%d", GET_LEVEL(ch));

	for (i = 0; i <= GET_COLS(ch) - 1; i++)
		seperator[i] = '-';

	seperator[GET_COLS(ch)] = '\0';

	strcpy(buf, VT_CLEAR);
	strcat(buf, VT_GOPOS(0, 0));
	strcat(buf, VT_SVPOS);
	strcat(buf, VT_GOPOS(0, 0));
	strcat(buf, seperator);
	strcat(buf, VT_GOPOS(0, 5));
	strcat(buf, "[Player: ");
	strcat(buf, GET_NAME(ch));
	strcat(buf, "   Level: ");
	strcat(buf, level);
	strcat(buf, "]");
	strcat(buf, VT_RTPOS);
	strcat(buf, VT_SVPOS);
	strcat(buf, VT_GOPOS(rows - 1, 1));
	strcat(buf, seperator);
	strcat(buf, VT_RTPOS);
	strcat(buf, VT_RPPOS(2, rows - 2));
	strcat(buf, VT_GOPOS(3, 1));
	strcat(buf, VT_SVPOS);

/*  
    send_to_char(ch, "%s%s%s%s%s%s%s%s%s%s%s%s%s", VT_CLEAR, VT_GOPOS(0,0), VT_SVPOS, VT_GOPOS(2,1), seperator,
    VT_RTPOS, VT_SVPOS, VT_GOPOS(rows-1,1), seperator,
    VT_RTPOS, VT_RPPOS(3,rows-2), VT_GOPOS(3,1), VT_SVPOS);
*/
}

void
disable_vt100(struct Creature *ch)
{
}

/* Gets number of lines in a buffer */
int
get_line_count(char *buffer)
{
	char *tmpbuf, *line, *safebuf;
	int i = 0;

	if (!buffer)
		return 0;

	safebuf = tmpbuf = strdup(buffer);
	line = strsep(&tmpbuf, "\n");
	while (line != NULL) {
		i++;
		line = strsep(&tmpbuf, "\n");
	}
	if (safebuf)
		free(safebuf);
	return (i);
}

// removes all occurances of the specified character c from char * str,
// replacing each occurance with a char c_to
int
remove_from_cstring(char *str, char c, char c_to)
{
	for (char *p = str; p && *p; ++p)
		if (*p == c)
			*p = c_to;
	return 0;
}


/* Create a duplicate of a string */
char *
str_dup(const char *source)
{
	char *new_str;

	CREATE(new_str, char, strlen(source) + 1);
	return (strcpy(new_str, source));
}



/* str_cmp: a case-insensitive version of strcmp */
/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different or end of both                 */
int
str_cmp(const char *arg1, const char *arg2)
{
	int chk, i;

	for (i = 0; *(arg1 + i) || *(arg2 + i); i++)
		if ((chk = tolower(*(arg1 + i)) - tolower(*(arg2 + i))))
			if (chk < 0)
				return (-1);
			else
				return (1);
	return (0);
}


/* strn_cmp: a case-insensitive version of strncmp */
/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different, end of both, or n reached     */
int
strn_cmp(char *arg1, char *arg2, int n)
{
	int chk, i;

	for (i = 0; (*(arg1 + i) || *(arg2 + i)) && (n > 0); i++, n--)
		if ((chk = tolower(*(arg1 + i)) - tolower(*(arg2 + i))))
			if (chk < 0)
				return (-1);
			else
				return (1);

	return (0);
}


/* log a death trap hit */
void
log_death_trap(struct Creature *ch)
{
	mudlog(LVL_AMBASSADOR, BRF, true,
		"%s hit death trap #%d (%s)", GET_NAME(ch),
		ch->in_room->number, ch->in_room->name);
}


/* writes a string to the log */
void
slog(char *str, ...)
{
	va_list args;
	time_t ct;
	char *tm_str;
	const char *msg_str;

	ct = time(0);
	tm_str = asctime(localtime(&ct));
	*(tm_str + strlen(tm_str) - 1) = '\0';
	va_start(args, str);
	msg_str = tmp_vsprintf(str, args);
	va_end(args);
	fprintf(stderr, "%-19.19s :: %s\n", tm_str, msg_str);
}


/* the "touch" command, essentially. */
int
touch(char *path)
{
	FILE *fl;

	if (!(fl = fopen(path, "a"))) {
		perror(path);
		return -1;
	} else {
		fclose(fl);
		return 0;
	}
}


/*
 * mudlog -- log mud messages to a file & to online imm's syslogs
 * based on syslog by Fen Jul 3, 1992
 */
void
mudlog(sbyte level, log_type type, bool file, const char *fmt, ...)
{
	extern struct descriptor_data *descriptor_list;
	struct descriptor_data *i;
	va_list args;
	char *msg, tp;
	char timebuf[25];
	time_t ct;
	struct tm *ctm;

	va_start(args, fmt);
	msg = tmp_vsprintf(fmt, args);
	va_end(args);

	ct = time(NULL);
	ctm = localtime(&ct);
	strftime(timebuf, 24, " - %b %d %T", ctm);

	if (file) {
		char *tm_str;

		tm_str = asctime(ctm);
		fprintf(stderr, "%-19.19s :: %s\n", tm_str, msg);
	}

	if (level < 0)
		return;


	for (i = descriptor_list; i; i = i->next)
		if (i->input_mode == CXN_PLAYING && !PLR_FLAGGED(i->creature, PLR_WRITING) &&
			!PLR_FLAGGED(i->creature, PLR_OLC)) {
			tp = ((PRF_FLAGGED(i->creature, PRF_LOG1) ? 1 : 0) +
				(PRF_FLAGGED(i->creature, PRF_LOG2) ? 2 : 0));

			if ((GET_LEVEL(i->creature) >= level) && (tp >= type)) {
				send_to_char(i->creature, "%s[ %s%s ]%s\r\n",
					CCGRN(i->creature, C_NRM),
					msg, timebuf,
					CCNRM(i->creature, C_NRM));
			}
		}
}

void
sprintbit(long vektor, const char *names[], char *result)
{
	long nr;

	*result = '\0';

	if (vektor < 0) {
		strcpy(result, "SPRINTBIT ERROR!");
		return;
	}
	for (nr = 0; vektor; vektor >>= 1) {
		if (IS_SET(1, vektor)) {
			if (*names[nr] != '\n') {
				strcat(result, names[nr]);
				strcat(result, " ");
			} else
				strcat(result, "UNDEFINED ");
		}
		if (*names[nr] != '\n')
			nr++;
	}

	if (!*result)
		strcat(result, "NOBITS ");
}



void
sprinttype(int type, const char *names[], char *result)
{
	int nr;

	for (nr = 0; (*names[nr] != '\n'); nr++);
	if (type < nr && type >= 0)
		strcpy(result, names[type]);
	else
		strcpy(result, "UNDEFINED");
}


/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct time_info_data
real_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;

	secs = (long)(t2 - t1);

	now.hours = (secs / SECS_PER_REAL_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_REAL_HOUR * now.hours;

	now.day = (secs / SECS_PER_REAL_DAY);	/* 0..34 days  */
	secs -= SECS_PER_REAL_DAY * now.day;

	now.month = -1;
	now.year = -1;

	return now;
}



/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data
mud_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;

	secs = (long)(t2 - t1);

	now.hours = (secs / SECS_PER_MUD_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_MUD_HOUR * now.hours;

	now.day = (secs / SECS_PER_MUD_DAY) % 35;	/* 0..34 days  */
	secs -= SECS_PER_MUD_DAY * now.day;

	now.month = (secs / SECS_PER_MUD_MONTH) % 17;	/* 0..16 months */
	secs -= SECS_PER_MUD_MONTH * now.month;

	now.year = (secs / SECS_PER_MUD_YEAR);	/* 0..XX? years */

	return now;
}



struct time_info_data
age(struct Creature *ch)
{
	struct time_info_data player_age;

	player_age = mud_time_passed(time(0), ch->player.time.birth);

	switch (GET_RACE(ch)) {
	case RACE_ELF:
	case RACE_DROW:
		player_age.year += 80;
		break;
	case RACE_DWARF:
		player_age.year += 40;
		break;
	case RACE_HALF_ORC:
		player_age.year += 12;
		break;
	case RACE_HUMAN:
		player_age.year += 13;
		break;
	default:
		player_age.year += 13;
		break;
	}

	return player_age;
}




/* Check if making CH follow VICTIM will create an illegal */
/* Follow "Loop/circle"                                    */
bool
circle_follow(struct Creature * ch, struct Creature * victim)
{
	struct Creature *k;

	for (k = victim; k; k = k->master) {
		if (k == ch)
			return TRUE;
	}

	return FALSE;
}



/* Called when stop following persons, or stopping charm */
/* This will NOT do if a character quits/dies!!          */
void
stop_follower(struct Creature *ch)
{
	struct follow_type *j, *k;

	if (!ch->master)
		raise(SIGSEGV);

	if (IS_AFFECTED(ch, AFF_CHARM) && !MOB2_FLAGGED(ch, MOB2_MOUNT)) {
		act("You realize that $N is a jerk!", FALSE, ch, 0, ch->master,
			TO_CHAR);
		act("$n realizes that $N is a jerk!", FALSE, ch, 0, ch->master,
			TO_NOTVICT);
		act("$n hates your guts!", FALSE, ch, 0, ch->master, TO_VICT);
		if (affected_by_spell(ch, SPELL_CHARM))
			affect_from_char(ch, SPELL_CHARM);
	} else {
		act("You stop following $N.", FALSE, ch, 0, ch->master, TO_CHAR);
		act("$n stops following $N.", TRUE, ch, 0, ch->master, TO_NOTVICT);
		if (GET_INVIS_LVL(ch) < GET_LEVEL(ch->master)
			&& !IS_AFFECTED(ch, AFF_SNEAK))
			act("$n stops following you.", TRUE, ch, 0, ch->master, TO_VICT);
	}

	if (ch->master->followers->follower == ch) {	/* Head of follower-list? */
		k = ch->master->followers;
		ch->master->followers = k->next;
		free(k);
#ifdef DMALLOC
		dmalloc_verify(0);
#endif
	} else {					/* locate follower who is not head of list */
		for (k = ch->master->followers; k->next->follower != ch; k = k->next);

		j = k->next;
		k->next = j->next;
		free(j);
#ifdef DMALLOC
		dmalloc_verify(0);
#endif
	}

	ch->master = NULL;
	REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARM | AFF_GROUP);
}



/* Called when a character that follows/is followed dies */
void
die_follower(struct Creature *ch)
{
	struct follow_type *j, *k;

	if (order_next_k && order_next_k->follower && ch == order_next_k->follower)
		order_next_k = NULL;

	if (ch->master)
		stop_follower(ch);

	for (k = ch->followers; k; k = j) {
		j = k->next;
		stop_follower(k->follower);
	}
}
int
player_in_room(struct room_data *room)
{
	CreatureList::iterator it = room->people.begin();
	for (; it != room->people.end(); ++it) {
		if (!IS_NPC((*it)) && GET_LEVEL((*it)) < LVL_AMBASSADOR)
			return 1;
	}
	return 0;
}



/* Do NOT call this before having checked if a circle of followers */
/* will arise. CH will follow leader                               */
void
add_follower(struct Creature *ch, struct Creature *leader)
{
	struct follow_type *k;

	if (ch->master)
		raise(SIGSEGV);

	ch->master = leader;

	CREATE(k, struct follow_type, 1);

	k->follower = ch;
	k->next = leader->followers;
	leader->followers = k;

	act("You now follow $N.", FALSE, ch, 0, leader, TO_CHAR);
	if (can_see_creature(leader, ch))
		act("$n starts following you.", TRUE, ch, 0, leader, TO_VICT);
	act("$n starts to follow $N.", TRUE, ch, 0, leader, TO_NOTVICT);
}

void
add_stalker(struct Creature *ch, struct Creature *leader)
{
	struct follow_type *k;

	if (ch->master)
		raise(SIGSEGV);

	ch->master = leader;

	CREATE(k, struct follow_type, 1);

	k->follower = ch;
	k->next = leader->followers;
	leader->followers = k;

	act("You are now stalking $N.", FALSE, ch, 0, leader, TO_CHAR);
	if (can_see_creature(leader, ch)) {
		if (CHECK_SKILL(ch, SKILL_STALK) < (number(0, 80) + GET_WIS(leader))) {
			act("$n starts following you.", TRUE, ch, 0, leader, TO_VICT);
			act("$n starts to follow $N.", TRUE, ch, 0, leader, TO_NOTVICT);
		} else
			gain_skill_prof(ch, SKILL_STALK);
	}
}

/*
 * get_line reads the next non-blank line off of the input stream.
 * The newline character is removed from the input.  Lines which begin
 * with '*' are considered to be comments.
 *
 * Returns the number of lines advanced in the file.
 */
int
get_line(FILE * fl, char *buf)
{
	char temp[512];
	int lines = 0;

	temp[0] = '\0';
	do {
		lines++;
		fgets(temp, 256, fl);
		temp[511] = '\0';
		if (*temp)
			temp[strlen(temp) - 1] = '\0';
	} while (!feof(fl) && (*temp == '*' || !*temp));

	if (feof(fl))
		return 0;
	else {
		strcpy(buf, temp);
		return lines;
	}
}


int
get_filename(const char *orig_name, char *filename, int mode)
{
	char *prefix, *middle, *suffix, *ptr, name[64];

	switch (mode) {
	case PLAYER_MAIL_FILE:
		prefix = "plrmail";
		suffix = "mail";
		break;
	case CRASH_FILE:
		prefix = "plrobjs";
		suffix = "objs";
		break;
	case ETEXT_FILE:
		prefix = "plrtext";
		suffix = "text";
		break;
	case IMPLANT_FILE:
		prefix = "implants";
		suffix = "implant";
		break;
	default:
		return 0;
		break;
	}

	if (!*orig_name)
		return 0;

	strcpy(name, orig_name);
	for (ptr = name; *ptr; ptr++)
		*ptr = tolower(*ptr);

	switch (tolower(*name)) {
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
		middle = "A-E";
		break;
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
		middle = "F-J";
		break;
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
		middle = "K-O";
		break;
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
		middle = "P-T";
		break;
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
		middle = "U-Z";
		break;
	default:
		middle = "ZZZ";
		break;
	}

	sprintf(filename, "%s/%s/%s.%s", prefix, middle, name, suffix);
	return 1;
}

void
num2str(char *str, int num)
{
	str[0] = 0;

	if (num == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	if (num & (1 << 0))
		strncat(str, "a", 1);

	if (num & (1 << 1))
		strncat(str, "b", 1);

	if (num & (1 << 2))
		strncat(str, "c", 1);

	if (num & (1 << 3))
		strncat(str, "d", 1);

	if (num & (1 << 4))
		strncat(str, "e", 1);

	if (num & (1 << 5))
		strncat(str, "f", 1);

	if (num & (1 << 6))
		strncat(str, "g", 1);

	if (num & (1 << 7))
		strncat(str, "h", 1);

	if (num & (1 << 8))
		strncat(str, "i", 1);

	if (num & (1 << 9))
		strncat(str, "j", 1);

	if (num & (1 << 10))
		strncat(str, "k", 1);

	if (num & (1 << 11))
		strncat(str, "l", 1);

	if (num & (1 << 12))
		strncat(str, "m", 1);

	if (num & (1 << 13))
		strncat(str, "n", 1);

	if (num & (1 << 14))
		strncat(str, "o", 1);

	if (num & (1 << 15))
		strncat(str, "p", 1);

	if (num & (1 << 16))
		strncat(str, "q", 1);

	if (num & (1 << 17))
		strncat(str, "r", 1);

	if (num & (1 << 18))
		strncat(str, "s", 1);

	if (num & (1 << 19))
		strncat(str, "t", 1);

	if (num & (1 << 20))
		strncat(str, "u", 1);

	if (num & (1 << 21))
		strncat(str, "v", 1);

	if (num & (1 << 22))
		strncat(str, "w", 1);

	if (num & (1 << 23))
		strncat(str, "x", 1);

	if (num & (1 << 24))
		strncat(str, "y", 1);

	if (num & (1 << 25))
		strncat(str, "z", 1);

	if (num & (1 << 26))
		strncat(str, "A", 1);

	if (num & (1 << 27))
		strncat(str, "B", 1);

	if (num & (1 << 28))
		strncat(str, "C", 1);

	if (num & (1 << 29))
		strncat(str, "D", 1);

	if (num & (1 << 30))
		strncat(str, "E", 1);

	if (num & (1 << 31))
		strncat(str, "F", 1);
}

char *
GET_DISGUISED_NAME(struct Creature *ch, struct Creature *tch)
{
	struct affected_type *af = NULL;
	struct Creature *mob = NULL;
	static char buf[1024];

	if (IS_NPC(tch))
		return GET_NAME(tch);

	if (!(af = affected_by_spell(tch, SKILL_DISGUISE)))
		return GET_NAME(tch);

	if (!(mob = real_mobile_proto(af->modifier)))
		return GET_NAME(tch);

	if (CAN_DETECT_DISGUISE(ch, tch, af->duration)) {
		sprintf(buf, "%s (disguised as %s)", GET_NAME(tch), GET_NAME(mob));
		return (buf);
	}
	gain_skill_prof(tch, SKILL_DISGUISE);
	return GET_NAME(mob);
}

int
CHECK_SKILL(struct Creature *ch, int i)
{
	int level = 0;
	struct affected_type *af_ptr = NULL;

	if (!IS_NPC(ch)) {
		level = ch->player_specials->saved.skills[i];
	} else {
		if (GET_CLASS(ch) < NUM_CLASSES) {
			if (GET_LEVEL(ch) >= spell_info[i].min_level[(int)GET_CLASS(ch)])
				level = 50 + GET_LEVEL(ch);
		}
		if (!level &&
			GET_REMORT_CLASS(ch) < NUM_CLASSES && GET_REMORT_CLASS(ch) >= 0) {
			if (GET_LEVEL(ch) >=
				spell_info[i].min_level[(int)GET_REMORT_CLASS(ch)])
				level = 50 + GET_LEVEL(ch);
		}
		if (!level) {
			if (IS_GIANT(ch))
				if (GET_LEVEL(ch) >= spell_info[i].min_level[CLASS_BARB])
					level = 50 + GET_LEVEL(ch);
		}
		if (IS_DEVIL(ch))
			level += GET_LEVEL(ch) >> 1;
	}
	if (level > 0 && (af_ptr = affected_by_spell(ch, SPELL_AMNESIA)))
		level = MAX(0, level - af_ptr->duration);

	return level;
}

void
WAIT_STATE(struct Creature *ch, int cycle)
{
	int wait;

	if (GET_LEVEL(ch) >= LVL_TIMEGOD)
		return;

	wait = cycle;

	if (AFF2_FLAGGED(ch, AFF2_HASTE))
		wait -= cycle >> 2;
	if (AFF2_FLAGGED(ch, AFF2_SLOW))
		wait += cycle >> 2;
	if (ch->getSpeed())
		wait -= (cycle * ch->getSpeed()) / 100;

	if (ch->desc) {
		ch->desc->wait = MAX(ch->desc->wait, wait);
	}

	else if (IS_NPC(ch)) {
		GET_MOB_WAIT(ch) = MAX(GET_MOB_WAIT(ch), wait);
	}
}

char *
OBJN(obj_data * obj, Creature * vict)
{
	if (can_see_object(vict, obj))
		return fname((obj)->name);
	else
		return "something";
}

char *
OBJS(obj_data * obj, Creature * vict)
{
	if (can_see_object((vict), (obj)))
		return obj->short_description;
	else
		return "something";
}

char *
PERS(Creature * ch, Creature * sub)
{
	if (can_see_creature(sub, ch))
		return GET_DISGUISED_NAME(sub, ch);
	else if (GET_LEVEL(ch) >= LVL_AMBASSADOR)
		return "a divine presence";
	else
		return "someone";
}

char *
AN(char *str)
{
	if (PLUR(str))
		return "some";
	if (strchr("aeiouAEIOU", *str))
		return "an";
	return "a";
}

char *
YESNO(bool a)
{
	if (a)
		return "YES";
	else
		return "NO";
}

char *
ONOFF(bool a)
{
	if (a)
		return "ON";
	return "OFF";
}

char *
CURRENCY(Creature * ch)
{
	if (ch->in_room->zone->time_frame == TIME_ELECTRO)
		return "credit";
	return "coin";
}

bool
CAN_GO(Creature * ch, int door)
{
	room_direction_data *exit = EXIT(ch, door);
	return (exit != NULL &&
		!IS_SET(exit->exit_info, EX_CLOSED | EX_NOPASS) &&
		exit->to_room != NULL);
}

bool
CAN_GO(obj_data * obj, int door)
{
	room_direction_data *exit = EXIT(obj, door);
	return (exit != NULL &&
		!IS_SET(exit->exit_info, EX_CLOSED | EX_NOPASS) &&
		exit->to_room != NULL);
}

const char *
stristr(const char *haystack, const char *needle)
{
	const char *read_pt, *search_pt;

	while (*haystack) {
		search_pt = haystack;
		read_pt = needle;
		while (tolower(*search_pt) == tolower(*read_pt) && *search_pt
			&& *read_pt) {
			search_pt++;
			read_pt++;
		}

		if (!*read_pt)
			return haystack;

		if (!*search_pt)
			return NULL;

		haystack++;
	}

	return NULL;
}
