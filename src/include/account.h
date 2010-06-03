#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <stdbool.h>

struct creature;
struct descriptor_data;
struct id_list;

enum {
    DEFAULT_TERM_HEIGHT = 22,
    DEFAULT_TERM_WIDTH = 80,
};

extern const char *ansi_levels[];
extern const char *compact_levels[];
extern int long account_top_id;
extern struct account *_cache;


struct account {
		// Internal
		int id;
		char *name;
		char *password;
		// Administration
		char *email;
		time_t creation_time;		// time account was created
		char *creation_addr;
		time_t login_time;			// last time account was logged into
		char *login_addr;
		time_t entry_time;			// last time char entered game
		// Account-wide references
		unsigned char ansi_level;
		unsigned char compact_level;
		unsigned int term_height;
		unsigned int term_width;
		// Game data
		struct id_list *chars;
		struct id_list *trusted;
        bool banned;
		int reputation;
		int quest_points;
		bool quest_banned;
		money_t bank_past;
		money_t bank_future;
};
void account_boot(void);
struct account *account_create(const char *name, struct descriptor_data *d);
struct account *account_by_name(const char *name);
struct account *account_by_id(int id);
struct account *account_by_creature(struct creature *ch);
void account_preload(const char *conditions);
bool account_exists(int accountID);
bool account_remove(struct account *acct);
size_t account_cache_size(void);

bool account_has_password(struct account *account);
bool account_authenticate(struct account *account, const char *password);
void account_login(struct account *account, struct descriptor_data *d);
void account_logout(struct account *account,
                    struct descriptor_data *d,
                    bool forced);
bool account_is_logged_in(struct account *account);
void account_initialize(struct account *account,
                        const char *name,
                        struct descriptor_data *d,
                        int idnum);
bool account_load(struct account *account, long idnum);
bool account_reload(struct account *account);

struct creature *account_create_char(struct account *account, const char *name);
void account_delete_char(struct account *account, struct creature *ch);
long account_get_char_by_index(struct account *account, int idx);
struct creature *account_get_creature_by_index(struct account *account, int idx);
bool account_invalid_char_index(struct account *account, int idx);
int account_get_char_count(struct account *account);
int account_chars_available(struct account *account);
long account_get_char(struct account *account,  int index );
bool account_deny_char_entry(struct account *account, struct creature *ch);
void account_move_char(struct account *account, long id, struct account *dest);
// Attempts to add the given orphaned character to this account
void account_exhume_char(struct account *account,  struct creature *exhumer, long id );

void set_past_bank(struct account *account, money_t amt);
void set_future_bank(struct account *account, money_t amt);
void deposit_past_bank(struct account *account, money_t amt);
void deposit_future_bank(struct account *account, money_t amt);
void withdraw_past_bank(struct account *account, money_t amt);
void withdraw_future_bank(struct account *account, money_t amt);

void set_password(struct account *account, const char *password);

inline const char* get_login_addr(struct account *account);
inline const char* get_creation_addr(struct account *account);

inline time_t get_login_time(struct account *account);
inline time_t get_creation_time(struct account *account);
inline time_t get_entry_time(struct account *account);
void update_last_entry(struct account *account);

bool isTrusted(struct account *account, long idnum);
void trust(struct account *account, long idnum);
void distrust(struct account *account, long idnum);
bool trustsNobody(struct account *account);
void displayTrusted(struct account *account, struct creature *ch);

int hasCharLevel(struct account *account, int level);
int hasCharGen(struct account *account, int level);

int countGens(struct account *account);

void load_players(struct account *account);
void load_trusted(struct account *account);
void set(struct account *account, const char *key, const char *val);
void add_trusted(struct account *account, long idnum);


#endif
