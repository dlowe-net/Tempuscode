/* ************************************************************************
*   File: magic.c                                       Part of CircleMUD *
*  Usage: low-level functions for magic; spell template code              *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

//
// File: magic.c                      -- Part of TempusMUD
//
// All modifications and additions are
// Copyright 1998 by John Watson, all rights reserved.
//

#ifdef HAS_CONFIG_H
#include "config.h"
#endif

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "spells.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "char_class.h"
#include "flow_room.h"
#include "fight.h"
#include "obj_data.h"
#include "specs.h"

extern struct room_data *world;
extern struct obj_data *object_list;
extern struct room_direction_data *knock_door;

extern struct descriptor_data *descriptor_list;
extern struct zone_data *zone_table;

extern struct default_mobile_stats *mob_defaults;
extern char weapon_verbs[];
extern int *max_ac_applys;
extern struct apply_mod_defaults *apmd;

void weight_change_object(struct obj_data *obj, int weight);
void add_follower(struct creature *ch, struct creature *leader);
extern struct spell_info_type spell_info[];
ACMD(do_flee);
ACCMD(do_drop);
void sound_gunshots(struct room_data *rm, int type, int power, int num);
void ice_room(struct room_data *room, int amount);

/*
 * Saving throws for:
 * MCTW
 *   PARA, ROD, PETRI, BREATH, SPELL
 *     Levels 0-56
 */

const byte saving_throws[8][LVL_GRIMP + 1] = {

	/* PARA  */
	{90, 70, 69, 69, 68, 68, 67, 67, 66, 66, 65,	/* 0 - 10 */
			65, 65, 64, 64, 63, 63, 62, 62, 61, 61,	/* 11 - 20 */
			60, 60, 59, 59, 58, 58, 57, 57, 56, 56,	/* 21 - 30 */
			55, 55, 54, 54, 53, 53, 52, 52, 51, 51,	/* 31 - 40 */
			50, 50, 49, 49, 48, 48, 47, 47, 46, 46,	/* 41 - 50 */
			46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
		45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 35},
	/* ROD */
	{90, 75, 74, 73, 72, 71, 70, 69, 69, 68, 68,	/* 0 - 10 */
			67, 66, 65, 64, 63, 62, 61, 60, 59, 58,	/* 11 - 20 */
			57, 56, 55, 54, 53, 52, 51, 50, 49, 48,	/* 21 - 30 */
			47, 46, 45, 44, 43, 42, 41, 40, 39, 38,	/* 31 - 40 */
			37, 36, 35, 34, 33, 32, 31, 30, 29, 20,	/* 41 - 50 */
			20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/* PETRI  */
	{90, 70, 69, 69, 68, 68, 67, 67, 66, 66, 65,	/* 0 - 10 */
			65, 65, 64, 64, 63, 63, 62, 62, 61, 61,	/* 11 - 20 */
			60, 60, 59, 59, 58, 58, 57, 57, 56, 56,	/* 21 - 30 */
			55, 55, 54, 54, 53, 53, 52, 52, 51, 50,	/* 31 - 40 */
			47, 46, 45, 44, 43, 42, 41, 40, 39, 30,	/* 41 - 50 */
			30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	 /*BREATH*/ {90, 90, 89, 88, 87, 86, 75, 73, 71, 70, 69,	/* 0 - 10 */
			77, 76, 75, 74, 73, 72, 71, 70, 69, 68,	/* 11 - 20 */
			67, 66, 65, 64, 63, 62, 61, 60, 59, 58,	/* 21 - 30 */
			57, 56, 55, 54, 53, 52, 51, 50, 49, 48,	/* 31 - 40 */
			47, 46, 45, 44, 43, 42, 41, 40, 39, 30,	/* 41 - 50 */
			30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/* SPELL */
	{95, 90, 90, 89, 89, 89, 88, 88, 88, 87, 87,	/* 0 - 10 */
			87, 86, 86, 86, 85, 85, 85, 84, 84, 84,	/* 11 - 20 */
			83, 83, 83, 82, 82, 82, 81, 81, 81, 80,	/* 21 - 30 */
			80, 79, 79, 78, 78, 77, 77, 76, 76, 76,	/* 31 - 40 */
			75, 75, 74, 74, 73, 73, 72, 72, 71, 71,	/* 41 - 50 */
			70, 69, 68, 67, 66, 65, 63, 61, 59, 55,
		42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 34, 0},
	/* CHEM */
	{90, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79,	/* 0 - 10 */
			78, 78, 77, 77, 77, 76, 76, 75, 74, 73,	/* 11 - 20 */
			72, 71, 70, 69, 68, 67, 66, 65, 64, 63,	/* 21 - 30 */
			62, 61, 60, 59, 58, 57, 56, 55, 54, 53,	/* 31 - 40 */
			52, 51, 50, 49, 48, 47, 46, 45, 44, 43,	/* 41 - 50 */
			43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
		42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 34, 0},
	/* PSIONIC */
	{90, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79,	/* 0 - 10 */
			78, 78, 77, 77, 77, 76, 76, 75, 74, 73,	/* 11 - 20 */
			72, 71, 70, 69, 68, 67, 66, 65, 64, 63,	/* 21 - 30 */
			62, 61, 60, 59, 58, 57, 56, 55, 54, 53,	/* 31 - 40 */
			52, 51, 50, 49, 48, 47, 46, 45, 44, 43,	/* 41 - 50 */
			43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
		42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 34, 0},
	/* physic */
	{90, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79,	/* 0 - 10 */
			78, 78, 77, 77, 77, 76, 76, 75, 74, 73,	/* 11 - 20 */
			72, 71, 70, 69, 68, 67, 66, 65, 64, 63,	/* 21 - 30 */
			62, 61, 60, 59, 58, 57, 56, 55, 54, 53,	/* 31 - 40 */
			52, 51, 50, 49, 48, 47, 46, 45, 44, 43,	/* 41 - 50 */
			43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
		42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 34, 0}
};

int
mag_savingthrow(struct creature *ch, int level, int type)
{
	int save;

	if (GET_LEVEL(ch) > LVL_GOD) {
		return 1;
	}
	// If its > 100 its obviously a search and doesn't need to be saveable.
	if (level > 100) {
		return 0;
	}
	if (type == SAVING_NONE)
		return 0;

	/* negative apply_saving_throw values make saving throws better! */

	save = saving_throws[type][(int)GET_LEVEL(ch)];
	save += GET_SAVE(ch, type);
	save += (level / 2);

	if (AFF2_FLAGGED(ch, AFF2_EVADE))
		save -= (GET_LEVEL(ch) / 5);
	if (GET_POSITION(ch) < POS_FIGHTING)
		save += ((10 - GET_POSITION(ch)) * 4);

	if (ch->getSpeed())
		save -= (ch->getSpeed() / 8);

	if (GET_POSITION(ch) < POS_RESTING)
		save += 10;

	save -= (GET_REMORT_GEN(ch) * 2);

	switch (type) {
	case SAVING_PARA:
		if ((IS_CLERIC(ch) || IS_KNIGHT(ch)) && !IS_NEUTRAL(ch))
			save -= (5 + (GET_LEVEL(ch) / 16));
		if (IS_RANGER(ch))
			save -= (5 + (GET_LEVEL(ch) / 16));
		save -= (GET_CON(ch) / 8);
		break;
	case SAVING_ROD:
		save -= (AFF_FLAGGED(ch, AFF_ADRENALINE)
			|| AFF2_FLAGGED(ch, AFF2_HASTE)) ? (GET_LEVEL(ch) / 5) : 0;
		if (IS_MAGE(ch))
			save -= (4 + (GET_LEVEL(ch) / 16));
		if (IS_DWARF(ch))
			save -= (GET_CON(ch) / 4) + GET_REMORT_GEN(ch);
		if (GET_CLASS(ch) == CLASS_BARB)
			save -= (GET_LEVEL(ch) / 8) + (GET_REMORT_GEN(ch) * 2);
		else if (IS_BARB(ch))
			save -= (GET_LEVEL(ch) / 16) + GET_REMORT_GEN(ch);

		break;
	case SAVING_PETRI:
		if (IS_MONK(ch) || IS_THIEF(ch) || GET_MOVE(ch) > number(100, 400))
			save -= (5 + (GET_LEVEL(ch) / 8));
		if (IS_DWARF(ch))
			save -= (GET_CON(ch) / 4) + GET_REMORT_GEN(ch);
		if (GET_CLASS(ch) == CLASS_BARB)
			save -= (GET_LEVEL(ch) / 8) + (GET_REMORT_GEN(ch) * 2);
		else if (IS_BARB(ch))
			save -= (GET_LEVEL(ch) / 16) + GET_REMORT_GEN(ch);

		break;
	case SAVING_BREATH:
		save -= (AFF_FLAGGED(ch, AFF_ADRENALINE)
			|| AFF2_FLAGGED(ch, AFF2_HASTE)) ? (GET_LEVEL(ch) / 5) : 0;
		save -= (GET_INT(ch) / 16);
		save -= (GET_WIS(ch) / 16);
		if (IS_DWARF(ch))
			save -= (GET_CON(ch) / 4) + GET_REMORT_GEN(ch);
		if (GET_CLASS(ch) == CLASS_BARB)
			save -= (GET_LEVEL(ch) / 8) + (GET_REMORT_GEN(ch) * 2);
		else if (IS_BARB(ch))
			save -= (GET_LEVEL(ch) / 16) + GET_REMORT_GEN(ch);

		break;
	case SAVING_SPELL:
		save -= (AFF_FLAGGED(ch, AFF_ADRENALINE)
			|| AFF2_FLAGGED(ch, AFF2_HASTE)) ? (GET_LEVEL(ch) / 5) : 0;
		save -= (GET_WIS(ch) / 4);
		if (IS_DWARF(ch))
			save -= (GET_CON(ch) / 4) + GET_REMORT_GEN(ch);
		if (GET_CLASS(ch) == CLASS_BARB)
			save -= (GET_LEVEL(ch) / 8) + (GET_REMORT_GEN(ch) * 2);
		else if (IS_BARB(ch))
			save -= (GET_LEVEL(ch) / 16) + GET_REMORT_GEN(ch);
		if (IS_CYBORG(ch))
			save += 5 + (GET_LEVEL(ch) / 2);
		if (IS_DROW(ch))
			save -= (GET_LEVEL(ch) / 2);
		if (IS_NPC(ch) && GET_MOB_VNUM(ch) == 7100)	// morkoth magic resistance
			save -= GET_LEVEL(ch);
		break;
	case SAVING_CHEM:
		if (IS_CYBORG(ch))
			save -= GET_LEVEL(ch) / 2;
		if (IS_DWARF(ch))
			save -= (GET_CON(ch) / 4) + GET_REMORT_GEN(ch);
		if (GET_CLASS(ch) == CLASS_BARB)
			save -= (GET_LEVEL(ch) / 8) + (GET_REMORT_GEN(ch) * 2);
		else if (IS_BARB(ch))
			save -= (GET_LEVEL(ch) / 16) + GET_REMORT_GEN(ch);
		break;
	case SAVING_PSI:
		save -= GET_INT(ch);
		save += 15;
		break;
	case SAVING_PHY:
		if (IS_PHYSIC(ch))
			save -= (GET_LEVEL(ch) / 8) + (GET_REMORT_GEN(ch) * 2);
		break;
	default:
		errlog("unknown savetype in mag_savingthrow");
		break;
	}

	/* throwing a 0 is always a failure */
	if (MAX(1, save) < number(0, 99))
		return true;
	else
		return false;
}

int
update_iaffects(struct creature * ch)
{
	static struct affected_type *af, *next;
	for (af = ch->affected; af; af = next) {
		next = af->next;
		if (!af->is_instant)
			continue;
		af->duration--;
		if (af->duration <= 0) {
			if (affect_remove(ch, af))
				return 1;
			af = ch->affected;
		}
	}
	return 0;
}

void
obj_affect_update(void)
{
    struct creature *ch = NULL;
    extern struct obj_data *object_list;
    struct obj_data *obj;
    struct tmp_obj_affect *af, *next_af;
	int pos = -1, pos_mode = EQUIP_WORN;
    int last = 0;
	bool aff_removed = false;

    for (obj = object_list; obj != NULL; obj = obj->next) {
		for (af = obj->tmp_affects; af != NULL; af = next_af) {
			next_af = af->next;
			af->duration--;
			if (af->duration)
				continue;
			if (!aff_removed) {
				aff_removed = true;
				if (obj->worn_by) {
					// If the piece of equipment is worn, we MUST remove it
					// before we start removing affects from it, or the affects
					// won't be properly removed from the wearer.  However,
					// it needs to be in-the-game so that act() can detect
					// its visibility.  Sooooo... we stick it in the player's
					// inventory.  Then below, we remove it from inventory and
					// place it back at its proper position
					ch = obj->worn_by;
					pos_mode = EQUIP_WORN;
					pos = obj->getEquipPos();
					if (pos < 0) {
						pos_mode = EQUIP_IMPLANT;
						pos = obj->getImplantPos();
					}
					if (pos >= 0) {
						unequip_char(ch, pos, pos_mode, true);
						obj_to_char(obj, ch);
					}
				} else if (obj->carried_by) {
					ch = obj->carried_by;
				} else
					ch = NULL;
			}

			if (af->type != last) {
				// First of this type of object affect, so send an emit
				// if anybody cares
				last = af->type;
				if (ch
						&& *item_wear_off_msg[af->type]
						&& !PLR_FLAGGED(ch, PLR_WRITING | PLR_OLC | PLR_MAILING))
					act(item_wear_off_msg[af->type], false, ch, obj,
						NULL, TO_CHAR);
			}

			obj->removeAffect(af);
		}

		if (aff_removed) {
			// If an affect was removed and the object is being worn,
			// we need to put the object back on the wearer
			if (ch && pos >= 0) {
				obj_from_char(obj);
				equip_char(ch, obj, pos, pos_mode);
				ch = NULL;
				pos = -1;
			}
			aff_removed = false;
		}
    }
}

/* affect_update: called from comm.c (causes spells to wear off) */
void
affect_update(void)
{
	static struct affected_type *af, *next;
	static struct creature *i;
	int found = 0;
	char assimilate_found = 0, berserk_found = 0,
		kata_found = 0, hamstring_found = 0;
	int METABOLISM = 0;
	ACMD(do_stand);

	struct creatureList_iterator cit = characterList.begin();
	for (; cit != characterList.end(); ++cit) {
		i = *cit;

		hamstring_found = kata_found = berserk_found = assimilate_found = 0;

		if (affected_by_spell(i, SPELL_METABOLISM))
			METABOLISM = 1;
		for (af = i->affected; af && !found; af = next) {
			next = af->next;
			if (af->type == SPELL_SHIELD_OF_RIGHTEOUSNESS && !IS_GOOD(i))
				af->duration = 0;	// take it off the nogood sonuvabitch
			if (af->duration >= 1) {
				af->duration--;

				if (METABOLISM && af->duration > 1 &&
					(af->type == SPELL_POISON ||
						af->type == SPELL_SLEEP ||
						af->type == SPELL_MELATONIC_FLOOD ||
						af->type == SPELL_BREATHING_STASIS))
					af->duration--;

				if (af->duration == 1) {
					if ((af->type == SPELL_GLOWLIGHT) ||
						(af->type == SPELL_DIVINE_ILLUMINATION)
						|| (af->type == SPELL_FLUORESCE))
						send_to_char(i,
							"The light which surrounds you starts to fade.\r\n");
                    else if (af->type == ZEN_OBLIVITY) {
                        send_to_char(i,
                            "You feel your grip on the zen of oblivity slipping.\r\n");
                    }
					else if (af->type == SPELL_FLY
						|| af->type == SPELL_TIDAL_SPACEWARP) {
						send_to_char(i,
							"You feel your ability to fly fading.\r\n");
						if (IS_MOB(i)) {
							if (IS_MAGE(i) && GET_LEVEL(i) > 32
								&& GET_MANA(i) > 50)
								found = cast_spell(i, i, 0, NULL, SPELL_FLY);
							else if (IS_CLERIC(i) && GET_LEVEL(i) > 31
								&& GET_MANA(i) > 50)
								found = cast_spell(i, i, 0, NULL, SPELL_AIR_WALK);
							else if (IS_PHYSIC(i) && GET_LEVEL(i) > 31
								&& GET_MANA(i) > 50)
								found =
									cast_spell(i, i, 0, NULL, SPELL_TIDAL_SPACEWARP);
							if (!found) {

								if (!i->in_room->isOpenAir()) {
									do_stand(i, tmp_strdup(""), 0, 0, 0);
								}

								else if (EXIT(i, DOWN)
									&& EXIT(i, DOWN)->to_room != NULL) {

									if (IS_SET(EXIT(i, DOWN)->exit_info,
											EX_CLOSED)) {
										do_stand(i, tmp_strdup(""), 0, 0, 0);
									}

									else if (do_simple_move(i, DOWN, MOVE_NORM,
											1) == 2) {
										found = 1;
										break;
									}

								} else {
									act("$n grins cryptically...", false, i, 0,
										0, TO_ROOM);
								}
							}

							found = 0;
						}
					}
				}
			} else if (af->duration == -1)	/* No action */
				af->duration = -1;	/* GODs only! unlimited */
			else {
				// spell-specific messages here
				if ((af->type > 0) && (af->type <= MAX_SPELLS) &&
					!PLR_FLAGGED(i, PLR_WRITING | PLR_MAILING | PLR_OLC)) {
					if (!af->next || (af->next->type != af->type) ||
						(af->next->duration > 0)) {
						if (*spell_wear_off_msg[af->type]
							&& !PLR_FLAGGED(i, PLR_OLC)
                            && af->type != SPELL_ITEM_ATTRACTION_FIELD
                            && af->type != SPELL_ITEM_REPULSION_FIELD) {
							send_to_char(i, "%s\r\n",
                                         spell_wear_off_msg[af->type]);
						}
					}
				}
				// skill-specific messages here
				else if (af->type == SKILL_BERSERK)
					berserk_found++;
				else if (af->type == SKILL_KATA)
					kata_found++;
				else if (af->type == SKILL_ASSIMILATE)
					assimilate_found++;
				else if (af->type == SKILL_HAMSTRING)
					hamstring_found++;

				// pull the affect off
				affect_remove(i, af);
			}
		}
		if (!PLR_FLAGGED(i, PLR_WRITING | PLR_MAILING | PLR_OLC)) {
			if (assimilate_found) {
				send_to_char(i, "%d assimilation affect%s worn off.\r\n",
					assimilate_found,
					assimilate_found > 1 ? "s have" : " has");
			}
			if (berserk_found)
				send_to_char(i, "You are no longer berserk.\r\n");
			if (kata_found)
				send_to_char(i, "Your kata has worn off.\r\n");
			if (hamstring_found)
				send_to_char(i, "The wound in your leg has closed.\r\n");

		}
	}
}

/*
 *  mag_materials:
 *  Checks for up to 3 vnums (spell reagents) in the player's inventory.
 *
 * No spells implemented in Circle 3.0 use mag_materials, but you can use
 * it to implement your own spells which require ingredients (i.e., some
 * heal spell which requires a rare herb or some such.)
 */
int
mag_materials(struct creature *ch, int item0, int item1, int item2,
	int extract, int verbose)
{
	struct obj_data *tobj;
	struct obj_data *obj0 = NULL, *obj1 = NULL, *obj2 = NULL;

	for (tobj = ch->carrying; tobj; tobj = tobj->next_content) {
		if ((item0 > 0) && (GET_OBJ_VNUM(tobj) == item0)) {
			obj0 = tobj;
			item0 = -1;
		} else if ((item1 > 0) && (GET_OBJ_VNUM(tobj) == item1)) {
			obj1 = tobj;
			item1 = -1;
		} else if ((item2 > 0) && (GET_OBJ_VNUM(tobj) == item2)) {
			obj2 = tobj;
			item2 = -1;
		}
	}
	if ((item0 > 0) || (item1 > 0) || (item2 > 0)) {
		if (verbose) {
			switch (number(0, 2)) {
			case 0:
				send_to_char(ch, "A wart sprouts on your nose.\r\n");
				break;
			case 1:
				send_to_char(ch, "Your hair falls out in clumps.\r\n");
				break;
			case 2:
				send_to_char(ch, "A huge corn develops on your big toe.\r\n");
				break;
			}
		}
		return (false);
	}
	if (extract) {
		if (item0 < 0) {
			obj_from_char(obj0);
			extract_obj(obj0);
		}
		if (item1 < 0) {
			obj_from_char(obj1);
			extract_obj(obj1);
		}
		if (item2 < 0) {
			obj_from_char(obj2);
			extract_obj(obj2);
		}
	}
	if (verbose) {
		send_to_char(ch, "A puff of smoke rises from your pack.\r\n");
		act("A puff of smoke rises from $n's pack.", true, ch, NULL, NULL,
			TO_ROOM);
	}
	return (true);
}

/*
 * Every spell that does damage comes through here.  This calculates the
 * amount of damage, adds in any modifiers, determines what the saves are,
 * tests for save and calls damage().
 * return values same as damage()
 *
 */

int
mag_damage(int level, struct creature *ch, struct creature *victim,
	int spellnum, int savetype)
{
	int is_mage = 0, is_cleric = 0, is_psychic = 0, is_physic = 0,
		is_ranger = 0, is_knight = 0, audible = 0, is_bard = 0;
	int dam = 0;

	if (victim == NULL || ch == NULL)
		return 0;

	if (victim->getPosition() <= POS_DEAD) {
		errlog("vict is already dead in mag_damage.");
		return DAM_VICT_KILLED;
	}

	is_mage = (IS_MAGE(ch) || IS_VAMPIRE(ch));
	is_cleric = IS_CLERIC(ch);
	is_psychic = IS_PSIONIC(ch);
	is_physic = IS_PHYSIC(ch);
	is_ranger = IS_RANGER(ch);
	is_knight = IS_KNIGHT(ch);
	is_bard = (IS_BARD(ch));

	switch (spellnum) {
	case SPELL_HELL_FIRE_STORM:
		spellnum = SPELL_HELL_FIRE;
	case SPELL_HELL_FIRE:
		dam = dice(GET_LEVEL(ch) / 3, 4) + GET_LEVEL(ch);
		audible = true;
		break;
	case SPELL_HELL_FROST_STORM:
		spellnum = SPELL_HELL_FROST;
	case SPELL_HELL_FROST:
		dam = dice(GET_LEVEL(ch) / 3, 4) + GET_LEVEL(ch);
		audible = true;
		break;
	case SPELL_ESSENCE_OF_EVIL:
		if (IS_GOOD(victim)) {
			dam = 100 + (level * 4);
		} else
			return 0;
		break;
	case SPELL_ESSENCE_OF_GOOD:
		if (IS_EVIL(victim)) {
			dam = 100 + (level * 4);
		} else
			return 0;
		break;

		/* Mostly mages */
	case SPELL_MAGIC_MISSILE:
		dam =
			dice(1 + (level > 5) + (level > 10) + (level > 15),
			8) + (level / 4);
		break;
	case SPELL_CHILL_TOUCH:	// chill touch also has an affect
		if (is_mage)
			dam =
				dice(1 + (level > 8) + (level > 16) + (level > 24),
				8) + (level / 2);
		else
			dam = dice(1, 6) + 1;
		break;
	case SPELL_TAINT:
		if (HAS_SYMBOL(victim)) {
			return 0;
		} else {
			dam = dice(get_skill_bonus(ch, SPELL_TAINT), 15);
		}
		break;
	case SPELL_BURNING_HANDS:
		if (is_mage)
			dam = dice(level, 6) + (level / 2);
		else
			dam = dice(3, 6) + 3;
		break;
	case SPELL_SHOCKING_GRASP:
		if (is_mage)
			dam = dice(level, 4) + (level / 2);
		else
			dam = dice(5, 6) + 5;
		break;
	case SPELL_CHAIN_LIGHTNING:
		spellnum = SPELL_LIGHTNING_BOLT;
	case SPELL_LIGHTNING_BOLT:
		audible = true;
		if (is_mage)
			dam = dice(level, 10) + (level / 2);
		else
			dam = dice(7, 6) + 7;
		break;
	case SPELL_COLOR_SPRAY:
		audible = true;
		if (is_mage)
			dam = dice(level, 8) + (level / 2);
		else
			dam = dice(9, 6) + 9;
		break;
	case SPELL_FIREBALL:
		audible = true;
		if (is_mage)
			dam = dice(level, 10) + (level / 2);
		else
			dam = dice(level, 7) + 11;

        if (!CHAR_WITHSTANDS_FIRE(ch))
            victim->ignite(ch);
		break;
	case SPELL_CONE_COLD:
		audible = true;
		if (is_mage)
			dam = dice(level, 12) + (level / 2);
		else
			dam = dice(level, 8) + 16;

		ice_room(ch->in_room, level);
		break;
	case SPELL_PRISMATIC_SPRAY:
		audible = true;
		if (is_mage)
			dam = dice(level, 16) + (level / 2);
		else
			dam = dice(level, 10) + 20;
		break;
	case SPELL_METEOR_STORM:
		if (is_mage)
			dam = dice(level, 10) + (level / 2);
		else
			dam = dice(level, 8) + 20;
		break;
	case SPELL_HAILSTORM:
		dam = dice(level, 9) + 10;
		ice_room(ch->in_room, 10);
		break;
	case SPELL_ACIDITY:
		if (is_physic)
			dam = dice(3, 4) + (level / 4);
		else
			dam = dice(2, 4) + 2;
		break;
	case SPELL_GAMMA_RAY:
		if (GET_CLASS(ch) == CLASS_PHYSIC)
			dam = dice(12, level / 2);
		else
			dam = dice(12, level / 4);
		break;
	case SPELL_MICROWAVE:
		dam = dice(9, 8) + (level / 2);
		break;
	case SPELL_OXIDIZE:
		dam = dice(level, 4) + (level * 2);
		break;
	case SPELL_ENTROPY_FIELD:
		dam = 10+dice(level / 5, 3);
		if (!GET_CLASS(ch) == CLASS_PHYSIC)
			dam = dam / 2;
		break;
	case SPELL_GRAVITY_WELL:
		dam = dice(level / 2, 5);
		if (!GET_CLASS(ch) == CLASS_PHYSIC)
			dam = dam / 2;
		if (AFF3_FLAGGED(victim, AFF3_GRAVITY_WELL))
			dam = dam / 2;
		break;

	case SPELL_ELECTRIC_ARC:
		dam = dice(get_skill_bonus(ch, SPELL_ELECTRIC_ARC), 5) + (level * 8);
		break;

	case SPELL_DISRUPTION:
		dam = dice(level, 6) + (level * 2);
		break;

	case SPELL_CALL_LIGHTNING:
		audible = true;
		dam = dice(level, 7) + (level / 2);
		break;

	case SPELL_HARM:
		dam = dice(level, 6) + (level * 2);
		if (IS_GOOD(ch) && IS_GOOD(victim))
			dam /= 2;
		break;

	case SPELL_ENERGY_DRAIN:
		if (GET_LEVEL(victim) <= 2)
			dam = 100;
		else
			dam = dice(level, 10);
		break;

	case SPELL_FLAME_STRIKE:
		audible = true;
		dam = dice(level, 8) + level;
		break;

	case SPELL_SPIRIT_HAMMER:
		dam = dice(level, 4) + level;
		break;

	case SPELL_SYMBOL_OF_PAIN:
		if (HAS_SYMBOL(victim)) {
			return 0;
		} else {
			if (IS_GOOD(victim))
				dam = dice(level, 12) + (level * 5);
			else
				dam = dice(level - GET_LEVEL(victim), 7) + level;
			if (IS_EVIL(victim))
				dam /= 4;
		}
		break;
	case SPELL_ICE_STORM:
		spellnum = SPELL_ICY_BLAST;
	case SPELL_ICY_BLAST:
		audible = true;
		if (is_cleric || is_ranger || is_knight)
			dam = dice(level, 11) + (level / 4);
		else
			dam = dice(level, 7) + 16;
		ice_room(ch->in_room, level);
		break;
	case SPELL_GAS_BREATH:
		dam = dice(level, 15) + level;
		break;
	case SPELL_FIRE_BREATH:
		audible = true;
		dam = dice(level, 15) + level;
		break;
	case SPELL_FROST_BREATH:
		audible = true;
		dam = dice(level, 7) + level;
		ice_room(ch->in_room, level);
		break;
	case SPELL_ACID_BREATH:
		dam = dice(level, 15) + level;
		break;
	case SPELL_LIGHTNING_BREATH:
		audible = true;
		dam = dice(level, 15) + level;
		break;
	case SPELL_SHADOW_BREATH:
		dam = dice(level, 9) + level;
		break;
	case SPELL_STEAM_BREATH:
		dam = dice(level, 15) + level;
		break;

	case SPELL_ZHENGIS_FIST_OF_ANNIHILATION:
		dam = dice(level, 25) + level;
		break;

		/* psionic attacks */
	case SPELL_PSYCHIC_SURGE:
		if (!affected_by_spell(victim, SPELL_PSYCHIC_SURGE))
			dam = dice(3, 7) + (level * 4);
		break;

	case SPELL_EGO_WHIP:
		dam = dice(5, 9) + (level);
		break;

	case SPELL_MOTOR_SPASM:
		dam = dice(9, 5) + (level / 2);
		break;

	case SPELL_PSYCHIC_CRUSH:
		dam = dice(level, 10);
		break;

		/* Area spells */
	case SPELL_EARTHQUAKE:
		dam = dice((level /2), 14) + level * 2;
		break;

	case SPELL_FISSION_BLAST:
        dam =  dice(level, 8) + level;
		break;

	case SONG_SONIC_DISRUPTION:
		dam = dice((level / 2), 7) + (3 * level);
		break;

	case SONG_DIRGE:
		dam = dice((level / 4), (level / 4)) + (level * 4);
		break;

    case SONG_LICHS_LYRICS:
        dam = dice(level / 2, 4);
        break;

	}							/* switch(spellnum) */

	if (spellnum < MAX_SPELLS && CHECK_SKILL(ch, spellnum) >= 50) {
		// 1.2x dam for 120 skill level, fuck the LEARNED(ch) shit
        if( CHECK_SKILL(ch, spellnum) > 100 ) {
            dam += (dam * ( CHECK_SKILL(ch, spellnum) - 100 ))/100;
        }

		// int bonus for mages
		if (SPELL_IS_MAGIC(spellnum) && is_mage) {
			dam += (dam * (GET_INT(ch) - 10)) / 45;	// 1.25 dam at full int

		// wis bonus for clerics
		} else if (SPELL_IS_DIVINE(spellnum) && is_cleric) {
			dam += (dam * (GET_WIS(ch) - 10)) / 45;	// 1.25 dam at full wis
        } else if (SPELL_IS_BARD(spellnum) && is_bard) {
			dam += (dam * (GET_CHA(ch) - 10)) / 45; // 1.25 dam at full cha
            //fortissimo makes bard songs more powerful
            affected_type *af = NULL;
            if ((af = affected_by_spell(ch, SONG_FORTISSIMO))) {
                dam += (dam * af->level)/100; //up to 1.79 dam at gen 10/49
            }
		}
	}

	//
	// divine attacks get modified
	//

	if (savetype == SAVING_SPELL && SPELL_IS_DIVINE(spellnum)) {
		if (IS_GOOD(ch)) {
			dam = (int)(dam * 0.75);
		} else if (IS_EVIL(ch)) {
			dam += dam * abs(GET_ALIGNMENT(ch)) / 4000;
			if (IS_SOULLESS(ch))
				dam += (int)(dam * 0.25);

			if (IS_GOOD(victim)) {
				dam += dam * abs(GET_ALIGNMENT(victim)) / 4000;
			}
		}
	}

	if (mag_savingthrow(victim, level, savetype))
		dam /= 2;

	if (audible)
		sound_gunshots(ch->in_room, spellnum, dam, 1);

	// Do spell damage of type spellnum
	// unless its gravity well which does pressure damage.
	if (spellnum == SPELL_GRAVITY_WELL) {
		int retval = damage(ch, victim, dam, TYPE_PRESSURE, WEAR_RANDOM);
		if (retval) {
			return retval;
		}
		WAIT_STATE(victim, 2 RL_SEC);
		if (!AFF3_FLAGGED(victim, AFF3_GRAVITY_WELL) &&
			(victim->getPosition() > POS_STANDING
				|| number(1, level / 2) > GET_STR(victim))) {
			victim->setPosition(POS_RESTING);
			act("The gravity around you suddenly increases, slamming you to the ground!", false, victim, 0, ch, TO_CHAR);
			act("The gravity around $n suddenly increases, slamming $m to the ground!", true, victim, 0, ch, TO_ROOM);
		}
	} else { // normal spell damage type
		int retval = damage(ch, victim, dam, spellnum, WEAR_RANDOM);
		if (retval) {
			return retval;
		}
	}
	if (spellnum == SPELL_PSYCHIC_SURGE) {
		if (!affected_by_spell(victim, SPELL_PSYCHIC_SURGE) &&
				!mag_savingthrow(victim, level, SAVING_PSI) &&
				(!IS_NPC(victim) || !MOB2_FLAGGED(victim, MOB2_NOSTUN)) &&
				victim->getPosition() > POS_STUNNED) {
			affected_type af;

            victim->removeCombat(ch);
            ch->removeCombat(victim);
			victim->setPosition(POS_STUNNED);
			WAIT_STATE(victim, 5 RL_SEC);
			memset(&af, 0, sizeof(af));
			af.type = SPELL_PSYCHIC_SURGE;
			af.duration = 1;
			af.level = level;
            af.owner = GET_IDNUM(ch);
			affect_to_char(victim, &af);
		}
	} else if (spellnum == SPELL_PSIONIC_SHOCKWAVE && victim->getPosition() > POS_SITTING) {
		if (number(5, 25) > GET_DEX(victim) / 3) {
			act("$N is knocked to the ground by your psionic shockwave!",
				false, ch, 0, victim, TO_CHAR);
			if (IS_PSIONIC(victim))
				act("You are knocked to the ground by $n's psionic shockwave!",
					false, ch, 0, victim, TO_VICT);
			else
				act("Your head suddenly explodes in pain and you fall to the ground in agony!",
					false, ch, 0, victim, TO_VICT);
			act("$N suddenly falls to the ground, clutching $S head!",
				false, ch, 0, victim, TO_ROOM);
			victim->setPosition(POS_SITTING);
			WAIT_STATE(victim, 2 RL_SEC);
		}
	} else if (spellnum == SPELL_EGO_WHIP
		&& victim->getPosition() > POS_SITTING) {
		if (number(5, 25) > GET_DEX(victim)) {
			act("You are knocked to the ground by the psychic attack!",
				false, victim, 0, 0, TO_CHAR);
			act("$n is knocked to the ground by the psychic attack!",
				false, victim, 0, 0, TO_ROOM);
			victim->setPosition(POS_SITTING);
			WAIT_STATE(victim, 2 RL_SEC);
		}
	} else if (spellnum == SPELL_CONE_COLD || spellnum == SPELL_HAILSTORM ||
			spellnum == SPELL_HELL_FROST) {
		if (AFF2_FLAGGED(victim, AFF2_ABLAZE)) {
            victim->extinguish();
			act("The flames on your body sizzle out and die.",
				true, victim, 0, 0, TO_CHAR);
			act("The flames on $n's body sizzle out and die.",
				true, victim, 0, 0, TO_ROOM);
		}
	}

	return 0;
}

/*
  Every spell that does an affect comes through here.  This determines
  the effect, whether it is added or replacement, whether it is legal or
  not, etc.

  affect_join(vict, aff, add_dur, avg_dur, add_mod, avg_mod)
*/

void
mag_affects(int level,
            struct creature *ch,
            struct creature *victim,
            int *dir __attribute__ ((unused)),
            int spellnum,
            int savetype)
{

	struct affected_type af, af2, *afp;
	struct affected_type aff_array[8];
	int is_mage = false;
	int is_cleric = false;
	int is_psychic = false;
	int is_physic = false;
	int accum_affect = false;
	int accum_duration = false;
	const char *to_vict = NULL;
	const char *to_room = NULL;

	if (victim == NULL || ch == NULL)
		return;

	if( IS_PC(ch) && GET_REMORT_GEN(ch) > 0 &&
		GET_CLASS(ch) >= 0 && GET_CLASS(ch) < NUM_CLASSES ) {
		level +=  spell_info[spellnum].gen[GET_CLASS(ch)] * 2;
	}

	if (spell_info[spellnum].violent
		&& mag_savingthrow(victim, level, savetype)) {
		send_to_char(ch, "%s", NOEFFECT);
		if (ch != victim)
			send_to_char(victim, "%s", NOEFFECT);
		return;
	}

	is_mage = (IS_MAGE(ch) || IS_VAMPIRE(ch));
	is_cleric = (IS_CLERIC(ch));
	is_psychic = (IS_PSYCHIC(ch));
	is_physic = (IS_PHYSIC(ch));

    // This is dumb.  One day when one of us
    // has some time we should completely remove
    // af and af2 from this function and convert
    // everything to aff_array
    memset(&af, 0x0, sizeof(affected_type));
    memset(&af2, 0x0, sizeof(affected_type));
	af.type = af2.type = spellnum;
	af.location = af2.location = APPLY_NONE;
	af.level = af2.level = level;
    af.owner = af2.owner = GET_IDNUM(ch);

    for (int i = 0; i < 8; i++) {
        memset(&aff_array[i], 0x0, sizeof(affected_type));
        aff_array[i].type = spellnum;
        aff_array[i].location = APPLY_NONE;
        aff_array[i].level = level;
        aff_array[i].owner = GET_IDNUM(ch);
    }
	switch (spellnum) {

	case SPELL_CHILL_TOUCH:
	case SPELL_CONE_COLD:
		af.location = APPLY_STR;
		if (mag_savingthrow(victim, level, savetype))
			af.duration = 1;
		else
			af.duration = 4;
		af.modifier = -((level / 16) + 1);
		accum_duration = true;
		to_vict = "You feel your strength wither!";
		break;
	case SPELL_HELL_FROST_STORM:
		spellnum = SPELL_HELL_FROST;
	case SPELL_HELL_FROST:
		af.location = APPLY_STR;
		if (mag_savingthrow(victim, level, savetype))
			af.duration = 1;
		else
			af.duration = 4;
		af.modifier = -((level / 16) + 1);
		accum_duration = true;
		to_vict = "You feel your strength withered by the cold!";
		break;

	case SPELL_TROG_STENCH:
		af.location = APPLY_STR;
		if (mag_savingthrow(victim, level, savetype)) {
			af.duration = 1;
			af.modifier = -(number(0, 3));
			to_vict = "You feel your strength wither!";
		} else {
			af.duration = 4;
			af.modifier = -(number(1, 8));
			to_vict =
				"You feel your strength wither as you vomit on the floor!";
			if (!number(0, 1))
				to_room = "$n vomits all over the place!";
			else if (!number(0, 1))
				to_room = "$n pukes all over $mself!";
			else if (!number(0, 1))
				to_room = "$n blows chunks all over you!";
			else
				to_room = "$n starts vomiting uncontrollably!";
		}
		accum_affect = false;
		accum_duration = true;
		break;
	case SPELL_ARMOR:
		af.location = APPLY_AC;
		af.duration = 24;
		af.modifier = -((level / 4) + 20);
		accum_duration = true;
		to_vict = "You feel someone protecting you.";
		break;

	case SPELL_BARKSKIN:
		if (affected_by_spell(victim, SPELL_STONESKIN)) {
			affect_from_char(victim, SPELL_STONESKIN);
			if (*spell_wear_off_msg[SPELL_STONESKIN]) {
				send_to_char(victim, "%s\r\n",
                             spell_wear_off_msg[SPELL_STONESKIN]);
			}
		}

		af.location = APPLY_AC;
		af.duration = dice(4, (level / 8) + 1);
		af.modifier = -10;
		af.level = get_skill_bonus(ch, SPELL_BARKSKIN);
		accum_duration = true;
		to_vict = "Your skin tightens up and hardens.";
		break;

    case SPELL_THORN_SKIN:
        af.location = APPLY_AC;
        af.duration = dice(3, (level / 4) + 1);
        af.modifier = -(get_skill_bonus(ch, SPELL_THORN_SKIN) / 10 + 5);
        break;
	case SPELL_STONESKIN:
		if (affected_by_spell(victim, SPELL_BARKSKIN)) {
			affect_from_char(victim, SPELL_BARKSKIN);
			if (*spell_wear_off_msg[SPELL_BARKSKIN]) {
				send_to_char(victim, "%s\r\n",
                             spell_wear_off_msg[SPELL_BARKSKIN]);
			}
		}
		af.level = af2.level = get_skill_bonus(ch, SPELL_STONESKIN);
		af2.location = APPLY_DEX;
		af.location = APPLY_AC;
		af.duration = af2.duration = dice(4, (level / 8) + 1);
		af.modifier = -20;
		af2.modifier = -2;
		accum_duration = true;
		to_vict = "Your skin hardens to a rock-like shell.";
		to_room = "$n's skin turns a pale, rough grey.";
		break;
	case SPELL_PRAY:
		af.location = APPLY_HITROLL;
		af.modifier = 3 + (level / 8);
		af.duration = 4 + (level / 16);
		af2.location = APPLY_SAVING_SPELL;
		af2.modifier = -(3 + (level / 16));
		af2.duration = af.duration;
		accum_duration = true;
        if (IS_GOOD(ch))
            to_vict = "You feel extremely righteous.";
        else
            to_vict = "You feel a dark power enter your soul.";
		break;
	case SPELL_BLINDNESS:
		if (MOB_FLAGGED(victim, MOB_NOBLIND)) {
			send_to_char(ch, "You fail.\r\n");
			return;
		}
        if (ch->checkReputations(victim))
            return;

		af.location = APPLY_HITROLL;
		af.modifier = -4;
		af.duration = 2;
		af.bitvector = AFF_BLIND;
		af2.location = APPLY_AC;
		af2.modifier = 40;
		af2.duration = 2;
		af2.bitvector = AFF_BLIND;
		to_room = "$n seems to be blinded!";
		to_vict = "You have been blinded!";
		break;
	case SPELL_BREATHE_WATER:
		af.bitvector = AFF_WATERBREATH;
		af.duration = 10 + level;
		to_vict = "You are now able to breathe underwater.";
		break;
	case SPELL_SPIRIT_TRACK:
		af.duration = level;
		to_vict = "You can now sense trails to other creatures.";
		break;
	case SPELL_WORD_STUN:
		af.location = APPLY_INT;
		af.duration = 1;
		af.modifier = -1;
		af.bitvector = 0;
		if (MOB2_FLAGGED(victim, MOB2_NOSTUN)) {
			send_to_char(ch, "You fail the stun.\r\n");
			hit(victim, ch, TYPE_UNDEFINED);
			return;
		}
        if (ch->checkReputations(victim))
            return;

        victim->removeCombat(ch);
        ch->removeCombat(victim);
		victim->setPosition(POS_STUNNED);
		WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
		to_room = "$n suddenly looks stunned!";
		to_vict = "You have been stunned!";
		break;
	case SPELL_BLUR:
		af.location = APPLY_AC;
		af.duration = 2 + (level / 4);
		af.modifier = -10;
		af.bitvector = AFF_BLUR;
		accum_duration = true;
		to_vict = "Your image suddenly starts to blur and shift.";
		to_room = "The image of $n suddenly starts to blur and shift.";
		break;
	case SPELL_CURSE:
		af.location = APPLY_HITROLL;
		af.duration = 1 + (level / 2);
		af.modifier = -(1 + level / 8);
		af.bitvector = AFF_CURSE;
		af2.location = APPLY_DAMROLL;
		af2.duration = 1 + (level / 2);
		af2.modifier = -(1 + level / 10);
		af2.bitvector = AFF_CURSE;
		accum_duration = true;
		accum_affect = false;
		to_room = "$n briefly glows with a sick red light!";
		to_vict = "You feel very uncomfortable";
		break;
	case SPELL_DETECT_ALIGN:
		af.duration = 12 + level;
		af.bitvector = AFF_DETECT_ALIGN;
		accum_duration = true;
		to_vict = "Your eyes tingle.";
		break;
	case SPELL_DETECT_INVIS:
		af.duration = 12 + level;
		af.bitvector = AFF_DETECT_INVIS;
		accum_duration = false;
		to_vict = "Your eyes tingle.";
		break;
	case SPELL_DETECT_MAGIC:
		af.duration = 12 + level;
		af.bitvector = AFF_DETECT_MAGIC;
		accum_duration = false;
		to_vict = "Your eyes tingle.";
		break;
	case SPELL_DETECT_POISON:
        af.duration = 12 + level;
        af.bitvector = AFF3_DETECT_POISON;
        af.aff_index = 3;
        to_vict = "Your eyes tingle.";
        if (AFF_FLAGGED(victim, AFF_POISON))
            send_to_char(victim, "You can sense poison in your blood.\r\n");
		break;
	case SPELL_DETECT_SCRYING:
		to_vict = "You are now aware.";
		af.duration = level;
		accum_duration = true;
		break;
	case SPELL_DISPLACEMENT:
		af.duration = 4 + 2 * (level > 48);
		af.bitvector = AFF2_DISPLACEMENT;
		af.aff_index = 2;
		accum_duration = false;
		to_vict = "Your image will now be displaced from its actual position.";
		break;
	case SPELL_ENDURE_COLD:
		af.duration = 24;
		af.bitvector = AFF2_ENDURE_COLD;
		af.aff_index = 2;
		accum_duration = true;
		to_vict = "You can now endure the coldest of cold.";
		break;
	case SPELL_FIRE_SHIELD:
		af.duration = 6 + (level / 8);
		af.bitvector = AFF2_FIRE_SHIELD;
		af.aff_index = 2;
		af.location = APPLY_AC;
		af.modifier = -8;
		accum_duration = true;
		to_vict = "A sheet of flame appears before your body.";
		to_room = "A sheet of flame appears before $n!";
		break;
	case SPELL_AIR_WALK:
	case SPELL_FLY:
		if (victim->getPosition() <= POS_SLEEPING) {
			act("$E is in no position to be flying!", false, ch, 0, victim,
				TO_CHAR);
			return;
		}
		af.duration = 2 + (level / 8);
		af.bitvector = AFF_INFLIGHT;
		accum_duration = true;
		to_vict = "Your feet lift lightly from the ground.";
		to_room = "$n begins to hover above the ground.";
		victim->setPosition(POS_FLYING);
		break;
	case SPELL_HASTE:
		af.duration = (level / 4);
		af.bitvector = AFF2_HASTE;
		af.aff_index = 2;
		accum_duration = false;
		to_vict = "You start moving FAST.";
		break;
	case SPELL_INFRAVISION:
		af.duration = 12 + (level / 4);
		af.bitvector = AFF_INFRAVISION;
		accum_duration = true;
		to_vict = "Your eyes glow red.";
		to_room = "$n's eyes glow red.";
		break;
	case SPELL_DIVINE_ILLUMINATION:
		af.duration = 8 + level;
		af.bitvector = AFF2_DIVINE_ILLUMINATION;
		af.aff_index = 2;
		accum_duration = true;
		if (IS_GOOD(ch)) {
			to_vict = "You are surrounded with a soft holy light.";
			to_room = "$n is surrounded by a soft, holy light.";
		} else if (IS_EVIL(ch)) {
			to_vict = "You are surrounded with an unholy light.";
			to_room = "$n is surrounded by an unholy light.";
		} else {
			to_vict = "You are surrounded with a sickly light.";
			to_room = "$n is surrounded by a sickly light.";
		}
		break;
	case SPELL_GLOWLIGHT:
		af.duration = 8 + level;
		af.bitvector = AFF_GLOWLIGHT;
		accum_duration = true;
		to_vict = "The area around you is illuminated with ghostly light.";
		to_room = "A ghostly light appears around $n.";
		break;
	case SPELL_INVISIBLE:
		if (!victim)
			victim = ch;
		af.duration = 12 + (level / 4);
		af.modifier = -20;
		af.location = APPLY_AC;
		af.bitvector = AFF_INVISIBLE;
		accum_duration = true;
		to_vict = "You vanish.";
		to_room = "$n slowly fades out of existence.";
		break;
	case SPELL_GREATER_INVIS:
		af.duration = 3 + (level / 8);
		if (!AFF_FLAGGED(victim, AFF_INVISIBLE)) {
			af.modifier = -20;
			af.location = APPLY_AC;
		} else {
			af.modifier = -4;
			af.location = APPLY_AC;
		}
		af.bitvector = AFF_INVISIBLE;
		accum_duration = false;
		to_vict = "You vanish.";
		to_room = "$n slowly fades out of existence.";
		break;
	case SPELL_INVIS_TO_UNDEAD:
		af.duration = 12 + (level / 4);
		af.bitvector = AFF2_INVIS_TO_UNDEAD;
		af.aff_index = 2;
		accum_duration = true;
		to_vict = "The undead can no longer see you.";
		break;
	case SPELL_ANIMAL_KIN:
		af.duration = 12 + (level / 4);
		af.bitvector = AFF2_ANIMAL_KIN;
		af.aff_index = 2;
		accum_duration = true;
		to_vict = "You feel a strong kinship with animals.";
		break;
	case SPELL_MAGICAL_PROT:
		af.duration = 3 + (level / 4);
		af.location = APPLY_SAVING_SPELL;
		af.modifier = -(level / 8) + 1;
		accum_duration = false;
		to_vict =
			"You are now protected somewhat against the forces of magic.";
		to_room =
			"A shimmering aura appears around $n's body, then dissipates.";
		break;
	case SPELL_PETRIFY:
		af.duration = level;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF2_PETRIFIED;
		af.aff_index = 2;
		to_vict = "You feel petrified as your body TURNS TO STONE!";
		to_room = "$n suddenly turns to stone and stops in $s tracks!";
		accum_duration = false;
		accum_affect = false;
		break;
	case SPELL_GAS_BREATH:
		if (!NEEDS_TO_BREATHE(victim))
			return;
		af.type = SPELL_POISON;
		af.location = APPLY_STR;
		af.duration = level;
		af.modifier = -2;
		accum_duration = true;

		if (level > 40 + number(0, 8)) {
			af.bitvector = AFF3_POISON_3;
			af.aff_index = 3;
		} else if (level > 30 + number(0, 9)) {
			af.bitvector = AFF3_POISON_2;
			af.aff_index = 3;
		} else {
			af.bitvector = AFF_POISON;
			af.aff_index = 1;
		}

		to_vict = "You inhale the vapors and get violently sick!";
		to_room = "$n gets violently ill from inhaling the vapors!";
		break;
	case SPELL_POISON:
		if (IS_UNDEAD(victim))
			return;
		af.location = APPLY_STR;
		af.duration = level;
		af.modifier = -2;
		accum_duration = true;

		if (level > 40 + number(0, 8)) {
			af.bitvector = AFF3_POISON_3;
			af.aff_index = 3;
		} else if (level > 30 + number(0, 9)) {
			af.bitvector = AFF3_POISON_2;
			af.aff_index = 3;
		} else {
			af.bitvector = AFF_POISON;
			af.aff_index = 1;
		}

		to_vict = "You feel very sick!";
		to_room = "$n gets violently ill!";
		break;
	case SPELL_PRISMATIC_SPHERE:
		af.type = SPELL_PRISMATIC_SPHERE;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.duration = 2 + GET_INT(ch);
		af.bitvector = AFF3_PRISMATIC_SPHERE;
		af.aff_index = 3;
		accum_affect = false;
		accum_duration = false;
		to_room = "A prismatic sphere of light appears, surrounding $n!";
		to_vict = "A prismatic sphere of light appears around you!";
		break;

	case SPELL_SICKNESS:
		if (IS_SICK(victim))
			return;
		af.type = SPELL_SICKNESS;
		af2.type = SPELL_SICKNESS;
		af.location = APPLY_HITROLL;
		af.duration = dice(level, 8) - (GET_CON(victim) * 2);
		af.modifier = -(level / 5);
		af.bitvector = AFF3_SICKNESS;
		af.aff_index = 3;
		af2.bitvector = 0;
		af2.location = APPLY_DAMROLL;
		af2.modifier = af.modifier;
		af2.duration = af.duration;
		break;

	case SPELL_SHROUD_OBSCUREMENT:
		af.duration = 10 + (level / 2);
		af.bitvector = AFF3_SHROUD_OBSCUREMENT;
		af.aff_index = 3;
		to_vict = "An obscuring shroud forms in the space around you.";
		to_room = "An obscuring shroud forms around $n.";
		break;

	case SPELL_SLOW:
		if (mag_savingthrow(victim, level, SAVING_SPELL)) {
			send_to_char(ch, "%s", NOEFFECT);
			return;
		}

        if (ch->checkReputations(victim))
            return;

		af.duration = 1 + (level / 4);
		af.bitvector = AFF2_SLOW;
		af.aff_index = 2;
		af.location = APPLY_DEX;
		af.modifier = -number(0, (level / 16));
		to_vict = "Your movements slow to a tortured crawl.";
		break;

	case SPELL_PROT_FROM_EVIL:
		if (IS_EVIL(victim)) {
            if (ch->checkReputations(victim))
                return;

			to_vict = "You feel terrible!";
			af.bitvector = 0;

			switch (number(0, 5)) {
			case 0:
				af.location = APPLY_STR;
				af.modifier = -(level / 16);
				break;
			case 1:
				af.location = APPLY_INT;
				af.modifier = -(level / 16);
				break;
			case 2:
				af.location = APPLY_CHA;
				af.modifier = -(level / 16);
				break;
			case 3:
				af.location = APPLY_CON;
				af.modifier = -(level / 8);
				break;
			case 4:
				af.location = APPLY_HITROLL;
				af.modifier = -(level / 8);
				break;
			case 5:
				af.location = APPLY_DAMROLL;
				af.modifier = -(level / 8);
				break;
			}
		} else {
			af.bitvector = AFF_PROTECT_EVIL;
			to_vict = "You feel invulnerable against the forces of evil!";
		}
		af.duration = 12;
		accum_duration = true;
		break;

	case SPELL_PROT_FROM_GOOD:
		if (IS_GOOD(victim)) {
            if (ch->checkReputations(victim))
                return;

			to_vict = "You feel terrible!";
			af.bitvector = 0;
			switch (number(0, 5)) {
			case 0:
				af.location = APPLY_STR;
				af.modifier = -(level / 16);
				break;
			case 1:
				af.location = APPLY_INT;
				af.modifier = -(level / 16);
				break;
			case 2:
				af.location = APPLY_CHA;
				af.modifier = -(level / 16);
				break;
			case 3:
				af.location = APPLY_CON;
				af.modifier = -(level / 8);
				break;
			case 4:
				af.location = APPLY_HITROLL;
				af.modifier = -(level / 8);
				break;
			case 5:
				af.location = APPLY_DAMROLL;
				af.modifier = -(level / 8);
				break;
			}
		} else {
			af.bitvector = AFF_PROTECT_GOOD;
			to_vict = "You feel invulnerable against the forces of good!";
		}
		af.duration = 12;
		accum_duration = true;
		break;
	case SPELL_PROTECT_FROM_DEVILS:
		af.duration = 12 + (level / 8);
		af.bitvector = AFF2_PROT_DEVILS;
		af.aff_index = 2;
		accum_duration = false;
		to_vict = "The devilish races will have difficulty harming you.";
		break;
	case SPELL_PROT_FROM_LIGHTNING:
		af.duration = 12 + (level / 4);
		af.bitvector = AFF2_PROT_LIGHTNING;
		af.aff_index = 2;
		accum_duration = true;
		to_vict = "You feel like standing on a hill holding a flagpole!";
		break;

	case SPELL_PROT_FROM_FIRE:
		af.duration = 12 + (level / 4);
		af.bitvector = AFF2_PROT_FIRE;
		af.aff_index = 2;
		accum_duration = true;
		to_vict = "You feel like joining the local volunteer fire department!";
		break;

	case SPELL_REGENERATE:
		af.duration = (GET_LEVEL(ch) / 10 + GET_REMORT_GEN(ch) / 2);
		af.bitvector = AFF_REGEN;
		accum_duration = true;
		to_vict = "Your body begins to regenerate at an accelerated rate.";
		break;

	case SPELL_REJUVENATE:
		af.duration = 3;
		af.bitvector = AFF_REJUV;
		accum_duration = true;
		to_vict = "You will heal faster while sleeping.";
		break;
	case SPELL_UNDEAD_PROT:
		af.duration = 12 + (level / 8);
		af.bitvector = AFF2_PROTECT_UNDEAD;
		af.aff_index = 2;
		accum_duration = false;
		to_vict = "The undead are toast.  You're bad.";
		break;
	case SPELL_SANCTUARY:
		af.duration = 4;
		af.bitvector = AFF_SANCTUARY;

		accum_duration = false;
        if (IS_EVIL(ch)) {
            to_vict = "A cloud of darkness surrounds you!";
            to_room = "$n is surrounded by a cloud of darkness.";
        } else {
            to_vict = "A white aura momentarily surrounds you!";
            to_room = "$n is surrounded by a white aura.";
        }
		break;

	case SPELL_SLEEP:
	case SPELL_MELATONIC_FLOOD:

		if (MOB_FLAGGED(victim, MOB_NOSLEEP) || IS_UNDEAD(victim))
			return;

        if (ch->checkReputations(victim))
            return;

		af.duration = 4 + level / 10;
		af.bitvector = AFF_SLEEP;

		if (victim->getPosition() > POS_SLEEPING) {
			act("You feel very sleepy...ZZzzzz...", false, victim, 0, 0,
				TO_CHAR);
			act("$n goes to sleep.", true, victim, 0, 0, TO_ROOM);
			victim->setPosition(POS_SLEEPING);
		}
		break;

	case SPELL_STRENGTH:
		af.location = APPLY_STR;
		af.duration = (level / 2) + 4;
		af.modifier = 1 + (number(0, (level / 8)));
		accum_duration = true;
		accum_affect = false;
		to_vict = "You feel stronger!";
		break;
	case SPELL_WORD_OF_INTELLECT:
		af.location = APPLY_INT;
		af.duration = (level / 2) + 4;
		af.modifier = 1 + (level > 18);
		accum_duration = false;
		accum_affect = false;

		if AFF_FLAGGED
			(victim, AFF_CONFUSION) {
			REMOVE_BIT(AFF_FLAGS(victim), AFF_CONFUSION);
			}

		to_vict = "Your mental faculties improve!";
		break;

	case SPELL_SENSE_LIFE:
		to_vict = "You feel your awareness improve.";
		af.duration = 1;		//level
		af.bitvector = AFF_SENSE_LIFE;
		accum_duration = true;
		break;

	case SPELL_TELEKINESIS:
		to_vict = "You feel able to carry a greater load.";
		af.duration = level / 2;
		af.bitvector = AFF2_TELEKINESIS;
		af.aff_index = 2;
		accum_duration = true;
		break;

	case SPELL_TRUE_SEEING:
		af.duration = 1 + (level / 4);
		af.bitvector = AFF2_TRUE_SEEING;
		af.aff_index = 2;

		accum_duration = true;
		to_room = "$n's eyes open wide.";
		to_vict = "You can now see things as they really are.";
		break;

	case SPELL_WATERWALK:
		af.duration = 24;
		af.bitvector = AFF_WATERWALK;
		af.aff_index = 1;
		accum_duration = true;
		to_vict = "You feel webbing between your toes.";
		break;

	case SPELL_MANA_SHIELD:
		af.duration = -1;
		to_vict = "Your mana will now absorb a percentage of damage.";
		break;

	/** psionic skills here **/
	case SPELL_POWER:
		af.duration = (level / 4) + 4;
		af.modifier = 1 + dice(1, (level / 16));
		af.location = APPLY_STR;
		to_vict = "A psychic finger on your brain makes you feel stronger!";
		break;

	case SPELL_WEAKNESS:
		af.duration = (level / 4) + 4;
		af.modifier = -(1 + dice(1, (level / 16)));
		af.location = APPLY_STR;
		to_vict = "A psychic finger on your brain makes you feel weaker!";
		break;

	case SPELL_CLUMSINESS:
		af.duration = (level / 4) + 4;
		af.modifier = -(1 + dice(1, (level / 8)));
		af.location = APPLY_DEX;
		to_vict = "A psychic finger on your brain makes you feel less agile!";
		break;

	case SPELL_INTELLECT:
		af.duration = (level / 4) + 4;
		af.modifier = 1 + dice(1, (level / 16));
		af.location = APPLY_INT;
		to_vict = "Your mental faculties improve!";
		break;

	case SPELL_CONFUSION:
		af.duration = 1 + (level / 4);
		af.modifier = -(1 + (level / 7));
		af.location = APPLY_HITROLL;
		af.bitvector = AFF_CONFUSION;

		if (victim->getPosition() > POS_SLEEPING)
			to_room = "$n stops in $s tracks and stares off into space.";
		WAIT_STATE(victim, PULSE_VIOLENCE * 2);
		to_vict = "You suddenly feel very confused!";
		break;

	case SPELL_ENDURANCE:
		af.duration = 1 + (level / 4);
		af.modifier = 10 + (level * 2);
		af.location = APPLY_MOVE;
		to_vict = "You feel your energy capacity rise.";
		accum_duration = 1;
		break;

	case SPELL_FEAR:
        if (ch->checkReputations(victim))
            return;

		if (IS_UNDEAD(victim) || IS_DRAGON(victim) || IS_DEVIL(victim)) {
			act("You fail to affect $N!", false, ch, 0, victim, TO_CHAR);
			send_to_char(ch, "You feel a wave of fear pass over you!\r\n");
			return;
		}
		af.duration = 1 + (level / 16);
		accum_affect = 1;
		accum_duration = 1;
		to_vict = "You suddenly feel very afraid!";
		to_room = "$n looks very afraid!";
		break;

	case SPELL_TELEPATHY:
		af.duration = 1 + (level / 16);
		to_vict = "Your telepathic senses are greatly heightened.";
		break;

	case SPELL_CONFIDENCE:
		af.modifier = dice(2, (level / 8) + 1);
		af.duration = 3 + (level / 4);
		af.location = APPLY_HITROLL;
		af.bitvector = AFF_CONFIDENCE;

		af2.location = APPLY_SAVING_SPELL;
		af2.modifier = -dice(1, (level / 8) + 1);
		af2.duration = af.duration;
		accum_duration = 1;
		to_vict = "You suddenly feel very confident!";
		break;

	case SPELL_NOPAIN:
		to_room = "$n ripples $s muscles and grins insanely!";
		to_vict = "You feel like you can take anything!";
		af.duration = 1 + dice(3, (level / 16) + 1);
		af.bitvector = AFF_NOPAIN;
		break;

	case SPELL_RETINA:
		af.duration = 12 + (level / 2);
		af.bitvector = AFF_RETINA;
		to_vict = "The rods of your retina are stimulated!";
		to_room = "$n's eyes shine brightly.";
		break;

	case SPELL_ADRENALINE:
		af.modifier = dice(1, (level / 8) + 1);
		af.duration = 3 + (level / 8);
		af.location = APPLY_HITROLL;
		af.bitvector = AFF_ADRENALINE;
		accum_duration = false;
		to_vict = "A rush of adrenaline hits your brain!";
		break;

	case SPELL_DERMAL_HARDENING:
		af.level = get_skill_bonus(ch, SPELL_DERMAL_HARDENING);
		af.location = APPLY_AC;
		af.modifier = -10;
		af.duration = dice(4, (level / 8) + 1);
		accum_duration = true;
		to_vict = "You feel your skin tighten up and thicken.";
		break;

	case SPELL_VERTIGO:
        if (ch->checkReputations(victim))
            return;

		af.modifier = -(2 + (level / 10));
		af.duration = 6;
		af.location = APPLY_HITROLL;
		af.bitvector = AFF2_VERTIGO;
		af.aff_index = 2;
		accum_duration = false;
		af2.location = APPLY_DEX;
		af2.modifier = -(1 + (level / 16));
		af2.duration = af.duration;
		to_vict = "You feel a wave of vertigo rush over you!";
		to_room = "$n staggers in a dazed way.";
		break;

	case SPELL_BREATHING_STASIS:
		af.bitvector = AFF3_NOBREATHE;
		af.aff_index = 3;
		af.location = APPLY_MOVE;
		af.modifier = -(50 - (level / 2));
		af.duration = (dice(1, 1 + (level / 8)) * (level / 16));
		to_vict = "Your breathing rate drops into a static state.";
		break;

	case SPELL_METABOLISM:
		af.location = APPLY_SAVING_CHEM;
		af.duration = dice(2, 2 + (level / 8));
		af.modifier = (level / 4);
		to_vict = "Your metabolism speeds up.";
		break;

	case SPELL_RELAXATION:
		af.location = APPLY_MOVE;
		af.duration = dice(2, 2 + (level / 8));
		af.modifier = -(35 - (level / 2));
		af.aff_index = 3;
		af.bitvector = AFF3_MANA_TAP;
		af2.location = APPLY_STR;
		af2.duration = af.duration;
		af2.modifier = -1;
		to_vict = "Your body and mind relax.";
		break;

	case SPELL_CELL_REGEN:
        if (level + GET_CON(victim) > number(34, 70))
            af.bitvector = AFF_REGEN;

        af.location = APPLY_CON;
        af.modifier = 1;
        af.duration = dice(1, 1 + (level / 8));
        to_vict = "Your cell regeneration rate increases.";

        if (affected_by_spell(victim, SKILL_HAMSTRING)) {
            affect_from_char(victim, SKILL_HAMSTRING);
            act("The wound on your leg closes!", false, victim, 0, ch,
                TO_CHAR);
            act("The gaping wound on $n's leg closes.", true, victim, 0,
                ch, TO_ROOM);
        }
		break;

	case SPELL_PSISHIELD:

		af.bitvector = AFF3_PSISHIELD;
		af.duration = dice(1, 1 + (level / 8)) + 3;
		af.aff_index = 3;
		to_vict = "You feel a psionic shield form around your mind.";
		break;

	case SPELL_MOTOR_SPASM:
		af.location = APPLY_DEX;
		af.modifier = -(number(0, level / 8));
		af.duration = number(0, level / 16) + 1;
		to_vict = "Your muscles begin spasming uncontrollably.";
		break;

	case SPELL_PSYCHIC_RESISTANCE:

		af.location = APPLY_SAVING_PSI;
		af.modifier = -(5 + (level / 8));
		af.duration = dice(1, 1 + (level / 8)) + 3;
		to_vict =
			"The psychic conduits of your mind become resistant to external energies.";
		accum_duration = 1;
		break;

	case SPELL_PSYCHIC_CRUSH:
		af.location = APPLY_MANA;
		af.modifier = -(5 + (level / 8));
		af.duration = dice(1, 1 + (level / 16)) + 2;
		af.aff_index = 3;
		af.bitvector = AFF3_PSYCHIC_CRUSH;
		break;

    case SPELL_PSYCHIC_FEEDBACK:
        af.aff_index = 0;
        af.duration = (level / 8) + dice(1, 1 + (level / 8));
        to_vict = "You will now send psychic feedback to anyone who attacks you.";
        break;

		/* physic skills */
	case SPELL_GAMMA_RAY:
		af.duration = (level / 4);
		af.location = APPLY_HIT;
		af.modifier = -(level);
		if (GET_CLASS(ch) == CLASS_PHYSIC)
			af.modifier *= (GET_REMORT_GEN(ch) + 2) / 2;
        af.modifier = MAX(-(GET_MAX_HIT(victim) - 1), af.modifier);
		af2.location = APPLY_MOVE;
		af2.modifier = -(level / 2);
		af2.duration = af.duration;
		accum_affect = true;
		to_room = "$n appears slightly irradiated.";
		to_vict = "You feel irradiated... how irritating.";
		break;

	case SPELL_GRAVITY_WELL:
        if (ch->checkReputations(victim))
            return;
		af.duration = (level / 8);
		af.location = APPLY_STR;
		af.bitvector = AFF3_GRAVITY_WELL;
		af.aff_index = 3;

		if (GET_CLASS(ch) == CLASS_PHYSIC) {
			af.modifier = -(level / 5);
		} else {
			af.modifier = -(level / 8);
		}
		to_vict = "The gravity well seems to take hold on your body.";

		accum_affect = false;
		break;

    case SPELL_CAPACITANCE_BOOST:
		af.duration = 1 + (level / 4);
		af.modifier = 10 + (level * 2);
		af.location = APPLY_MOVE;
		to_vict = "You feel your energy capacity rise.";
		accum_duration = 1;
		break;

	case SPELL_VACUUM_SHROUD:
        af.type = SPELL_VACUUM_SHROUD;
		af.bitvector = AFF3_NOBREATHE;
		af.aff_index = 3;
		af.duration =
                     MAX(15, get_skill_bonus(ch, SPELL_VACUUM_SHROUD) / 4);
        af2.type = SPELL_VACUUM_SHROUD;
		af2.bitvector = AFF2_PROT_FIRE;
		af2.aff_index = 2;
		af2.duration =
                     MAX(15, get_skill_bonus(ch, SPELL_VACUUM_SHROUD) / 4);
		to_vict = "A total vacuum springs into existence around your body.";
        accum_affect = true;
		break;

	case SPELL_ALBEDO_SHIELD:
        af.type = SPELL_ALBEDO_SHIELD;
        af.bitvector = AFF3_EMP_SHIELD;
        af.aff_index = 3;
		af.duration = level;
		to_vict = "You feel protected from electromagnetic attacks.";
		break;

    case SPELL_GAUSS_SHIELD:
        af.type = SPELL_GAUSS_SHIELD;
        af.duration = get_skill_bonus(ch, SPELL_GAUSS_SHIELD) / 2;
        to_vict = "You feel protected from metal.";
        break;

	case SPELL_CHEMICAL_STABILITY:
		af.duration = (level / 4);
		to_room = "$n begins looking more chemically inert.";
		to_vict = "You feel more chemically inert.";
		break;

	case SPELL_ACIDITY:
	case SPELL_ACID_BREATH:	// acid breath
		{
			struct affected_type *af_ptr =
				affected_by_spell(victim, SPELL_CHEMICAL_STABILITY);

			// see if we have chemical stability
			if (af_ptr) {
				act("$n's chemical stability prevents further acidification from occurring!", false, victim, 0, 0, TO_ROOM);
				send_to_char(victim,
					"You chemical stability prevents further acidification from occurring!\r\n");
				af_ptr->duration -= (level / 8);

				if (af_ptr->duration <= 0) {
					if (af_ptr->type <= MAX_SPELLS &&
						af_ptr->type > 0 &&
						*spell_wear_off_msg[af_ptr->type]) {
						send_to_char(victim, "%s\r\n",
                                     spell_wear_off_msg[af_ptr->type]);
					}
					affect_remove(victim, af_ptr);
				}
				return;
			}
		}
		af.duration = (level / 8);
		af.bitvector = AFF3_ACIDITY;
		af.aff_index = 3;
		accum_duration = true;
		break;

	case SPELL_HALFLIFE:
		to_room = "$n becomes radioactive.";
		to_vict = "You suddenly begin to feel radioactive.";
		af.duration = (level / 4);
		af.bitvector = AFF3_RADIOACTIVE;
		af.aff_index = 3;
		af.location = APPLY_CON;
		af.modifier = -number(1, 2 + (level / 16));
		break;

	case SPELL_ELECTROSTATIC_FIELD:
		to_room = "An electrostatic field crackles into being around $n.";
		to_vict = "An electrostatic field crackles into being around you.";
		af.duration = (level / 4) + 2;
		break;

	case SPELL_RADIOIMMUNITY:
		to_vict = "You feel more resistant to radiation.";
		af.duration = (level / 2);
		af.bitvector = AFF2_PROT_RAD;
		af.aff_index = 2;
		break;

	case SPELL_ATTRACTION_FIELD:
		af.duration = 1 + (level / 4);
		af.modifier = 10 + (level);
		af.location = APPLY_AC;
		af.bitvector = AFF3_ATTRACTION_FIELD;
		af.aff_index = 3;
		to_room = "$n suddenly becomes attractive like a magnet!";
		to_vict = "You feel very attractive -- to weapons.";
		break;

   	case SPELL_REPULSION_FIELD:
		af.location = APPLY_AC;
		af.duration = MAX(12, get_skill_bonus(ch, SPELL_REPULSION_FIELD) / 4);
		af.modifier = -((get_skill_bonus(ch, SPELL_REPULSION_FIELD) / 4) + 20);
		accum_duration = true;
		to_vict = "The space around you begins repelling matter.";
		break;

	case SPELL_FLUORESCE:
		af.duration = 8 + level;
		af.bitvector = AFF2_FLUORESCENT;
		af.aff_index = 2;
		to_vict = "The area around you is illuminated with fluorescent atoms.";
		to_room = "The light of fluorescent atoms surrounds $n.";
		break;

	case SPELL_TEMPORAL_COMPRESSION:
		af.duration = get_skill_bonus(ch, SPELL_TEMPORAL_COMPRESSION) / 2;
		af.bitvector = AFF2_HASTE;
		af.aff_index = 2;
		accum_duration = false;
		to_vict = "Time seems to slow down around you.";
		break;

	case SPELL_TEMPORAL_DILATION:
		if (mag_savingthrow(victim, level, SAVING_PHY)) {
			send_to_char(ch, "%s", NOEFFECT);
			return;
		}
		af.duration = 1 + (level / 4);
		af.bitvector = AFF2_SLOW;
		af.aff_index = 2;
		af.location = APPLY_DEX;
		af.modifier = -number(0,
                              get_skill_bonus(ch, SPELL_TEMPORAL_DILATION) / 25);
		to_vict = "Time seems to speed up around you as your movements slow to a crawl.";
		break;

    case SPELL_DIMENSIONAL_SHIFT:
        af.duration = number(1+get_skill_bonus(ch, SPELL_DIMENSIONAL_SHIFT)/15,
                            1+get_skill_bonus(ch, SPELL_DIMENSIONAL_SHIFT)/9);
        af.aff_index = 0;
        to_vict = "You step into an infinitesimally different plane of the multiverse.";
        break;

	case SPELL_TAINT:
		if (HAS_SYMBOL(victim)) {
			send_to_char(ch, "Your rune of taint fails to form.\r\n");
			return;
		} else {
			af.bitvector = AFF3_TAINTED;
			af.aff_index = 3;
			af.location = af.modifier = 0;
			af.duration = dice(get_skill_bonus(ch, SPELL_TAINT), 6);
			af.level = 2 * (get_skill_bonus(ch, SPELL_TAINT));
			to_vict =
				"The mark of the tainted begins to burn brightly on your forehead!";
			to_room =
				"The mark of the tainted begins to burn brightly on $n's forehead!";
		}
		break;
	case SPELL_SYMBOL_OF_PAIN:
		if (HAS_SYMBOL(victim)) {
			send_to_char(ch, "Your symbol of pain fails.\r\n");
			act("$N's symbol of pain fails to mark you!",
				false, victim, 0, ch, TO_CHAR);
			return;
		} else {
			af.bitvector = AFF3_SYMBOL_OF_PAIN;
			af.aff_index = 3;
			af.location = APPLY_DEX;
			af.modifier = -(level / 7);
			af.duration = number(1, 3);
			af2.location = APPLY_HITROLL;
			af2.modifier = -(1 + level / 4);
			af2.duration = af.duration;
			to_vict = "You shudder and shake as your mind burns!";
			to_room = "$n shudders and shakes in pain!";
		}
		break;
	case SPELL_TIME_WARP:
		af.duration = 3 + level;
		af.bitvector = AFF_TIME_WARP;
		accum_duration = true;
		to_vict = "You are now able to move freely through time.";
		break;

	case SPELL_TRANSMITTANCE:
		if (!victim)
			victim = ch;
		to_room = "$n's body slowly becomes completely transparent.";
		to_vict = "You become transparent.";

		af.duration = 8 + (level / 4);
		af.modifier = -16;
		af.location = APPLY_AC;
		af.bitvector = AFF2_TRANSPARENT;
		af.aff_index = 2;
		accum_duration = true;
		break;

	case SPELL_TIDAL_SPACEWARP:
		af.duration = 6 + level;
		af.bitvector = AFF_INFLIGHT;
		accum_duration = true;
		to_vict = "Your feet lift lightly from the ground.";
		to_room = "$n begins to hover above the ground.";
		victim->setPosition(POS_FLYING);
		break;

		//
		//
	case SPELL_QUAD_DAMAGE:
		af.duration = 6;
		accum_affect = true;
		to_vict = "There is a screaming roar and you begin to glow brightly!";
		to_room = "There is a screaming roar as $n begins to glow brightly!";
		break;

		// physic mag_affect items
	case SPELL_DENSIFY:
		af.duration = 1 + (level / 2);
		af.location = APPLY_CHAR_WEIGHT;
		af.modifier = level + GET_INT(ch);

		if (victim == ch)
			accum_affect = true;

		to_vict = "You feel denser.";

		break;

	case SPELL_LATTICE_HARDENING:
/*
  Fireball: for instance, carbon or silicon have really nice lattices
  Forget: so would it harden your entire structure, or just your shell?
  Fireball: probably most of your solid structure, a little bit
  Fireball: like harder bones, skin, organ membranecs
*/
		if (ch != victim) {
			send_to_char(ch, "There seems to be no affect.\r\n");
			return;
		} else {
			af.duration = 1 + (level / 2);
			af.level = level;
			accum_affect = false;
			to_vict = "Your molecular bonds seem strengthened.";
		}
		break;
	case SPELL_REFRACTION:
		af.duration = 1 + (level / 2);
		af.location = APPLY_AC;
		af.modifier = -GET_INT(ch);
		af.bitvector = AFF2_DISPLACEMENT;
		af.aff_index = 2;
		accum_duration = false;
		to_vict = "Your body becomes irregularly refractive.";
		to_room = "$n's body becomes irregularly refractive.";
		break;

		/* REMORT SKILLS GO HERE */

	case SPELL_SHIELD_OF_RIGHTEOUSNESS:
		if (!IS_GOOD(victim))
			return;
		af.duration = level / 4;
		af.location = APPLY_CASTER;
		af.modifier = !IS_NPC(ch) ? GET_IDNUM(ch) : -MOB_IDNUM(ch);
		af2.duration = af.duration;
		af2.location = APPLY_AC;
		af2.modifier = -10;
		if (ch == victim) {
			to_vict = "A shield of righteousness appears around you.";
			to_room = "A shield of righteousness expands around $N.";
		} else
			to_vict = "You feel enveloped in $N's shield of righteousness.";
		break;
	case SPELL_BLACKMANTLE:
		af.duration = level / 4;
		af.location = APPLY_HIT;
		af.modifier = -level;
        af.modifier = MAX(-(GET_MAX_HIT(victim) - 1), af.modifier);
		to_room = "A mantle of darkness briefly surrounds $n.";
		to_vict = "An evil black mantle of magic surrounds you.";
		break;
	case SPELL_SANCTIFICATION:
		af.duration = level / 8;
		af.location = APPLY_MOVE;
		af.modifier = level / 2;
		to_room = "An aura of sanctification glows about $n.";
		to_vict = "You have been sanctified!";
		break;
	case SPELL_STIGMATA:
		if (IS_GOOD(victim)) {
			send_to_char(ch, "You cannot stigmatize good characters.\r\n");
			return;
		}
		if (HAS_SYMBOL(victim)) {
			send_to_char(ch, "Your stigmata fails.\r\n");
			return;
		}
		af.duration = level / 4;
		to_room = "A bloody stigmatic mark appears on $n's forehead.";
		to_vict = "A bloody stigmatic mark appears on your forehead.";
		break;
	case SPELL_ENTANGLE:
		if (ch->in_room->sector_type != SECT_FIELD &&
			ch->in_room->sector_type != SECT_FOREST &&
			ch->in_room->sector_type != SECT_HILLS &&
			ch->in_room->sector_type != SECT_MOUNTAIN &&
			ch->in_room->sector_type != SECT_FARMLAND &&
			ch->in_room->sector_type != SECT_ROCK &&
			ch->in_room->sector_type != SECT_SWAMP &&
			ch->in_room->sector_type != SECT_CITY &&
			ch->in_room->sector_type != SECT_CATACOMBS &&
			(ch->in_room->sector_type != SECT_ROAD || !OUTSIDE(ch)) &&
			ch->in_room->sector_type != SECT_JUNGLE) {
			send_to_char(ch, "There is not enough vegetation here for that.\r\n");
			return;
		}
		af.location = APPLY_HITROLL;
		af2.location = APPLY_DEX;

		if (ch->in_room->sector_type == SECT_CITY
			|| ch->in_room->sector_type == SECT_CRACKED_ROAD) {
			af.duration =
				(level + (CHECK_SKILL(ch, SPELL_ENTANGLE) / 4)) / 4;
			to_room =
				"The grass and weeds growing through cracks in the pavement come alive, entangling $n where $e stands!";
			to_vict =
				"The grass and weeds growing through cracks in the pavement come alive, entangling you where you stands!";
			af.modifier = -(level / 4);
			af2.modifier = -(level / 16);
		} else {
			to_room =
				"The vines and vegetation surrounding $n come alive, entangling $n where $e stands!";
			to_vict =
				"The vines and vegetation surrounding you come alive, entangling you where you stand!";
			if (!OUTSIDE(ch) || ch->in_room->sector_type == SECT_ROAD) {
				af.duration =
					(level + (CHECK_SKILL(ch, SPELL_ENTANGLE) / 2)) / 4;
				af.modifier = -(level / 4);
				af2.modifier = -(level / 16);
			} else {
				af.duration =
					(level + (CHECK_SKILL(ch, SPELL_ENTANGLE) / 2)) / 2;
				af.modifier = -(level / 2);
				af2.modifier = -(level / 8);
			}
		}
		af2.duration = af.duration;
		break;
	case SPELL_AMNESIA:
		af.duration = MAX(10, level - 20);
		af.location = APPLY_INT;
		af.modifier = -(level / 8);
		to_room = "A cloud of forgetfulness passes over $n's face.";
		to_vict = "An wave of amnesia washes over your mind.";
		break;
	case SPELL_ANTI_MAGIC_SHELL:
		af.duration = level;
		to_room = "A dark and glittering translucent shell appears around $n.";
		to_vict =
			"A dark and glittering translucent shell appears around you.";
		break;
	case SPELL_SPHERE_OF_DESECRATION:
		af.duration = level;
		to_room = "A shimmering dark translucent sphere appears around $n.";
		to_vict = "A shimmering dark translucent sphere appears around you.";
		break;
	case SPELL_DIVINE_INTERVENTION:
		af.duration = level;
		to_room = "A shimmering pearly translucent sphere appears around $n.";
		to_vict = "A shimmering pearly translucent sphere appears around you.";
		break;
	case SPELL_MALEFIC_VIOLATION:
		af.duration = level;
		to_vict = "You feel wickedly potent.";
		break;
	case SPELL_RIGHTEOUS_PENETRATION:
		af.duration = level;
		to_vict =
			"You have been granted terrible potency against the forces of evil.";
		break;
	case SPELL_VAMPIRIC_REGENERATION:
		af.duration =
			(3 + get_skill_bonus(ch, SPELL_VAMPIRIC_REGENERATION) / 25);
		af.location = APPLY_CASTER;
		af.modifier = !IS_NPC(ch) ? GET_IDNUM(ch) : -MOB_IDNUM(ch);
		to_vict = "You feel a vampiric link formed between you and $N!";
		break;
	case SPELL_LOCUST_REGENERATION:
		af.duration = 3 + (get_skill_bonus(ch, SPELL_LOCUST_REGENERATION) / 25);
		af.location = APPLY_CASTER;
		af.modifier = !IS_NPC(ch) ? GET_IDNUM(ch) : -MOB_IDNUM(ch);
		to_vict = "You shriek in terror as $N drains your energy!";
		break;
	case SPELL_ENTROPY_FIELD:
		af.duration = level;
		to_vict = "You suddenly feel like you're falling apart!";
		break;
	case SPELL_DIVINE_POWER:
		// Set duration of all affects
		af.duration = 8 + (get_skill_bonus(ch, SPELL_DIVINE_POWER) / 10);
		af2.duration = 8 + (get_skill_bonus(ch, SPELL_DIVINE_POWER) / 10);
		aff_array[0].duration =
			8 + (get_skill_bonus(ch, SPELL_DIVINE_POWER) / 10);
		aff_array[1].duration =
			8 + (get_skill_bonus(ch, SPELL_DIVINE_POWER) / 10);

		// Set type of all affects
		af.type = SPELL_DIVINE_POWER;
		af2.type = SPELL_DIVINE_POWER;
		aff_array[0].type = SPELL_DIVINE_POWER;
		aff_array[1].type = SPELL_DIVINE_POWER;

		// Should only need to set the bitvector with one of the affects
		af.bitvector = AFF3_DIVINE_POWER;
		af.aff_index = 3;
		// Set the to_vict message on the first affect
		to_vict = "Your veins course with the power of your Guiharia!";

		// The location of each affect
		af.location = APPLY_STR;
		af2.location = APPLY_HIT;
		aff_array[0].location = APPLY_HIT;
		aff_array[1].location = APPLY_HIT;

		// The amoune of modification to each affect
		af.modifier = (get_skill_bonus(ch, SPELL_DIVINE_POWER) / 15);
		af2.modifier = get_skill_bonus(ch, SPELL_DIVINE_POWER);
		aff_array[0].modifier = get_skill_bonus(ch, SPELL_DIVINE_POWER);
		aff_array[1].modifier = get_skill_bonus(ch, SPELL_DIVINE_POWER);

		if (!AFF3_FLAGGED(ch, AFF3_DIVINE_POWER))
			accum_affect = 1;
		break;

    case SPELL_FIRE_BREATHING:
		af.duration = 10 + get_skill_bonus(ch, SPELL_FIRE_BREATHING)/4;
		to_room = "$n's eyes begin to glow a deep red.";
		to_vict = "A warm tingling begins in the back of your throat.";
		break;

    case SPELL_FROST_BREATHING:
		af.duration = 10 + get_skill_bonus(ch, SPELL_FROST_BREATHING)/4;
		to_room = "$n's eyes begin to glow a deep blue.";
		to_vict = "A cold tingling begins in the back of your throat.";
		break;

    // Bard stuff

    case SONG_DRIFTERS_DITTY:
		aff_array[0].duration = 1 + (level / 2);
		aff_array[0].modifier = 15 + get_skill_bonus(ch, SONG_DRIFTERS_DITTY);
		aff_array[0].location = APPLY_MOVE;

		to_vict = "The song bolsters your spirit!";
		accum_duration = 1;
		break;

	case SONG_ARIA_OF_ARMAMENT:
		aff_array[0].location = APPLY_AC;
		aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_ARIA_OF_ARMAMENT) / 4;
		aff_array[0].modifier = -((get_skill_bonus(ch, SONG_ARIA_OF_ARMAMENT) / 4) + 20);

		accum_duration = 1;
		to_vict = "You feel the song form a protective shield around you.";
		break;

	case SONG_VERSE_OF_VULNERABILITY:
		aff_array[0].location = APPLY_AC;
		aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_VERSE_OF_VULNERABILITY) / 8;
		aff_array[0].modifier = (get_skill_bonus(ch, SONG_VERSE_OF_VULNERABILITY) / 4) + number(5, 20);
        aff_array[1].location = APPLY_DEX;
        aff_array[1].duration = aff_array[0].duration;
        aff_array[1].modifier = -(1 + get_skill_bonus(ch, SONG_VERSE_OF_VULNERABILITY) / 25);
		to_vict = "Your armor softens at $N's words!";
        to_room = "$n's armor appears to grow softer with $N's song!";
		WAIT_STATE(victim, PULSE_VIOLENCE * 2);
		break;

    case SONG_MELODY_OF_METTLE:
        aff_array[0].location = APPLY_CON;
        aff_array[0].duration = (get_skill_bonus(ch, SONG_MELODY_OF_METTLE) / 4) + 10;
        aff_array[0].modifier = 1 + (get_skill_bonus(ch, SONG_MELODY_OF_METTLE) / 2) / 25;

        aff_array[1].location = APPLY_HIT;
        aff_array[1].duration = aff_array[0].duration;
        aff_array[1].modifier = 50 + MIN(get_skill_bonus(ch, SONG_MELODY_OF_METTLE), 125);
        to_vict = "The power of the song infuses your spirit!";
        break;

    case SONG_REGALERS_RHAPSODY:
        aff_array[0].location = APPLY_NOHUNGER;
        aff_array[0].duration = (get_skill_bonus(ch, SONG_REGALERS_RHAPSODY) / 4) + 10;
        aff_array[0].modifier = 1;

        aff_array[1].location = APPLY_NOTHIRST;
        aff_array[1].duration = aff_array[0].duration;
        aff_array[1].modifier = 1;
        to_vict = "The uplifting tune drains away your hunger and thirst.";
        break;

    case SONG_DEFENSE_DITTY:
        aff_array[0].location = APPLY_SAVING_PSI;
        aff_array[0].duration = (get_skill_bonus(ch, SONG_DEFENSE_DITTY) / 8) + 20;
        aff_array[0].modifier = -(1 + get_skill_bonus(ch, SONG_DEFENSE_DITTY) / 10);

        if (number(0, 120) < get_skill_bonus(ch, SONG_DEFENSE_DITTY)) {
            aff_array[1].location = APPLY_SAVING_PHY;
            aff_array[1].duration = aff_array[0].duration;
            aff_array[1].modifier = aff_array[0].modifier;
        }
        if (number(0, 200) < get_skill_bonus(ch, SONG_DEFENSE_DITTY)) {
            aff_array[2].location = APPLY_SAVING_SPELL;
            aff_array[2].duration = aff_array[0].duration;
            aff_array[2].modifier = aff_array[0].modifier;
        }
        to_vict = "Your resistances increase as the music surrounds you.";
        break;

	case SONG_ALRONS_ARIA:
		aff_array[0].modifier = dice(2, (level / 8) + 1);
		aff_array[0].duration = 3 + (level / 4);
		aff_array[0].location = APPLY_HITROLL;
		aff_array[0].bitvector = AFF_CONFIDENCE;

		aff_array[1].location = APPLY_SAVING_SPELL;
		aff_array[1].modifier = -dice(1, (level / 8) + 1);
		aff_array[1].duration = aff_array[0].duration;
		accum_duration = 1;
		to_vict = "Your confidence soars!";
		break;

    case SONG_VERSE_OF_VALOR:
        aff_array[0].location = APPLY_HITROLL;
        aff_array[0].duration = 6 + get_skill_bonus(ch, SONG_VERSE_OF_VALOR) / 8;
        aff_array[0].modifier = 5 + get_skill_bonus(ch, SONG_VERSE_OF_VALOR) / 25 + number(0, 6);

        to_vict = "The valor of heros gone comes crashing into your mind!";
        break;

	case SONG_WHITE_NOISE:
		aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_WHITE_NOISE) / 25;
		aff_array[0].bitvector = AFF_CONFUSION;

		if (victim->getPosition() > POS_SLEEPING)
			to_room = "$n stops suddenly and stares around as if confused.";
		to_vict = "You suddenly feel very confused!";
		WAIT_STATE(victim, PULSE_VIOLENCE * 2);
		break;

	case SONG_CHANT_OF_LIGHT:
		aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_CHANT_OF_LIGHT) / 4;
		aff_array[0].bitvector = AFF_GLOWLIGHT;

		aff_array[1].duration = aff_array[0].duration;
		aff_array[1].bitvector = AFF2_ENDURE_COLD;
		aff_array[1].aff_index = 2;
		accum_duration = true;
		to_vict = "The air around you begins to emit a warm glow.";
		to_room = "$n's music causes the air to glow warmly.";
		break;

    case SONG_IRRESISTABLE_DANCE:
        aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_IRRESISTABLE_DANCE) / 25;
		aff_array[0].modifier = -(4 + (get_skill_bonus(ch, SONG_IRRESISTABLE_DANCE) / 20));
		aff_array[0].location = APPLY_HITROLL;

        if (victim->getPosition() > POS_SITTING) {
            to_vict = "You begin to dance uncontrollably!";
            to_room = "$n begins to dance uncontrollably!";
        }
        break;

    case SONG_INSIDIOUS_RHYTHM:
        aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_INSIDIOUS_RHYTHM) / 25;
		aff_array[0].modifier = -(2 + (get_skill_bonus(ch, SONG_INSIDIOUS_RHYTHM) / 20));
		aff_array[0].location = APPLY_INT;

        to_vict = "$N's music snakes it's way into your brain, dulling your senses.";
        to_room = "The music causes $n's eyes glaze over.";
        break;

	case SONG_EAGLES_OVERTURE:
		aff_array[0].location = APPLY_CHA;
		aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_EAGLES_OVERTURE) / 4;
		aff_array[0].modifier = 5 + get_skill_bonus(ch, SONG_EAGLES_OVERTURE) / 20;

		accum_duration = 1;
		to_vict = "The song lifts your spirits and puts a smile on your face.";
		break;

	case SONG_WEIGHT_OF_THE_WORLD:
		aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_WEIGHT_OF_THE_WORLD) / 25;
		aff_array[0].bitvector = AFF2_TELEKINESIS;
		aff_array[0].aff_index = 2;

		to_vict = "You feel the weight of the world lifted from your shoulders.";
		break;

	case SONG_GUIHARIAS_GLORY:
		aff_array[0].modifier = dice(2, (get_skill_bonus(ch, SONG_GUIHARIAS_GLORY) / 16) + 1);
		aff_array[0].duration = 3 + (get_skill_bonus(ch, SONG_GUIHARIAS_GLORY) / 8);
		aff_array[0].location = APPLY_DAMROLL;

		to_vict = "You feel the power of dieties flowing in your veins!";
		break;

	case SONG_UNLADEN_SWALLOW_SONG:
		if (victim->getPosition() <= POS_SLEEPING) {
			act("$N is asleep and probably should not fly right now.", false, ch, 0, victim,
				TO_CHAR);
			return;
		}
		aff_array[0].duration = 5 + get_skill_bonus(ch, SONG_UNLADEN_SWALLOW_SONG) / 10;
		aff_array[0].bitvector = AFF_INFLIGHT;

		accum_duration = true;
		to_vict = "The music lifts you off your feet and sustains you.";
		to_room = "$n is lifted by the music and held suspended.";
		victim->setPosition(POS_FLYING);
		break;

    case SONG_POWER_OVERTURE:
        if (ch != victim) {
            send_to_char(ch, "%s doesn't seem to be moved by your song.\r\n",
                         GET_NAME(victim));
            return;
        }

        aff_array[0].duration = 1 + get_skill_bonus(ch, SONG_EAGLES_OVERTURE) / 4;
        aff_array[0].modifier = 1 + get_skill_bonus(ch, SONG_POWER_OVERTURE) / 30 + number(0, 2);
        aff_array[0].location = APPLY_STR;

        aff_array[1].duration = aff_array[0].duration;
        aff_array[1].modifier = aff_array[0].modifier + number(0, 5);
        aff_array[1].location = APPLY_HITROLL;

        to_vict = "Your strength seems to grow as the song swells.";
        break;

    case SONG_WOUNDING_WHISPERS:
        if (ch != victim) {
            send_to_char(ch, "Nothing seems to happen.\r\n");
            send_to_char(victim, "Nothing seems to happen.\r\n");
            return;
        }
        aff_array[0].duration = 5 + get_skill_bonus(ch, SONG_WOUNDING_WHISPERS) / 4;
        to_vict = "Whispers of your song begin whirling around you!";
        to_room = "Whispers of $N's song begin whirling around $M!";
        break;

    case SONG_RHYTHM_OF_RAGE: {
        if (ch != victim) {
            send_to_char(ch, "Your tune fails to enrage %s.\r\n", GET_NAME(victim));
        }

        aff_array[0].type = SKILL_BERSERK;
        aff_array[1].type = SKILL_BERSERK;
        aff_array[2].type = SKILL_BERSERK;

        aff_array[0].duration = 1 + (get_skill_bonus(ch, SONG_RHYTHM_OF_RAGE) / 32);

        aff_array[1].duration = aff_array[0].duration;
        aff_array[2].duration = aff_array[0].duration;

        aff_array[0].location = APPLY_INT;
        aff_array[1].location = APPLY_WIS;
        aff_array[2].location = APPLY_DAMROLL;

        aff_array[0].modifier = -(4 + (get_skill_bonus(ch, SONG_RHYTHM_OF_RAGE) / 32));
        aff_array[1].modifier = aff_array[0].modifier;
        aff_array[2].modifier = (1 + (get_skill_bonus(ch, SONG_RHYTHM_OF_RAGE) / 12) +
                                 (GET_LEVEL(ch) / 16));

        aff_array[0].aff_index = 2;
        aff_array[0].bitvector = AFF2_BERSERK;

        act("The music drives you feril with rage!", false, ch, 0, 0, TO_CHAR);
        act("$n looks murderous.  You might want to get out of here!", false,
            ch, 0, 0, TO_ROOM);

        struct creatureList_iterator it = ch->in_room->people.begin();
        for (; it != ch->in_room->people.end(); ++it) {
            if (ch == (*it) || !can_see_creature(ch, (*it)))
                continue;

            int percent = (number(1, 101) - GET_LEVEL(ch));
            if (percent < CHECK_SKILL(ch, SKILL_BERSERK))
                continue;
            else {
                act("You attack $N in your berserk rage!!!",
                    false, ch, 0, (*it), TO_CHAR);
                act("$n attacks you in $s berserk rage!!!",
                    false, ch, 0, (*it), TO_VICT);
                act("$n attacks $N in $s berserk rage!!!",
                    true, ch, 0, (*it), TO_NOTVICT);
                hit(ch, (*it), TYPE_UNDEFINED);
                break;
            }
        }
        break;
    }

    case SONG_ARIA_OF_ASYLUM:
        aff_array[0].duration = 5 + get_skill_bonus(ch, SONG_ARIA_OF_ASYLUM) / 8;
        aff_array[0].location = APPLY_CASTER;
        if (IS_NPC(ch))
            aff_array[0].modifier = -(GET_IDNUM(ch));
        else
            aff_array[0].modifier = GET_IDNUM(ch);

        to_vict = "A gossimer shield of music forms around you.";
        to_room = "A gossimer shield of music forms around $n";
    break;

	case SONG_FORTISSIMO:
        aff_array[0].duration = 1 + GET_CHA(ch) + (get_skill_bonus(ch, SONG_FORTISSIMO) / 8);
        aff_array[0].location = APPLY_CASTER;

		if (IS_NPC(ch))
            aff_array[0].modifier = -(GET_IDNUM(ch));
        else
            aff_array[0].modifier = GET_IDNUM(ch);

        to_vict = "The air around you begins to vibrate with an increased intensity.";
    break;

    case SONG_LICHS_LYRICS:
        aff_array[0].duration = 1 + (GET_CHA(ch) / 8) +
                                (get_skill_bonus(ch, SONG_LICHS_LYRICS) / 16);
        aff_array[0].location = APPLY_CASTER;

        if (IS_NPC(ch))
            aff_array[0].modifier = -(GET_IDNUM(ch));
        else
            aff_array[0].modifier = GET_IDNUM(ch);

        to_vict = "Your flesh begins to rot and decay!";
        to_room = "$n's flesh begins to rot and decay!";
        break;

    case SONG_MISDIRECTION_MELISMA:
        aff_array[0].duration = 1 + (GET_CHA(ch) / 4) +
                                (get_skill_bonus(ch, SONG_MISDIRECTION_MELISMA) / 16);
        aff_array[0].location = APPLY_CASTER;

        if (IS_NPC(ch))
            aff_array[0].modifier = -(GET_IDNUM(ch));
        else
            aff_array[0].modifier = GET_IDNUM(ch);

        to_vict = "You begin misdirecting attempts to track you.";
        break;

    case SONG_MIRROR_IMAGE_MELODY:
        aff_array[0].duration = 1 + (GET_CHA(ch) / 4) +
                               (get_skill_bonus(ch, SONG_MIRROR_IMAGE_MELODY) / 8);
        aff_array[0].location = APPLY_CASTER;
        aff_array[0].modifier = 1 + (GET_CHA(ch) / 8) +
                                (get_skill_bonus(ch, SONG_MIRROR_IMAGE_MELODY) / 33);
        to_vict = "Mirror images of yourself begin moving around you.";
        to_room = "Mirror images of $n begin moving around $m.";
        break;

    default:
		errlog("unknown spell %d in mag_affects.", spellnum);
		break;
	}

	/*
	 * If this is a mob that has this affect set in its mob file, do not
	 * perform the affect.  This prevents people from un-sancting mobs
	 * by sancting them and waiting for it to fade, for example.
	 */
	if (IS_NPC(victim)) {
		int done = 0;
		afp = &af;
		while (!done) {
			if (afp->aff_index == 0) {
				if (AFF_FLAGGED(victim, afp->bitvector) &&
					!affected_by_spell(victim, spellnum)) {
					send_to_char(ch, "%s", NOEFFECT);
					return;
				}
			} else if (afp->aff_index == 2) {
				if (AFF2_FLAGGED(victim, afp->bitvector) &&
					!affected_by_spell(victim, spellnum)) {
					send_to_char(ch, "%s", NOEFFECT);
					return;
				}
			} else if (afp->aff_index == 3) {
				if (AFF3_FLAGGED(victim, afp->bitvector) &&
					!affected_by_spell(victim, spellnum)) {
					send_to_char(ch, "%s", NOEFFECT);
					return;
				}
			}
			if (afp == &af)
				afp = &af2;
			else
				done = 1;
		}
	}

    // Lingering song causes a bards songs to last longer
    if (IS_BARD(ch) && SPELL_IS_BARD(spellnum)) {
        //we do the check first so that we get all of the applies of a given spell
        //prevents multiple wear off messages
        if (CHECK_SKILL(ch, SKILL_LINGERING_SONG) > number(1, 120)) {
            for (int x = 0; x < 8; x++) {
                aff_array[x].duration = (int)(aff_array[x].duration * 1.5);
            }
        }
    }

	/* If the victim is already affected by this spell, and the spell does
	 * not have an accumulative effect, then fail the spell.
	 */
	if (affected_by_spell(victim, spellnum) && !(accum_duration
			|| accum_affect)) {
		return;
    }

    if (af.bitvector || af.location || af.duration)
	    affect_join(victim, &af, accum_duration, false, accum_affect, false);
	if (af2.bitvector || af2.location || af2.duration)
		affect_join(victim, &af2, accum_duration, false, accum_affect, false);

	for (int x = 0; x < 8; x++) {
		if (aff_array[x].bitvector || aff_array[x].location || aff_array[x].duration)
			affect_join(victim, &aff_array[x], accum_duration, false,
				accum_affect, false);
	}
	if (to_vict != NULL)
		act(to_vict, false, victim, 0, ch, TO_CHAR);
	if (to_room != NULL)
		act(to_room, true, victim, 0, ch, TO_ROOM);

	if (spellnum == SPELL_DIVINE_POWER && accum_affect)
		GET_HIT(ch) += (get_skill_bonus(ch, SPELL_DIVINE_POWER) * 3);

    if (spellnum == SONG_MELODY_OF_METTLE)
        GET_HIT(ch) += 50 + MIN(get_skill_bonus(ch, SONG_MELODY_OF_METTLE), 125);

	if (spellnum == SPELL_FEAR && !mag_savingthrow(victim, level, SAVING_PSI)
		&& victim->getPosition() > POS_SITTING)
		do_flee(victim, tmp_strdup(""), 0, 0, 0);

	// This looks redundant, but serves as a reminder that the damage()
	// function might kill the character
	if (spellnum == SPELL_THORN_SKIN)
		if (damage(ch, ch, (150 - get_skill_bonus(ch, SPELL_THORN_SKIN) / 2),
				SPELL_THORN_SKIN_CASTING, WEAR_RANDOM))
			return;
}

/*
  * This function is used to provide services to mag_groups.  This function
  * is the one you should change to add new group spells.
  */

void
perform_mag_groups(int level, struct creature *ch,
	struct creature *tch, int *tdir, int spellnum, int savetype)
{
	switch (spellnum) {
	case SPELL_GROUP_HEAL:
		mag_points(level, ch, tch, tdir, SPELL_HEAL, savetype);
		break;
	case SPELL_GROUP_ARMOR:
		mag_affects(level, ch, tch, tdir, SPELL_ARMOR, savetype);
		break;
	case SPELL_GROUP_RECALL:
		spell_recall(level, ch, tch, NULL, NULL);
		break;
	case SPELL_GROUP_CONFIDENCE:
		mag_affects(level, ch, tch, tdir, SPELL_CONFIDENCE, savetype);
		break;
	case SPELL_SHIELD_OF_RIGHTEOUSNESS:
		if (!IS_GOOD(tch))
			break;
		mag_affects(level, ch, tch, tdir, SPELL_SHIELD_OF_RIGHTEOUSNESS, savetype);
		break;
    case SONG_DRIFTERS_DITTY:
		mag_affects(level, ch, tch, tdir, SONG_DRIFTERS_DITTY, savetype);
		mag_points(level, ch, tch, tdir, SONG_DRIFTERS_DITTY, savetype);
        break;
    case SONG_ARIA_OF_ARMAMENT:
        mag_affects(level, ch, tch, tdir, SONG_ARIA_OF_ARMAMENT, savetype);
        break;
    case SONG_MELODY_OF_METTLE:
        mag_affects(level, ch, tch, tdir, SONG_MELODY_OF_METTLE, savetype);
        break;
    case SONG_REGALERS_RHAPSODY:
        mag_affects(level, ch, tch, tdir, SONG_REGALERS_RHAPSODY, savetype);
        break;
    case SONG_DEFENSE_DITTY:
        mag_affects(level, ch, tch, tdir, SONG_DEFENSE_DITTY, savetype);
        break;
    case SONG_ALRONS_ARIA:
        mag_affects(level, ch, tch, tdir, SONG_ALRONS_ARIA, savetype);
        mag_unaffects(level, ch, tch, SONG_ALRONS_ARIA, savetype);
        break;
    case SONG_LUSTRATION_MELISMA:
        mag_unaffects(level, ch, tch, SONG_LUSTRATION_MELISMA, savetype);
        break;
    case SONG_VERSE_OF_VALOR:
        mag_affects(level, ch, tch, tdir, SONG_VERSE_OF_VALOR, savetype);
        break;
    case SONG_CHANT_OF_LIGHT:
        mag_affects(level, ch, tch, tdir, SONG_CHANT_OF_LIGHT, savetype);
        break;
    case SONG_EAGLES_OVERTURE:
        mag_affects(level, ch, tch, tdir, SONG_EAGLES_OVERTURE, savetype);
        break;
    case SONG_WEIGHT_OF_THE_WORLD:
        mag_affects(level, ch, tch, tdir, SONG_WEIGHT_OF_THE_WORLD, savetype);
        break;
    case SONG_GUIHARIAS_GLORY:
        mag_affects(level, ch, tch, tdir, SONG_GUIHARIAS_GLORY, savetype);
        break;
    case SONG_UNLADEN_SWALLOW_SONG:
        mag_affects(level, ch, tch, tdir, SONG_UNLADEN_SWALLOW_SONG, savetype);
        break;
    case SONG_ARIA_OF_ASYLUM:
        mag_affects(level, ch, tch, tdir, SONG_ARIA_OF_ASYLUM, savetype);
        break;
	default:
		errlog("Unknown spellnum %d in perform_mag_groups()",
			spellnum);
		break;

	}
}

/*
 * Every spell that affects the group should run through here
 * perform_mag_groups contains the switch statement to send us to the right
 * magic.
 *
 * group spells affect everyone grouped with the caster who is in the room,
 * caster last.
 *
 * To add new group spells, you shouldn't have to change anything in
 * mag_groups -- just add a new case to perform_mag_groups.
 */

void
mag_groups(int level, struct creature *ch, int spellnum, int savetype)
{
	struct creature *tch, *k;
    int tdir = -1;
	struct follow_type *f, *f_next;

	if (ch == NULL)
		return;

	if (!AFF_FLAGGED(ch, AFF_GROUP) && !SPELL_IS_BARD(spellnum))
		return;

	if (ch->master != NULL)
		k = ch->master;
	else
		k = ch;
	for (f = k->followers; f; f = f_next) {
		f_next = f->next;
		tch = f->follower;
		if (tch->in_room != ch->in_room)
			continue;
		if (!AFF_FLAGGED(tch, AFF_GROUP))
			continue;
		if (ch == tch)
			continue;
        if (ch->in_room != tch->in_room)
            continue;

		perform_mag_groups(level, ch, tch, &tdir, spellnum, savetype);
	}

	if ((k != ch) && AFF_FLAGGED(k, AFF_GROUP))
		perform_mag_groups(level, ch, k, &tdir, spellnum, savetype);
	perform_mag_groups(level, ch, ch, &tdir, spellnum, savetype);
}

/*
 * mass spells affect every creature in the room except the caster.
 * ONLY if (ch == FIGHTING(vict))
 * No spells of this char_class currently implemented as of Circle 3.0.
 */

void
mag_masses(byte level, struct creature *ch, int spellnum, int savetype)
{
	int found = 0;
	struct creatureList_iterator it = ch->in_room->people.begin();
	for (; it != ch->in_room->people.end(); ++it) {
		if ((*it) == ch || !(*it)->findCombat(ch))
			continue;
		found = true;
		mag_damage(level, ch, (*it), spellnum, savetype);
	}
	if (!found)
		send_to_char(ch,
			"This spell is only useful if someone is fighting you.\r\n");
}

/*
 * Every spell that affects an area (room) runs through here.  These are
 * generally offensive spells.  This calls mag_damage to do the actual
 * damage -- all spells listed here must also have a case in mag_damage()
 * in order for them to work.
 *
 *  area spells have limited targets within the room.
*/

int
mag_areas(byte level, struct creature *ch, int spellnum, int savetype)
{
	const char *to_char = NULL;
	const char *to_room = NULL;
	const char *to_next_room = NULL;
	struct room_data *was_in, *adjoin_room;
	byte count;
	int return_value = 0;

	if (ch == NULL)
		return 0;

	if (spellnum == SPELL_MASS_HYSTERIA) {
		struct creatureList_iterator it = ch->in_room->people.begin();
		for (count = 0; it != ch->in_room->people.end(); ++it)
			if ((*it) != ch && can_see_creature(ch, (*it)))
				count++;
		if (!count) {
			send_to_char(ch,
				"You need some people present to make that effective.\r\n");
			return 0;
		}
	}
	/*
	 * to add spells to this fn, just add the message here plus an entry
	 * in mag_damage for the damaging part of the spell.
	 */
	switch (spellnum) {
	case SPELL_EARTHQUAKE:
		to_char = "You gesture and the earth begins to shake all around you!";
		to_room =
			"$n gracefully gestures and the earth begins to shake violently!";
		to_next_room = "You hear a loud rumbling and feel the earth shake.";
		break;
	case SPELL_METEOR_STORM:
		to_char =
			"You gesture and a storm of meteors appears blazing in the sky!";
		to_room =
			"$n gestures and a storm of meteors appears blazing in the sky!";
		to_next_room =
			"A blazing storm of meteors appears and streaks across the sky!";
		break;
    case SPELL_MASS_HYSTERIA:
        to_char =
			"You begin to induce hysteria on the group of people around you.";
		to_room = "A wave of psychic power begins rolling off of $n.";
		break;
	case SPELL_FISSION_BLAST:
		to_char =
			"You begin splitting atoms and the room erupts into a fission blast!";
		to_room = "The room erupts in a blinding flash of light.";
		to_next_room = "A blinding flash of light briefly envelopes you.";
		break;
	case SONG_SONIC_DISRUPTION:
		to_char = "Sonic shockwaves pulse out in time with your powerful rhythm!";
		to_room = "$n plays a powerful rhythm creating pulsing sonic shockwaves!";
		to_next_room = "You hear a loud rhythm being played nearby.";
        break;
	case SONG_DIRGE:
		to_char = "Your dirge tugs at the souls of the undead, beckoning them back to the grave.";
		to_room = "$n's dirge tugs at the souls of the undead, beckoning them back to the grave.";
		to_next_room = "Chilling music can be heard in the distance.";
        break;
    case SONG_LICHS_LYRICS:
        to_char = "Your enchanting voice beckons to the life force of your victims.";
        to_room = "$n's voice begins to suck in the life force of $s surroundings!";
        to_next_room = "You feel a chill deep in your soul.";
	}

	if (to_char != NULL)
		act(to_char, false, ch, 0, 0, TO_CHAR);
	if (to_room != NULL)
		act(to_room, false, ch, 0, 0, TO_ROOM);

	if (spellnum == SPELL_EARTHQUAKE &&
        (room_is_underwater(ch->in_room)
				|| ch->in_room->isOpenAir()))
		return 0;
	if (spellnum == SPELL_METEOR_STORM
        && room_is_underwater(ch->in_room))
		return 0;

	// check for players if caster is not a pkiller
    struct creatureList_iterator it = ch->in_room->people.begin();
    for (; it != ch->in_room->people.end(); ++it) {
        struct creature *vict = *it;
		if (vict == ch)
			continue;
		if (!IS_NPC(vict) && PRF_FLAGGED(vict, PRF_NOHASSLE))
			continue;
		if (!IS_NPC(ch) && IS_NPC(vict) && AFF_FLAGGED(vict, AFF_CHARM))
			continue;
		if (spellnum == SPELL_EARTHQUAKE && GET_POSITION(vict) == POS_FLYING)
			continue;
		if (spellnum == SONG_SONIC_DISRUPTION && IS_UNDEAD(vict)) {
			continue;
		}
		if (spellnum == SONG_DIRGE && !IS_UNDEAD(vict)) {
			continue;
		}
        if (spellnum == SONG_LICHS_LYRICS && !LIFE_FORM(vict)) {
            continue;
        }

        if (vict != ch && !ok_to_attack(ch, *it, false)) {
            if (SPELL_IS_PSIONIC(spellnum)) {
                send_to_char(ch, "The Universal Psyche decends on your "
                             "mind and renders you powerless!\r\n");
                act("$n concentrates for an instant, and is suddenly "
                    "thrown into mental shock!", false, ch, 0, 0, TO_ROOM);
            }
            else if (SPELL_IS_PHYSICS(spellnum)) {
                send_to_char(ch, "The Supernatural Reality prevents you "
                             "from twisting nature in that way!\r\n");
                act("$n attempts to violently alter reality, but is "
                    "restrained by the whole of the universe.", false,
                    ch, 0, 0, TO_ROOM);
            }
            else if (SPELL_IS_BARD(spellnum)) {
                send_to_char(ch, "Your voice is stifled!\r\n");
                act("$n attempts to sing a violent song, but is "
                    "restrained by the whole of the universe.", false,
                    ch, 0, 0, TO_ROOM);
            }
            else {
                send_to_char(ch, "A flash of white light fills "
                             "the room, dispelling your violent "
                             "magic!\r\n");
                act("White light from no particular source suddenly "
                    "fills the room, then vanishes.", false,
                    ch, 0, 0, TO_ROOM);
            }
            return 0;
        }
    }

    it = ch->in_room->people.begin();
	for (; it != ch->in_room->people.end(); ++it) {
        struct creature *vict = *it;
		// skips:
		//          caster
		//          nohassle-flagged players (imms)
		//          charmed mobs
		//          flying chars if spell is earthquake
		//			undead chars if spell is sonic disruption
		//			non-undead chars if spell is dirge
		//          non-living chars if spell is lich's lyric

		if (vict == ch)
			continue;
		if (!IS_NPC(vict) && PRF_FLAGGED(vict, PRF_NOHASSLE))
			continue;
		if (!IS_NPC(ch) && IS_NPC(vict) && AFF_FLAGGED(vict, AFF_CHARM))
			continue;
		if (spellnum == SPELL_EARTHQUAKE && GET_POSITION(vict) == POS_FLYING)
			continue;
		if (spellnum == SONG_SONIC_DISRUPTION && IS_UNDEAD(vict)) {
			continue;
		}
		if (spellnum == SONG_DIRGE && !IS_UNDEAD(vict)) {
			continue;
		}
        if (spellnum == SONG_LICHS_LYRICS && !LIFE_FORM(vict)) {
            continue;
        }

		if (spellnum == SPELL_MASS_HYSTERIA) {
			call_magic(ch, vict, 0, NULL, SPELL_FEAR, level, CAST_PSIONIC);
			continue;
		}

		if (spellnum == SPELL_FISSION_BLAST
			&& !(mag_savingthrow(vict, level, SAVING_PHY))) {
			add_rad_sickness(vict, level);
		}
		if (spellnum == SONG_SONIC_DISRUPTION) { //drop things
			struct obj_data *obj = NULL;
            if (IS_NPC(vict) && vict->mob_specials.shared->func == vendor) {
                continue;
            }
			if ((random_number_zero_low(3 + (level / 4)) + 3) > GET_DEX(vict) &&
				!is_arena_combat(ch, *it) && (obj = vict->carrying)) { //assignment to obj
				while (obj) {
					if (can_see_object(*it, obj) && !IS_OBJ_STAT(obj, ITEM_NODROP))
						break;
					obj = obj->next_content;
				}
				if (obj) {
					act("$p is blasted from the hands of $N by the powerful sonic waves!",
					true, ch, obj, *it,	TO_ROOM);
					act("$p is blasted from your hands by the powerful sonic waves!",
					true, ch, obj, *it, TO_VICT);
					act("$p is blasted from the hands of $N by your powerful sonic waves!",
					true, ch, obj, *it, TO_CHAR);
					obj_from_char(obj);
					obj_to_room(obj, vict->in_room);
				}
			}
		}
        if (spellnum == SONG_LICHS_LYRICS) {
            mag_affects(level, ch, *it, 0, SONG_LICHS_LYRICS, savetype);
        }
		int retval = mag_damage(level, ch, vict, spellnum, 1);
		return_value |= retval;
		if (retval == 0) {
			if (spellnum == SPELL_EARTHQUAKE && number(10, 20) > GET_DEX(ch)) {
				send_to_char(ch, "You stumble and fall to the ground!\r\n");
				GET_POSITION(ch) = POS_SITTING;
			}
		}
	}
	if (to_next_room) {
		was_in = ch->in_room;
		for (int door = 0; door < NUM_OF_DIRS; door++) {
			if (CAN_GO(ch, door) && ch->in_room != EXIT(ch, door)->to_room) {
				ch->in_room = was_in->dir_option[door]->to_room;
				act(to_next_room, false, ch, 0, 0, TO_ROOM);
				adjoin_room = ch->in_room;
				ch->in_room = was_in;

				struct creatureList_iterator it = adjoin_room->people.begin();
				for (; it != adjoin_room->people.end(); ++it) {
                    struct creature *vict = *it;
					if (!IS_NPC(vict) && GET_LEVEL(vict) >= LVL_AMBASSADOR)
						continue;
					if (!IS_NPC(ch) && IS_NPC(vict)
						&& AFF_FLAGGED(vict, AFF_CHARM))
						continue;
					if (spellnum == SPELL_EARTHQUAKE
						&& GET_POSITION(vict) == POS_FLYING)
						continue;
					if (spellnum == SPELL_EARTHQUAKE
						&& GET_POSITION(vict) == POS_STANDING
						&& number(10, 20) > GET_DEX(vict)) {
						send_to_char(vict, "You stumble and fall to the ground!\r\n");
						GET_POSITION(vict) = POS_SITTING;
					}
				}
			}
		}
	}
	return return_value;
}

/*
  Every spell which summons/gates/conjours a mob comes through here.
  None of these spells are currently implemented in Circle 3.0; these
  were taken as examples from the JediMUD code.  Summons can be used
  for spells like clone, ariel servant, etc.
*/

static const char *mag_summon_msgs[] = {
	"\r\n",
	"$n makes a strange magical gesture; you feel a strong breeze!\r\n",
	"$n animates a corpse!\r\n",
	"$N appears from a cloud of thick blue smoke!\r\n",
	"$N appears from a cloud of thick green smoke!\r\n",
	"$N appears from a cloud of thick red smoke!\r\n",
	"$N disappears in a thick black cloud!\r\n"
		"As $n makes a strange magical gesture, you feel a strong breeze.\r\n",
	"As $n makes a strange magical gesture, you feel a searing heat.\r\n",
	"As $n makes a strange magical gesture, you feel a sudden chill.\r\n",
	"As $n makes a strange magical gesture, you feel the dust swirl.\r\n",
	"$n magically divides!\r\n",
	"$n animates a corpse!\r\n"
};

static const char *mag_summon_fail_msgs[] = {
	"\r\n",
	"There are no such creatures.\r\n",
	"Uh oh...\r\n",
	"Oh dear.\r\n",
	"This is bad...\r\n",
	"The elements resist!\r\n",
	"You failed.\r\n",
	"There is no corpse!\r\n"
};

#define MOB_MONSUM_I        130
#define MOB_MONSUM_II        140
#define MOB_MONSUM_III        150
#define MOB_GATE_I        160
#define MOB_GATE_II        170
#define MOB_GATE_III        180
#define MOB_ELEMENTAL_BASE    110
#define MOB_CLONE        69
#define MOB_ZOMBIE        101
#define MOB_AERIALSERVANT    109

void
mag_summons(int level __attribute__ ((unused)),
            struct creature *ch,
            struct obj_data *obj,
            int spellnum,
            int savetype __attribute__ ((unused)))
{
	struct creature *mob = NULL;
	struct obj_data *tobj, *next_obj;
	int pfail = 0;
	int fmsg = 0;
	int num = 1;
	int a, i;
	int mob_num = 0;
	int handle_corpse = 0;

	if (ch == NULL)
		return;

	switch (spellnum) {
	case SPELL_ANIMATE_DEAD:
		if ((obj == NULL) || (GET_OBJ_TYPE(obj) != ITEM_CONTAINER) ||
			(!GET_OBJ_VAL(obj, 3))) {
			act(mag_summon_fail_msgs[7], false, ch, 0, 0, TO_CHAR);
			return;
		}
		handle_corpse = 1;
		mob_num = MOB_ZOMBIE;
		a = number(0, 5);
		if (a)
			mob_num++;
		pfail = 8;
		break;

	default:
		return;
	}

	if (AFF_FLAGGED(ch, AFF_CHARM)) {
		send_to_char(ch, "You are too giddy to have any followers!\r\n");
		return;
	}
	if (number(0, 101) < pfail) {
		send_to_char(ch, "%s", mag_summon_fail_msgs[fmsg]);
		return;
	}
	for (i = 0; i < num; i++) {
		mob = read_mobile(mob_num);
		char_to_room(mob, ch->in_room,false);
		IS_CARRYING_W(mob) = 0;
		IS_CARRYING_N(mob) = 0;
		SET_BIT(AFF_FLAGS(mob), AFF_CHARM);
		add_follower(mob, ch);
		act(mag_summon_msgs[fmsg], false, ch, 0, mob, TO_ROOM);
		if (spellnum == SPELL_CLONE) {
			strcpy(GET_NAME(mob), GET_NAME(ch));
			strcpy(mob->player.short_descr, GET_NAME(ch));
		}
	}
	if (handle_corpse) {
		for (tobj = obj->contains; tobj; tobj = next_obj) {
			next_obj = tobj->next_content;
			obj_from_obj(tobj);
			obj_to_char(tobj, mob);
		}
		extract_obj(obj);
	}
}

void
mag_points(int level,
           struct creature *ch,
           struct creature *victim,
           int *dir __attribute__ ((unused)),
           int spellnum,
           int savetype)
{
	int hit = 0;
	int move = 0;
	int mana = 0;
	int align = 0;
	int hunger = 0;
	int thirst = 0;
	const char *to_vict = NULL, *to_room = NULL;
	int skill;

	if (victim == NULL)
		return;

	if (savetype == SAVING_ROD || savetype == SAVING_NONE)
		skill = 100;
	else
		skill = CHECK_SKILL(ch, spellnum);
	switch (spellnum) {
	case SPELL_CURE_LIGHT:
		hit = dice(1, 8) + 1 + (level / 4);
        hit = (skill * hit) / 100;
		to_vict = "You feel better.";
		break;
	case SPELL_CURE_CRITIC:
		hit = dice(3, 8) + 3 + (level / 4);
        hit = (skill * hit) / 100;
		to_vict = "You feel a lot better!";
		break;
	case SPELL_HEAL:
		hit = 50 + dice(3, level);
		hit = (skill * hit) / 100;
		to_vict = "A warm feeling floods your body.";
		break;
	case SPELL_GREATER_HEAL:
		hit = 100 + dice(5, level);
        hit = (skill * hit) / 100;
		to_vict = "A supreme warm feeling floods your body.";
		break;
	case SPELL_RESTORATION:
		hit = MIN(GET_MAX_HIT(victim), (level * 32));
		if (GET_COND(victim, FULL) >= 0)
			GET_COND(victim, FULL) = 24;
		if (GET_COND(victim, THIRST) >= 0)
			GET_COND(victim, THIRST) = 24;
		to_vict = "You feel totally healed!";
		break;
	case SPELL_REFRESH:
		move = 50 + number(0, level) + GET_WIS(ch);
		move = (skill * move) / 100;
		to_vict = "You feel refreshed!";
		break;
	case SPELL_MANA_RESTORE:
		mana = dice(level, 10);
		to_vict = "You feel your spiritual energies replenished.";
		to_room = "$n is surrounded by a brief aura of blue light.";
		break;
		/* psionic triggers */
	case SPELL_PSYCHIC_CONDUIT:
		mana = level + (CHECK_SKILL(ch, SPELL_PSYCHIC_CONDUIT) / 20) +
			number(0, GET_WIS(ch)) + (GET_REMORT_GEN(ch) * 4);
		break;

	case SPELL_SATIATION:
		hunger = dice(3, MIN(3, (1 + (level / 4))));
		to_vict = "You feel satiated.";
		break;

	case SPELL_QUENCH:
		thirst = dice(3, MIN(3, (1 + (level / 4))));
		to_vict = "Your thirst is quenched.";
		break;

	case SPELL_WOUND_CLOSURE:
		hit = dice(3, 6 + (CHECK_SKILL(ch, SPELL_WOUND_CLOSURE) / 32)) +
			number(level / 2, level);
		hit += GET_REMORT_GEN(ch) * 4;
		hit +=
			((CHECK_SKILL(ch, SPELL_WOUND_CLOSURE) - LEARNED(ch)) * hit) / 100;
		to_vict = "Some of your wounds seal as you watch.";
		break;

	case SPELL_ENDURANCE:
		move = level * 2;
		break;

	case SPELL_CELL_REGEN:
		hit = dice(4, 6 + (CHECK_SKILL(ch, SPELL_CELL_REGEN) / 16)) +
			number(level / 2, level * 2);
		hit = (skill * hit) / 100;
		break;

    /* physic stuff */
	case SPELL_CAPACITANCE_BOOST:
		move = level * 2;
		break;

    // Bard stuff
    case SONG_DRIFTERS_DITTY:
        move = level * 2;
        break;

    case SONG_RHAPSODY_OF_REMEDY:
        hit = dice(10, 1 + get_skill_bonus(ch, SONG_RHAPSODY_OF_REMEDY) / 4);
        hit += ((get_skill_bonus(ch, SONG_RHAPSODY_OF_REMEDY) / 10) - (25 - GET_CHA(ch))) * 4;
        to_vict = "Your wounds fade along with the last notes of the music.";
        break;

	/** non-pc spells **/
	case SPELL_ESSENCE_OF_EVIL:
		if (!IS_GOOD(victim)) {
			to_vict = "You feel the essence of evil burning in your soul.";
			align = -(level * 2);
		}
		break;
	case SPELL_ESSENCE_OF_GOOD:
		if (!IS_EVIL(victim)) {
			to_vict = "You feel the essence of goodness bathe your soul.";
			align = +(level * 2);
		}
		break;
	default:
		errlog("spellnum %d in mag_points.", spellnum);
		return;
	}

	if (hit > 0 && savetype == SAVING_SPELL && SPELL_IS_DIVINE(spellnum)) {
		int alignment = GET_ALIGNMENT(ch);
		if (alignment < 0)
			alignment *= -1;

		hit += (hit * alignment) / 3000;
		if (IS_EVIL(ch)) {
			hit /= 2;
		}
	}

	if (hit && affected_by_spell(victim, SPELL_BLACKMANTLE)) {
		hit = 0;
		to_vict = NULL;
		send_to_char(ch, "Your blackmantle absorbs the healing!\r\n");
	}

	GET_HIT(victim) = MIN(GET_MAX_HIT(victim), GET_HIT(victim) + hit);
	GET_MOVE(victim) = MIN(GET_MAX_MOVE(victim), GET_MOVE(victim) + move);
	GET_MANA(victim) = MIN(GET_MAX_MANA(victim), GET_MANA(victim) + mana);
	GET_ALIGNMENT(victim) =
		MAX(MIN(GET_ALIGNMENT(victim) + align, 1000), -1000);
	if (hunger)
		gain_condition(victim, FULL, hunger);
	if (thirst)
		gain_condition(victim, THIRST, thirst);
	if (to_vict)
		act(to_vict, false, ch, 0, victim, TO_VICT);
	if (to_room)
		act(to_room, false, ch, 0, victim, TO_NOTVICT);
}

void
mag_unaffects(int level, struct creature *ch, struct creature *victim,
	int spellnum, int type)
{
	int spell = 0, spell2 = 0, spell3 = 0, spell4 = 0, spell5 = 0;
	const char *to_vict = NULL, *to_room = NULL, *to_vict2 = NULL, *to_room2 = NULL,
		*to_vict3 = NULL, *to_room3 = NULL, *to_vict4 = NULL, *to_room4 = NULL,
		*to_vict5 = NULL, *to_room5 = NULL;
	struct affected_type *aff = NULL, *next_aff = NULL;

	if (victim == NULL)
		return;

	if (spell_info[spellnum].violent && mag_savingthrow(victim, level, type)) {
		act("You resist the affects!", false, ch, 0, victim, TO_VICT);
		if (ch != victim)
			act("$N resists the affects!", false, ch, 0, victim, TO_CHAR);
		WAIT_STATE(ch, PULSE_VIOLENCE);
		return;
	}

	switch (spellnum) {
	case SPELL_CURE_BLIND:
	case SPELL_HEAL:
	case SPELL_GREATER_HEAL:
		spell = SPELL_BLINDNESS;
		to_vict = "Your vision returns!";
		to_room = "There's a momentary gleam in $n's eyes.";
		spell2 = SKILL_GOUGE;
		to_vict2 = "Your vision returns!";
		to_room2 = "There's a momentary gleam in $n's eyes.";
		spell3 = SKILL_HAMSTRING;
		to_vict3 = "The wound on your leg closes!";
		to_room3 = "The gaping wound on $n's leg closes.";
		spell4 = TYPE_MALOVENT_HOLYTOUCH;
		to_vict4 = "Your vision returns!";
		to_room4 = "There's a momentary gleam in $n's eyes.";
		if (spellnum == SPELL_GREATER_HEAL) {
			spell5 = SPELL_POISON;
			to_vict5 = "A warm feeling runs through your body!";
			to_room5 = "$n looks better.";
		}
		break;
	case SPELL_REMOVE_POISON:
		spell = SPELL_POISON;
		to_vict = "A warm feeling runs through your body!";
		to_room = "$n looks better.";
		break;
	case SPELL_ANTIBODY:
		spell = SPELL_POISON;
		if (GET_LEVEL(ch) > 30) {
			spell2 = SPELL_SICKNESS;
			REMOVE_BIT(AFF3_FLAGS(victim), AFF3_SICKNESS);
			to_vict2 = "Your sickness subsides.  What intense relief.";
			to_room2 = "$n looks better.";
		}
		to_vict = "A warm feeling runs through your body!";
		to_room = "$n looks better.";
		break;
	case SPELL_REMOVE_CURSE:
		spell = SPELL_CURSE;
		to_vict = "You don't feel quite so unlucky.";
		break;
	case SPELL_STONE_TO_FLESH:
		REMOVE_BIT(AFF2_FLAGS(victim), AFF2_PETRIFIED);
		spell = SPELL_PETRIFY;
		spell2 = SPELL_STONESKIN;
		to_vict = "You feel freed as your body changes back to flesh.";
		to_room = "$n's body changes back from stone.";
		to_vict2 = tmp_strdup(spell_wear_off_msg[SPELL_STONESKIN]);
		break;
	case SPELL_REMOVE_SICKNESS:
		REMOVE_BIT(AFF3_FLAGS(victim), AFF3_SICKNESS);
		spell = SPELL_SICKNESS;
		spell2 = TYPE_RAD_SICKNESS;
		to_vict = "Your sickness subsides.  What intense relief.";
		to_room = "$n looks better.";
		break;
		/* psionic skills */
	case SPELL_NULLPSI:
		if (victim->affected) {
			send_to_char(victim, "Your feel the effects of a psychic purge.\r\n");
			for (aff = victim->affected; aff; aff = next_aff) {
				next_aff = aff->next;
				if (SPELL_IS_PSIONIC(aff->type)) {
					if (aff->level < number(level / 2, level * 2))
						affect_remove(victim, aff);
				}
			}
		}
		break;

	case SPELL_RELAXATION:
	case SPELL_MELATONIC_FLOOD:
		spell = SPELL_MOTOR_SPASM;
		spell2 = SPELL_ADRENALINE;
		spell3 = SKILL_BERSERK;
		break;
	case SPELL_INTELLECT:
		spell = SPELL_CONFUSION;
		to_vict = "Your haze of confusion dissipates!";
		break;
	case SPELL_CONFIDENCE:
		spell = SPELL_FEAR;
		to_vict = "You feel less afraid.";
		break;
	case SPELL_PSIONIC_SHATTER:
		if (!AFF3_FLAGGED(victim, AFF3_PSISHIELD)) {
			act("$N is not protected by a psionic shield.",
				false, ch, 0, victim, TO_CHAR);
			return;
		}
		spell = SPELL_PSISHIELD;
		REMOVE_BIT(AFF3_FLAGS(victim), AFF3_PSISHIELD);
		to_vict = "Your psionic shield shatters!";
		act("$N's psionic shield shatters!", false, ch, 0, victim, TO_CHAR);
		break;

		// physic spells for mag_unaffects
	case SPELL_NULLIFY:
		if (victim->affected) {
			for (aff = victim->affected; aff; aff = next_aff) {
				next_aff = aff->next;
				if (SPELL_IS_PHYSICS(aff->type)) {
					if (aff->level < number(level / 2, level * 2))
						affect_remove(victim, aff);
				}
			}
			send_to_char(victim,
				"Your physical states relax and resume their 'normal' states.\r\n");
			act("$n appears more 'physically normal', somehow...", true,
				victim, 0, 0, TO_ROOM);

		}
		break;

	case SPELL_CHEMICAL_STABILITY:
		spell = SPELL_ACIDITY;
		spell2 = SPELL_ACID_BREATH;	// acid breath
		to_vict = "You feel less acidic  What a relief!\r\n";
		break;

	case SONG_ALRONS_ARIA:
		spell = SPELL_FEAR;
		to_vict = "You feel less afraid.";
		break;

    case SONG_LUSTRATION_MELISMA:
        spell = SPELL_BLINDNESS;
        spell2 = SPELL_POISON;
        spell3 = SPELL_SICKNESS;
        to_vict = "You feel cleansed of all your illnesses.";
        break;

    case SONG_HYMN_OF_PEACE:
        spell = SKILL_BERSERK;
        to_vict = "You feel your rage calmed.";
        break;

	default:
		errlog("unknown spellnum %d passed to mag_unaffects",
			spellnum);
		return;
	}

	if ((spell != 0 && !affected_by_spell(victim, spell)) &&
		(spell2 != 0 && !affected_by_spell(victim, spell2)) &&
		(spell4 != 0 && !affected_by_spell(victim, spell4)) &&
		(spell3 != 0 && !affected_by_spell(victim, spell3))) {
		if (!(spell_info[spellnum].routines - MAG_UNAFFECTS))
			send_to_char(ch, "%s", NOEFFECT);
		return;
	}
	if (spell && affected_by_spell(victim, spell)) {
		affect_from_char(victim, spell);
		if (to_vict != NULL)
			act(to_vict, false, victim, 0, ch, TO_CHAR);
		if (to_room != NULL)
			act(to_room, true, victim, 0, ch, TO_ROOM);
	}
	if (spell2 && affected_by_spell(victim, spell2)) {
		affect_from_char(victim, spell2);
		if (to_vict2 != NULL)
			act(to_vict2, false, victim, 0, ch, TO_CHAR);
		if (to_room2 != NULL)
			act(to_room2, true, victim, 0, ch, TO_ROOM);
	}
	if (spell3 && affected_by_spell(victim, spell3)) {
		affect_from_char(victim, spell3);
		if (to_vict3 != NULL)
			act(to_vict3, false, victim, 0, ch, TO_CHAR);
		if (to_room3 != NULL)
			act(to_room3, true, victim, 0, ch, TO_ROOM);
	}
	if (spell4 && affected_by_spell(victim, spell4)) {
		affect_from_char(victim, spell4);
		if (to_vict4 != NULL)
			act(to_vict4, false, victim, 0, ch, TO_CHAR);
		if (to_room4 != NULL)
			act(to_room4, true, victim, 0, ch, TO_ROOM);
	}
	if (spell5 && affected_by_spell(victim, spell5)) {
		affect_from_char(victim, spell5);
		if (to_vict5 != NULL)
			act(to_vict5, false, victim, 0, ch, TO_CHAR);
		if (to_room5 != NULL)
			act(to_room5, true, victim, 0, ch, TO_ROOM);
	}
}

void
mag_alter_objs(int level, struct creature *ch, struct obj_data *obj,
	int spellnum, int savetype __attribute__ ((unused)))
{
	const char *to_char = NULL;
	const char *to_room = NULL;
    struct tmp_obj_affect oaf[5];
    int dur_mode, val_mode, aff_mode;
    int max_repulsions=0;
    int max_attractions=0;
    struct affected_type af;

    af.is_instant = 0;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.aff_index = 0;
    af.owner = GET_IDNUM(ch);
    af.duration = 0;
    af.level = GET_LEVEL(ch);
    af.type = 0;

    dur_mode = val_mode = aff_mode = AFF_NOOP;

    memset(&oaf, 0x0, sizeof(oaf));

	if (obj == NULL)
		return;

    switch (spellnum) {
    case SPELL_GREATER_INVIS:
    case SPELL_INVISIBLE:
        if (!IS_OBJ_STAT(obj, ITEM_NOINVIS | ITEM_INVISIBLE)) {
            SET_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);
            to_char = "$p turns invisible.";
        }
        break;
    case SPELL_BLUR:
        if (!IS_OBJ_STAT(obj, ITEM_BLURRED)) {
            SET_BIT(obj->obj_flags.extra_flags, ITEM_BLURRED);
            to_char = "The image of $p becomes blurred.";
        }
        break;
    case SPELL_CURSE:
        if (!IS_OBJ_STAT(obj, ITEM_NODROP)) {
            SET_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
            to_char = "$p becomes cursed.";
        }
        break;

    case SPELL_REMOVE_CURSE:
        if (IS_OBJ_STAT2(obj, ITEM2_CURSED_PERM) &&
            GET_LEVEL(ch) < LVL_ELEMENT) {
            to_char = "$p vibrates fiercely, then stops.";
        } else if (IS_OBJ_STAT(obj, ITEM_NODROP)) {
            REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
            to_char = "$p briefly glows blue.";
        }
        break;
    case SPELL_ENCHANT_WEAPON:
    case SPELL_ENCHANT_ARMOR:
    case SPELL_GREATER_ENCHANT:
    case SPELL_MAGICAL_VESTMENT:
        if (!IS_OBJ_STAT(obj, ITEM_MAGIC)) {
            SET_BIT(obj->obj_flags.extra_flags, ITEM_GLOW);
            SET_BIT(obj->obj_flags.extra_flags, ITEM_MAGIC);
        }
        break;

        // physic mag_alter_objs items
    case SPELL_HALFLIFE:
        if (!IS_OBJ_STAT(obj, ITEM_MAGIC_NODISPEL) && !OBJ_IS_RAD(obj)) {
            SET_BIT(obj->obj_flags.extra2_flags, ITEM2_RADIOACTIVE);
            to_char = "$p begins to emit radioactive decay particles.";
        }
        break;

    case SPELL_ATTRACTION_FIELD: {
        {
            tmp_obj_affect *af = obj->affectedBySpell(SPELL_ITEM_REPULSION_FIELD);
            if( af != NULL ) {
                struct affected_type *ra;

                act(item_wear_off_msg[af->type], false, ch, obj, NULL, TO_CHAR);
                obj->removeAffect( af );
                for (ra = ch->affected; ra; ra = ra->next) {
                    if (ra->type == SPELL_ITEM_REPULSION_FIELD) {
                        affect_remove(ch, ra);
                        break;
                    }
                }
            }
        }
        //range will be 200-2700, divided by 400 = 0-6, clamped to 1-6
        max_attractions = (get_skill_bonus(ch, SPELL_ATTRACTION_FIELD)*GET_INT(ch)+200)/400;
        max_attractions = MAX(max_attractions,1);

        if (obj->affectedBySpell(SPELL_ITEM_ATTRACTION_FIELD)) {
            to_char = "$p already has an attraction field!";
            break;
        } else if (count_affect(ch, SPELL_ITEM_ATTRACTION_FIELD) >= max_attractions) {
            to_char = "You will need to wait awhile before you can do this again.";
            break;
        }

        oaf[0].level = get_skill_bonus(ch, SPELL_ATTRACTION_FIELD);
        af.level = oaf[0].level;
        oaf[0].type = SPELL_ITEM_ATTRACTION_FIELD;
        af.type = oaf[0].type;
        oaf[0].duration = 20 + get_skill_bonus(ch, SPELL_ATTRACTION_FIELD) / 2;
        af.duration = oaf[0].duration;

        if (GET_OBJ_TYPE(obj) == ITEM_WEAPON || GET_OBJ_TYPE(obj) == ITEM_ENERGY_GUN) {
			int levelBonus = get_skill_bonus(ch, SPELL_ATTRACTION_FIELD);
			int hitroll = dice( 3, 2 );

			if( levelBonus > 50 && random_binary() )
				hitroll += 1;
			if( levelBonus > 75 && random_binary() )
				hitroll += 1;
			if( levelBonus > 95 && random_binary() )
				hitroll += 1;

			oaf[0].affect_loc[0] = APPLY_HITROLL;
			oaf[0].affect_mod[0] = hitroll;
        } else {
			oaf[0].affect_loc[0] = APPLY_AC;
			oaf[0].affect_mod[0] = (level / 8) + number(1, 2);
		}

        affect_to_char(ch, &af);
        to_char = "$p begins to emit an attraction field.";
        break;
    }
    case SPELL_REPULSION_FIELD:
        {
            tmp_obj_affect *af = obj->affectedBySpell(SPELL_ITEM_ATTRACTION_FIELD);
            if( af != NULL ) {
                struct affected_type *ra;

                act(item_wear_off_msg[af->type], false, ch, obj, NULL, TO_CHAR);
                obj->removeAffect( af );
                 for (ra = ch->affected; ra; ra = ra->next) {
                    if (ra->type == SPELL_ITEM_ATTRACTION_FIELD) {
                        affect_remove(ch, ra);
                        break;
                    }
                }
            }
        }
        //range will be 200-2700, divided by 400 = 0-6, clamped to 1-6
        max_repulsions = (get_skill_bonus(ch, SPELL_REPULSION_FIELD)*GET_INT(ch)+200)/400;
        max_repulsions = MAX(max_repulsions,1);

        if (obj->affectedBySpell(SPELL_ITEM_REPULSION_FIELD)) {
            to_char = "$p already has an repulsion field!";
            break;
        } else if (count_affect(ch, SPELL_ITEM_REPULSION_FIELD) >= max_repulsions) {
            to_char = "You will need to wait awhile before you can do this again.";
            break;
        }

        oaf[0].level = get_skill_bonus(ch, SPELL_REPULSION_FIELD);
        af.level = oaf[0].level;
        oaf[0].type = SPELL_ITEM_REPULSION_FIELD;
        af.type = oaf[0].type;
        oaf[0].duration = 20 + get_skill_bonus(ch, SPELL_REPULSION_FIELD) / 2;
        af.duration = oaf[0].duration;

        if (GET_OBJ_TYPE(obj) == ITEM_WEAPON) {
			int levelBonus = get_skill_bonus(ch, SPELL_REPULSION_FIELD);
			int hitroll = dice( 3, 2 );

			if( levelBonus > 50 && random_binary() )
				hitroll += 1;
			if( levelBonus > 75 && random_binary() )
				hitroll += 1;
			if( levelBonus > 95 && random_binary() )
				hitroll += 1;

			oaf[0].affect_loc[0] = APPLY_HITROLL;
			oaf[0].affect_mod[0] = -hitroll;
        } else {
			oaf[0].affect_loc[0] = APPLY_AC;
			oaf[0].affect_mod[0] = -((level / 8) + number(1, 2));
		}

        affect_to_char(ch, &af);
        to_char = "$p begins to emit an repulsion field.";
        break;
    case SPELL_TRANSMITTANCE:
        if (!IS_OBJ_STAT(obj,
                ITEM_NOINVIS | ITEM_INVISIBLE | ITEM_TRANSPARENT)) {
            SET_BIT(obj->obj_flags.extra_flags, ITEM_TRANSPARENT);
            to_char = "$p becomes transparent.";
        }
        break;

    case SPELL_DENSIFY:
        obj->modifyWeight(level + GET_INT(ch));
        to_char = "$p becomes denser.";
        break;

    case SPELL_LATTICE_HARDENING:
        if (IS_OBJ_STAT3(obj, ITEM3_LATTICE_HARDENED)) {
            act("$p's molecular lattice has already been strengthened.",
                true, ch, obj, 0, TO_CHAR);
            return;
        }
        if (obj->obj_flags.max_dam < 0 || obj->obj_flags.damage < 0) {
            act("$p's molecular lattice cannot be strengthened.",
                true, ch, obj, 0, TO_CHAR);
            return;
        }
        int increase;
        if (GET_CLASS(ch) == CLASS_PHYSIC)
            increase = obj->obj_flags.max_dam * (GET_LEVEL(ch) +
                (GET_REMORT_GEN(ch) * 2)) / 200;
        else
            increase = obj->obj_flags.max_dam * (GET_LEVEL(ch) +
                (GET_REMORT_GEN(ch) * 2)) / 240;
        obj->obj_flags.max_dam += increase;
        obj->obj_flags.damage =
            MIN(obj->obj_flags.damage + increase, obj->obj_flags.max_dam);
        SET_BIT(obj->obj_flags.extra3_flags, ITEM3_LATTICE_HARDENED);
        to_char = "$p's molecular lattice strengthens.";
        if (GET_LEVEL(ch) >= LVL_AMBASSADOR && !isname("imm", obj->aliases)) {
            char *new_aliases = strdup(tmp_sprintf("%s imm %shardening", obj->aliases, GET_NAME(ch)));
            free(obj->aliases);
            obj->aliases = new_aliases;
            mudlog(GET_LEVEL(ch), CMP, true,
                "ENCHANT: %s lattice hardened by %s.",
                obj->name, GET_NAME(ch));
        }
        break;

    case SPELL_WARDING_SIGIL:

        // can't sigilize money
        if (IS_OBJ_TYPE(obj, ITEM_MONEY)) {
            to_char = "Slaaaave.... to the traffic light!";
            break;
        }
        // item has a sigil, try to remove it
        if (GET_OBJ_SIGIL_IDNUM(obj)) {
            // sigil was planted by someone else
            if (GET_OBJ_SIGIL_IDNUM(obj) != GET_IDNUM(ch)) {
                if (level <= GET_OBJ_SIGIL_LEVEL(obj)) {
                    to_char = "You fail to remove the sigil.";
                    break;
                }
            }
            GET_OBJ_SIGIL_IDNUM(obj) = 0;
            GET_OBJ_SIGIL_LEVEL(obj) = 0;
            to_char = "You have dispelled the sigil.";
        }
        // there is no sigil, add one
        else {
            GET_OBJ_SIGIL_IDNUM(obj) = GET_IDNUM(ch);
            GET_OBJ_SIGIL_LEVEL(obj) = level;
            to_char = "A sigil of warding has been magically etched upon $p.";
            to_room = "A glowing sigil appears upon $p, then fades.";
        }
        break;
    case SPELL_ENVENOM:
        if (!(GET_OBJ_TYPE(obj) == ITEM_WEAPON)) {
            to_char = "You can only envenom weapons.";
            break;
        }

        if (obj->affectedBySpell(SPELL_ENVENOM)) {
            to_char = "That weapon is already envenomed!";
            break;
        }
        oaf[0].level = get_skill_bonus(ch, SPELL_ENVENOM);
        oaf[0].type = SPELL_ENVENOM;
        oaf[0].duration = get_skill_bonus(ch, SPELL_ENVENOM) / 5;
        oaf[0].val_mod[0] = SPELL_POISON - GET_OBJ_VAL(obj, 0);
        oaf[0].extra_mod = ITEM_MAGIC;
        oaf[0].extra_index = 1;
        oaf[1].level = oaf[0].level;
        oaf[1].type = oaf[0].type;
        oaf[1].duration = oaf[0].duration;
        oaf[1].extra_mod = ITEM2_CAST_WEAPON;
        oaf[1].extra_index = 2;
        to_char = "$p begins to drip poison.";
        break;

    case SPELL_ELEMENTAL_BRAND: {
        int num = number(0, 3);

        struct tmp_obj_affect *af;

        if (!(GET_OBJ_TYPE(obj) == ITEM_WEAPON)) {
            to_char = "You can only brand weapons.";
            break;
        }
        af = obj->affectedBySpell(SPELL_ELEMENTAL_BRAND);
        oaf[0].level = get_skill_bonus(ch, SPELL_ELEMENTAL_BRAND);
        oaf[0].type = SPELL_ELEMENTAL_BRAND;
        oaf[0].duration = get_skill_bonus(ch, SPELL_ELEMENTAL_BRAND) / 2;
        oaf[1].level = oaf[0].level;
        oaf[1].type = oaf[0].type;
        oaf[1].duration = oaf[0].duration;

        switch (num) {
            case 0: // Earth brand adds maxdam to the weapon
                for(; af != NULL; af = af->next) {
                    if (af->type != SPELL_ELEMENTAL_BRAND)
                        continue;
                    if (af->dam_mod) {
                        to_char = "Your branding fails.";
                    }
                }
                if (to_char != NULL)
                    break;

                oaf[0].dam_mod = get_skill_bonus(ch, SPELL_ELEMENTAL_BRAND) *
                                    GET_INT(ch) * 2;
                oaf[0].maxdam_mod = oaf[0].dam_mod;
                oaf[0].extra_mod = ITEM3_REQ_RANGER;
                oaf[0].extra_index = 3;
                oaf[1].extra_mod = ITEM_MAGIC;
                oaf[1].extra_index = 1;
                to_char = "The rune of earth solidifies onto $p.";
                val_mode = AFF_ADD;
                break;

            case 1: // Air brand reduces weight
                for(; af != NULL; af = af->next) {
                    if (af->type != SPELL_ELEMENTAL_BRAND)
                        continue;
                    if (af->weight_mod) {
                        to_char = "Your branding fails.";
                    }
                }
                if (to_char != NULL)
                    break;
                oaf[0].weight_mod =
                    -((int)(obj->getWeight() * 0.10) + get_skill_bonus(ch, SPELL_ELEMENTAL_BRAND) / 25);
                oaf[0].extra_mod = ITEM3_REQ_RANGER;
                oaf[0].extra_index = 3;
                oaf[1].extra_mod = ITEM_MAGIC;
                oaf[1].extra_index = 1;
                to_char = "The rune of air swirls around $p.";
                val_mode = AFF_ADD;
                break;

            case 2: // Fire brand adds item damroll
                for(; af != NULL; af = af->next) {
                    if (af->type != SPELL_ELEMENTAL_BRAND)
                        continue;
                    for (int i = 0; i < MAX_OBJ_AFFECT; i++) {
                        if (af->affect_loc[i] == APPLY_DAMROLL) {
                            to_char = "Your branding fails.";
                        }
                    }
                }
                if (to_char != NULL)
                    break;
                oaf[0].affect_loc[0] = APPLY_DAMROLL;
                oaf[0].affect_mod[0] = MAX(2, GET_REMORT_GEN(ch) / 2);
                oaf[0].extra_mod = ITEM3_REQ_RANGER;
                oaf[0].extra_index = 3;
                oaf[1].extra_mod = ITEM_MAGIC;
                oaf[1].extra_index = 1;
                to_char = "The rune of fire is emblazoned upon $p.";
                aff_mode = AFF_ADD;
                break;

            case 3: // Water brand adds hitroll
                for(; af != NULL; af = af->next) {
                    if (af->type != SPELL_ELEMENTAL_BRAND)
                        continue;
                    for (int i = 0; i < MAX_OBJ_AFFECT; i++) {
                        if (af->affect_loc[i] == APPLY_HITROLL) {
                            to_char = "Your branding fails.";
                        }
                    }
                }
                if (to_char != NULL)
                    break;
                oaf[0].affect_loc[0] = APPLY_HITROLL;
                oaf[0].affect_mod[0] = MAX(2, GET_REMORT_GEN(ch) / 2);
                oaf[0].extra_mod = ITEM3_REQ_RANGER;
                oaf[0].extra_index = 3;
                oaf[1].extra_mod = ITEM_MAGIC;
                oaf[1].extra_index = 1;
                to_char = "The rune of water permeates $p.";
                aff_mode = AFF_ADD;
                break;
        }
        break;
    }
    case SPELL_FLAME_OF_FAITH:
        if (GET_OBJ_TYPE(obj) != ITEM_WEAPON) {
            to_char = "The flame of faith will only burn on a weapon.";
            break;
        }

        if (IS_OBJ_STAT2(obj, ITEM2_ABLAZE) ) {
            to_char = "That weapon is already burning!";
            break;
        }
        oaf[0].level = oaf[0].duration = 20 + (get_skill_bonus(ch, SPELL_FLAME_OF_FAITH)/10);
        oaf[0].type = SPELL_FLAME_OF_FAITH;
        oaf[0].extra_mod = ITEM2_ABLAZE;
        oaf[0].extra_index = 2;

        oaf[1].level = oaf[0].level;
        oaf[1].type = oaf[0].type;
        oaf[1].duration = oaf[0].duration;
        oaf[1].extra_mod = ITEM_MAGIC;
        oaf[1].extra_index = 1;

        aff_mode = AFF_ADD;
        if( IS_EVIL(ch) ) {
            to_char = "A sickening red flame engulfs $p.";
        } else {
            to_char = "$p begins to burn brightly.";
        }
        break;

    default:
        errlog("Unknown spellnum in mag_alter_objs.");
        break;

    }
	if (to_char == NULL)
		send_to_char(ch, "%s", NOEFFECT);
	else
		act(to_char, true, ch, obj, 0, TO_CHAR);

	if (to_room != NULL)
		act(to_room, true, ch, obj, 0, TO_ROOM);

    for (int i = 0; i < 5; i++)
        if (oaf[i].type != 0)
            obj->affectJoin(&oaf[i], dur_mode, val_mode, aff_mode);
}

void
mag_objects(int level, struct creature *ch, struct obj_data *obj,
	int spellnum)
{
	int i;

	switch (spellnum) {
	case SPELL_CREATE_WATER:
		if (!obj) {
			send_to_char(ch, "What do you wish to fill?\r\n");
		} else if (GET_OBJ_TYPE(obj) != ITEM_DRINKCON) {
			act("Your deity deems $p to be unsuitable for water.",
                false, ch, obj, 0, TO_CHAR);
		} else {
            int dry_weight = obj->getWeight() - GET_OBJ_VAL(obj, 1) / 10;
            // Fill with clear water by default
            int resulting_liquid = LIQ_CLEARWATER;


            if (number(0, 101) > GET_SKILL(ch, SPELL_CREATE_WATER)) {
                send_to_char(ch, "Oops!  I wouldn't drink that if I were you...\r\n");
                resulting_liquid = LIQ_SLIME;
            } else if (GET_OBJ_VAL(obj, 1) != 0) {
                act(tmp_sprintf("The %s in $p clears.",
                                drinknames[GET_OBJ_VAL(obj, 2)]),
                    false, ch, obj, 0, TO_CHAR);
            } else {
                act("Your deity fills $p with crystal clear water.",
                    false,ch, obj, 0, TO_CHAR);
            }
            // Fill it to the top
			GET_OBJ_VAL(obj, 1) = GET_OBJ_VAL(obj, 0);
            // Set the liquid type
			GET_OBJ_VAL(obj, 2) = resulting_liquid;
            // Purify it of poison
            GET_OBJ_VAL(obj, 3) = 0;
            // Update the container's weight
            obj->obj_flags.setWeight(dry_weight + GET_OBJ_VAL(obj, 1) / 10);
		}
		break;
	case SPELL_BLESS:
		if (!obj)
			return;
		if (!ch) {
			errlog(" NULL ch passed to mag_objects(). Spellnum BLESS.");
			return;
		}
		if (IS_OBJ_STAT(obj, ITEM_MAGIC | ITEM_BLESS | ITEM_MAGIC_NODISPEL))
			act("The magic is reflected off of $p.", false, ch, obj, 0,
				TO_CHAR);
		for (i = 0; i < MAX_OBJ_AFFECT; i++)
			if (obj->affected[i].location != APPLY_NONE)
				return;
		if (IS_GOOD(ch))
			SET_BIT(GET_OBJ_EXTRA(obj), ITEM_BLESS);
		else if (IS_EVIL(ch))
			SET_BIT(GET_OBJ_EXTRA(obj), ITEM_DAMNED);
		if (GET_OBJ_TYPE(obj) == ITEM_WEAPON) {
			obj->affected[0].location = APPLY_HITROLL;
			obj->affected[0].modifier = 1 + (level / 12) +
				(CHECK_REMORT_CLASS(ch) >= 0) * (level / 18);
			obj->affected[1].location = APPLY_DAMROLL;
			obj->affected[1].modifier = 1 + (level / 18) +
				(CHECK_REMORT_CLASS(ch) >= 0) * (level / 24);
		} else if (GET_OBJ_TYPE(obj) == ITEM_ARMOR) {
			obj->affected[0].location = APPLY_AC;
			obj->affected[0].modifier = -(1 + (level / 12) +
				(CHECK_REMORT_CLASS(ch) >= 0) * (level / 28));
			obj->affected[1].location = APPLY_SAVING_SPELL;
			obj->affected[1].modifier = -(1 + (level / 18) +
				(CHECK_REMORT_CLASS(ch) >= 0) * (level / 28));
		} else
			return;
		if (GET_LEVEL(ch) >= LVL_AMBASSADOR) {
			char *new_aliases = strdup(tmp_sprintf("%s imm %senchant blessed", obj->aliases, GET_NAME(ch)));
            free(obj->aliases);
            obj->aliases = new_aliases;
			mudlog(GET_INVIS_LVL(ch), CMP, true,
				"ENCHANT: Bless. %s by %s.", obj->name,
				GET_NAME(ch));
		}
		break;
	case SPELL_POISON:
		if (GET_OBJ_TYPE(obj) == ITEM_FOOD
			|| GET_OBJ_TYPE(obj) == ITEM_DRINKCON) {
			GET_OBJ_VAL(obj, 3) =
				1 + (level > number(10, 40)) + (level > number(40, 60));
			act("$p is now poisoned.", false, ch, obj, 0, TO_CHAR);
		} else
			send_to_char(ch, "%s", NOEFFECT);
		break;
	}
}

void
mag_creations(int level, struct creature *ch, int spellnum)
{
	struct obj_data *tobj;
	int z;

	if (ch == NULL)
		return;
	level = MAX(MIN(level, LVL_GRIMP), 1);

	switch (spellnum) {
	case SPELL_CREATE_FOOD:
		if (IS_EVIL(ch))
			z = 3177;
		else if (IS_GOOD(ch))
			z = 3176;
		else
			z = 3175;
		break;
	case SPELL_GOODBERRY:
		z = 3179;
		break;
	case SPELL_MINOR_CREATION:
		z = 3009;
		break;
	default:
		send_to_char(ch, "Spell unimplemented, it would seem.\r\n");
		return;
	}

	if (!(tobj = read_object(z))) {
		send_to_char(ch, "I seem to have goofed.\r\n");
		errlog("spell_creations, spell %d, obj %d: obj not found",
			spellnum, z);
		return;
	}
	obj_to_char(tobj, ch);
	act("$n creates $p.", false, ch, tobj, 0, TO_ROOM);
	act("You create $p.", false, ch, tobj, 0, TO_CHAR);
	if (spellnum == SPELL_CREATE_FOOD)
		GET_OBJ_VAL(tobj, 1) = GET_LEVEL(ch);
}

int
mag_exits(int level, struct creature *caster, struct room_data *room,
	int spellnum)
{

	int dir = -1;
	struct room_affect_data rm_aff, o_rm_aff;
	struct room_data *o_room = NULL;

	if (!knock_door) {
		errlog("null knock_door in mag_exits().");
		return 0;
	}

	for (dir = 0; dir < NUM_DIRS; dir++)
		if (room->dir_option[dir] == knock_door)
			break;

	if (dir >= NUM_DIRS) {
		send_to_char(caster, "Funk.\r\n");
		return 0;
	}

	rm_aff.level = level;
	rm_aff.type = dir;

	if ((o_room = room->dir_option[dir]->to_room) &&
		o_room->dir_option[rev_dir[dir]] &&
		room == o_room->dir_option[rev_dir[dir]]->to_room) {
		o_rm_aff.level = rm_aff.level;
		o_rm_aff.type = rev_dir[dir];
	} else
		o_room = NULL;

    const char *aff_desc = NULL;

	switch (spellnum) {
	case SPELL_WALL_OF_THORNS:
		rm_aff.duration = MAX(2, level / 8);
		aff_desc = "   A wall of thorns blocks the way";
		rm_aff.flags = EX_WALL_THORNS;

		break;
	default:
		send_to_char(caster, "Nope.\r\n");
		break;
	}

	rm_aff.description = strdup(tmp_sprintf("%s %s.\r\n", aff_desc, to_dirs[dir]));

	if (o_room) {
		o_rm_aff.description = strdup(tmp_sprintf("%s %s.\r\n", aff_desc, to_dirs[(int)o_rm_aff.type]));
		o_rm_aff.duration = rm_aff.duration;
		o_rm_aff.flags = EX_WALL_THORNS;
		affect_to_room(o_room, &o_rm_aff);
	}

	affect_to_room(room, &rm_aff);
	return 1;
}

void
notify_cleric_moon(struct creature *ch)
{
	if (!IS_CLERIC(ch) || !ch->in_room || !PRIME_MATERIAL_ROOM(ch->in_room))
		return;

	switch (get_lunar_phase(lunar_day)) {
	case MOON_FULL:
        if (IS_EVIL(ch))
            send_to_char(ch, "The moon is full, its blasphemous light draining your dark magic.\r\n");
        else if (IS_GOOD(ch))
            send_to_char(ch, "The moon is full, blessing your magic with its radiance.\r\n");
        break;
	case MOON_NEW:
        if (IS_EVIL(ch))
            send_to_char(ch, "It is a new moon today, filling you with unholy power.\r\n");
        else if (IS_GOOD(ch))
            send_to_char(ch, "It is a new moon today, draining your strength with darkness.\r\n");
        break;
	}
}