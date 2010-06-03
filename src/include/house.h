#ifndef _HOUSE_H_
#define _HOUSE_H_

#include "defs.h"
#include "xml_utils.h"
#include "tmpstr.h"

struct creature;
struct obj_data;
struct room_data;

static inline char*
get_house_file_path( int id )
{
	return tmp_sprintf( "players/housing/%d/%04d.dat", (id % 10), id );
}

// Modes used for match_houses
enum HC_SearchModes { HC_INVALID=0, HC_OWNER, HC_LANDLORD, HC_GUEST };
enum HouseType { INVALID = 0, PRIVATE, PUBLIC, RENTAL, CLAN };

static const unsigned int MAX_HOUSE_GUESTS = 50;
static const int MAX_HOUSE_ITEMS = 50;

struct House
{
    // unique identifier of this house
    int id;
    // date this house was built
    time_t created;
    // type of ownership: personal, public, rental
    enum HouseType type;
    // idnum of house owner's account
    int ownerID;
    // The slumlord that built the house.
    long landlord;
    // the rate per date of rent charged on this house
    int rentalRate;
    // The left over gold amount from selling off items to
    // cover unpaid rent.  This is never given to the char.
    long rentOverflow;

    // idnums of house's guests
    // The list of id's of characters allowed to enter this house
    struct int_list *guests;
    // vnum house rooms
    // The list of room numbers that make up this house
    struct int_list *rooms;

    // the reposession notices created when objects are sold
    // to cover rent cost.
    struct txt_block_t *repoNotes;

};

struct HouseControl
{
		// the last time rent was paid.
		time_t lastCollection;
		int topId;
};
// The global housing project.
extern struct HouseControl Housing;

#define TOROOM(room, dir) (world[room].dir_option[dir] ? \
			    world[room].dir_option[dir]->to_room : NOWHERE)
char* print_room_contents(struct creature *ch, struct room_data *real_house_room, bool showContents);
int recurs_obj_cost(struct obj_data *obj, bool mode, struct obj_data *top_o);
int recurs_obj_contents(struct obj_data *obj, struct obj_data *top_o);
#endif
