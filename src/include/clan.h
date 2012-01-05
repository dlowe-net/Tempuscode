#ifndef _CLAN_H_
#define _CLAN_H_

//
// File: clan.h                     -- Part of TempusMUD
//
// Copyright 1998 by John Watson, all rights reserved.
//

#define CLAN_NONE	0

#define MAX_CLAN_NAME      24
#define MAX_CLAN_BADGE     16
#define MAX_CLAN_PASSWORD  16
#define MAX_CLAN_MEMBERS  200
#define MAX_CLAN_ROOMS    200
#define MAX_CLAN_RANKNAME  48
#define NUM_CLAN_RANKS     11
#define LVL_CAN_CLAN       10

struct clan_data;
struct room_data;
struct room_list_elem;

bool boot_clans(void);
struct clan_data *clan_by_owner(int idnum);
struct clan_data *real_clan(int vnum);
struct clan_data *clan_by_name(char *arg);
int clan_owning_room(struct room_data *room);
bool clan_house_can_enter(struct creature *ch, struct room_data *room);
void do_show_clan(struct creature *ch, struct clan_data *clan);
bool save_clans();
struct clan_data *create_clan(int vnum);
int delete_clan(struct clan_data *clan);
struct clanmember_data *real_clanmember(long idnum, struct clan_data *clan);
void sort_clanmembers(struct clan_data *clan);
void remove_member_from_clan(struct clanmember_data *member,
                             struct clan_data *clan);
void remove_room_from_clan(struct room_list_elem *rm_list,
                           struct clan_data *clan);
void remove_clan_member(struct creature *ch);
void clear_clan_owner(long idnum);
void remove_char_clan(int clan_idnum, long ch_idnum);
int clan_id(struct clan_data *clan);

struct clanmember_data {
	long idnum;
	int8_t rank;
    bool no_mail;
	struct clanmember_data *next;
};

struct room_list_elem {
	struct room_data *room;
	struct room_list_elem *next;
};

struct clan_data {
	int number;
	money_t bank_account;
	int8_t top_rank;
	long owner;
	char *name;					/* official clan name */
	char *badge;				/* title of clan for who list, etc. */
	char *password;				/* password of clan */
	char *ranknames[NUM_CLAN_RANKS];
	struct clanmember_data *member_list;	/* list of idnums */
	struct room_list_elem *room_list;	/* list of clan house rooms */
	struct clan_data *next;
};

ACMD(do_enroll);
ACMD(do_join);
ACMD(do_dismiss);
ACMD(do_resign);
ACMD(do_promote);
ACMD(do_demote);
ACMD(do_clanmail);
ACMD(do_clanlist);
ACMD(do_cinfo);
ACMD(do_clanpasswd);
ACMD(do_cedit);

#endif
