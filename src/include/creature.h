#ifndef _CREATURE_H_
#define _CREATURE_H_

//
// File: creature.h                      -- Part of TempusMUD
//
// All modifications and additions are
// Copyright 1998 by John Watson, all rights reserved.
//

#include "constants.h"
#include "macros.h"
#include "account.h"
#include "desc_data.h"

/* char and mob-related defines *****************************************/

/* Rent codes */
static const int RENT_UNDEF     = 0;
static const int RENT_CRASH     = 1;
static const int RENT_RENTED    = 2;
static const int RENT_CRYO      = 3;
static const int RENT_FORCED    = 4;
static const int RENT_TIMEDOUT  = 5;


/* PC char_classes */
static const int CLASS_HELP = -2;
static const int CLASS_UNDEFINED = -1;
static const int CLASS_MAGIC_USER = 0;
static const int CLASS_MAGE = CLASS_MAGIC_USER;
static const int CLASS_CLERIC = 1;
static const int CLASS_THIEF = 2;
static const int CLASS_WARRIOR = 3;
static const int CLASS_BARB = 4;
static const int CLASS_PSIONIC = 5;	/* F */
static const int CLASS_PHYSIC = 6;	/* F */
static const int CLASS_CYBORG = 7;	/* F */
static const int CLASS_KNIGHT = 8;
static const int CLASS_RANGER = 9;
static const int CLASS_HOOD = 10;	/* F */
static const int CLASS_MONK = 11;
static const int CLASS_VAMPIRE = 12;
static const int CLASS_MERCENARY = 13;
static const int CLASS_SPARE1 = 14;
static const int CLASS_SPARE2 = 15;
static const int CLASS_SPARE3 = 16;

static const int NUM_CLASSES = 17;	/* This must be the number of char_classes!! */
static const int CLASS_NORMAL = 50;
static const int CLASS_BIRD = 51;
static const int CLASS_PREDATOR = 52;
static const int CLASS_SNAKE = 53;
static const int CLASS_HORSE = 54;
static const int CLASS_SMALL = 55;
static const int CLASS_MEDIUM = 56;
static const int CLASS_LARGE = 57;
static const int CLASS_SCIENTIST = 58;
static const int CLASS_SKELETON = 60;
static const int CLASS_GHOUL = 61;
static const int CLASS_SHADOW = 62;
static const int CLASS_WIGHT = 63;
static const int CLASS_WRAITH = 64;
static const int CLASS_MUMMY = 65;
static const int CLASS_SPECTRE = 66;
static const int CLASS_NPC_VAMPIRE = 67;
static const int CLASS_GHOST = 68;
static const int CLASS_LICH = 69;
static const int CLASS_ZOMBIE = 70;

static const int CLASS_EARTH = 81;	/* Elementals */
static const int CLASS_FIRE = 82;
static const int CLASS_WATER = 83;
static const int CLASS_AIR = 84;
static const int CLASS_LIGHTNING = 85;
static const int CLASS_GREEN = 91;	/* Dragons */
static const int CLASS_WHITE = 92;
static const int CLASS_BLACK = 93;
static const int CLASS_BLUE = 94;
static const int CLASS_RED = 95;
static const int CLASS_SILVER = 96;
static const int CLASS_SHADOW_D = 97;
static const int CLASS_DEEP = 98;
static const int CLASS_TURTLE = 99;
static const int CLASS_LEAST = 101;	/* Devils  */
static const int CLASS_LESSER = 102;
static const int CLASS_GREATER = 103;
static const int CLASS_DUKE = 104;
static const int CLASS_ARCH = 105;
static const int CLASS_HILL = 111;
static const int CLASS_STONE = 112;
static const int CLASS_FROST = 113;
static const int CLASS_FIRE_G = 114;
static const int CLASS_CLOUD = 115;
static const int CLASS_STORM = 116;
static const int CLASS_SLAAD_RED = 120;	/* Slaad */
static const int CLASS_SLAAD_BLUE = 121;
static const int CLASS_SLAAD_GREEN = 122;
static const int CLASS_SLAAD_GREY = 123;
static const int CLASS_SLAAD_DEATH = 124;
static const int CLASS_SLAAD_LORD = 125;
static const int CLASS_DEMON_I = 130;	/* Demons of the Abyss */
static const int CLASS_DEMON_II = 131;
static const int CLASS_DEMON_III = 132;
static const int CLASS_DEMON_IV = 133;
static const int CLASS_DEMON_V = 134;
static const int CLASS_DEMON_VI = 135;
static const int CLASS_DEMON_SEMI = 136;
static const int CLASS_DEMON_MINOR = 137;
static const int CLASS_DEMON_MAJOR = 138;
static const int CLASS_DEMON_LORD = 139;
static const int CLASS_DEMON_PRINCE = 140;
static const int CLASS_DEVA_ASTRAL = 150;
static const int CLASS_DEVA_MONADIC = 151;
static const int CLASS_DEVA_MOVANIC = 152;
static const int CLASS_MEPHIT_FIRE = 160;
static const int CLASS_MEPHIT_LAVA = 161;
static const int CLASS_MEPHIT_SMOKE = 162;
static const int CLASS_MEPHIT_STEAM = 163;
static const int CLASS_DAEMON_ARCANA = 170;	// daemons
static const int CLASS_DAEMON_CHARONA = 171;
static const int CLASS_DAEMON_DERGHO = 172;
static const int CLASS_DAEMON_HYDRO = 173;
static const int CLASS_DAEMON_PISCO = 174;
static const int CLASS_DAEMON_ULTRO = 175;
static const int CLASS_DAEMON_YAGNO = 176;
static const int CLASS_DAEMON_PYRO = 177;
static const int CLASS_GODLING = 178;
static const int CLASS_DIETY = 179;

static const int TOP_CLASS = 180;

#define RACE_UNDEFINED  -1
static const int RACE_HUMAN = 0;
static const int RACE_ELF = 1;
static const int RACE_DWARF = 2;
static const int RACE_HALF_ORC = 3;
static const int RACE_KLINGON = 4;
static const int RACE_HAFLING = 5;
static const int RACE_TABAXI = 6;
static const int RACE_DROW = 7;
static const int RACE_MOBILE = 10;
static const int RACE_UNDEAD = 11;
static const int RACE_HUMANOID = 12;
static const int RACE_ANIMAL = 13;
static const int RACE_DRAGON = 14;
static const int RACE_GIANT = 15;
static const int RACE_ORC = 16;
static const int RACE_GOBLIN = 17;
static const int RACE_HALFLING = 18;
static const int RACE_MINOTAUR = 19;
static const int RACE_TROLL = 20;
static const int RACE_GOLEM = 21;
static const int RACE_ELEMENTAL = 22;
static const int RACE_OGRE = 23;
static const int RACE_DEVIL = 24;
static const int RACE_TROGLODYTE = 25;
static const int RACE_MANTICORE = 26;
static const int RACE_BUGBEAR = 27;
static const int RACE_DRACONIAN = 28;
static const int RACE_DUERGAR = 29;
static const int RACE_SLAAD = 30;
static const int RACE_ROBOT = 31;
static const int RACE_DEMON = 32;
static const int RACE_DEVA = 33;
static const int RACE_PLANT = 34;
static const int RACE_ARCHON = 35;
static const int RACE_PUDDING = 36;
static const int RACE_ALIEN_1 = 37;
static const int RACE_PRED_ALIEN = 38;
static const int RACE_SLIME = 39;
static const int RACE_ILLITHID = 40;
static const int RACE_FISH = 41;
static const int RACE_BEHOLDER = 42;
static const int RACE_GASEOUS = 43;
static const int RACE_GITHYANKI = 44;
static const int RACE_INSECT = 45;
static const int RACE_DAEMON = 46;
static const int RACE_MEPHIT = 47;
static const int RACE_KOBOLD = 48;
static const int RACE_UMBER_HULK = 49;
static const int RACE_WEMIC = 50;
static const int RACE_RAKSHASA = 51;
static const int RACE_SPIDER = 52;
static const int RACE_GRIFFIN = 53;
static const int RACE_ROTARIAN = 54;
static const int RACE_HALF_ELF = 55;
static const int RACE_CELESTIAL = 56;
static const int RACE_GUARDINAL = 57;
static const int RACE_OLYMPIAN = 58;
static const int NUM_RACES = 59;
static const int NUM_PC_RACES = 9;

// Hometown defines

#define HOME_UNDEFINED   -1
static const int HOME_MODRIAN = 0;
static const int HOME_NEW_THALOS = 1;
static const int HOME_ELECTRO = 2;
static const int HOME_ELVEN_VILLAGE = 3;
static const int HOME_ISTAN = 4;
static const int HOME_MONK = 5;
static const int HOME_NEWBIE_TOWER = 6;
static const int HOME_SOLACE_COVE = 7;
static const int HOME_MAVERNAL = 8;
static const int HOME_ZUL_DANE = 9;
static const int HOME_ARENA = 10;
static const int HOME_CITY = 11;
static const int HOME_DOOM = 12;
static const int HOME_SKULLPORT = 15;
static const int HOME_DWARVEN_CAVERNS = 16;
static const int HOME_HUMAN_SQUARE = 17;
static const int HOME_DROW_ISLE = 18;
static const int HOME_ASTRAL_MANSE = 19;
static const int HOME_SKULLPORT_NEWBIE = 20;
static const int HOME_NEWBIE_SCHOOL = 21;
static const int HOME_KROMGUARD = 22;
static const int NUM_HOMETOWNS = 23;

/* Deity Defines                */
static const int DEITY_NONE = 0;
static const int DEITY_GUIHARIA = 1;
static const int DEITY_PAN = 2;
static const int DEITY_JUSTICE = 3;
static const int DEITY_ARES = 4;
static const int DEITY_KALAR = 5;


/* Sex */
static const int SEX_NEUTRAL = 0;
static const int SEX_MALE = 1;
static const int SEX_FEMALE = 2;


/* Positions */
static const int BOTTOM_POS = 0;
static const int POS_DEAD = 0;	/* dead            */
static const int POS_MORTALLYW = 1;	/* mortally wounded    */
static const int POS_INCAP = 2;	/* incapacitated    */
static const int POS_STUNNED = 3;	/* stunned        */
static const int POS_SLEEPING = 4;	/* sleeping        */
static const int POS_RESTING = 5;	/* resting        */
static const int POS_SITTING = 6;	/* sitting        */
static const int POS_FIGHTING = 7;	/* fighting        */
static const int POS_STANDING = 8;	/* standing        */
static const int POS_FLYING = 9;	/* flying around        */
static const int POS_MOUNTED = 10;
static const int POS_SWIMMING = 11;
static const int TOP_POS = 11;


/* Player flags: used by Creature.char_specials.act */
static const int PLR_KILLER = (1 << 0);	/* Player is a player-killer        */
static const int PLR_THIEF = (1 << 1);	/* Player is a player-thief        */
static const int PLR_FROZEN = (1 << 2);	/* Player is frozen            */
static const int PLR_DONTSET = (1 << 3);	/* Don't EVER set (ISNPC bit)    */
static const int PLR_WRITING = (1 << 4);	/* Player writing (board/mail/olc)    */
static const int PLR_MAILING = (1 << 5);	/* Player is writing mail        */
static const int PLR_CRASH = (1 << 6);	/* Player needs to be crash-saved    */
static const int PLR_SITEOK = (1 << 7);	/* Player has been site-cleared    */
static const int PLR_NOSHOUT = (1 << 8);	/* Player not allowed to shout/goss    */
static const int PLR_NOTITLE = (1 << 9);	/* Player not allowed to set title    */
static const int PLR_DELETED = (1 << 10);	/* Player deleted - space reusable    */
static const int PLR_LOADROOM = (1 << 11);	/* Player uses nonstandard loadroom    */
static const int PLR_NOWIZLIST = (1 << 12);	/* Player shouldn't be on wizlist    */
static const int PLR_NODELETE = (1 << 13);	/* Player shouldn't be deleted    */
static const int PLR_INVSTART = (1 << 14);	/* Player should enter game wizinvis    */
static const int PLR_CRYO = (1 << 15);	/* Player is cryo-saved (purge prog)    */
static const int PLR_AFK = (1 << 16);	/* Player is away from keyboard      */
static const int PLR_CLAN_LEADER = (1 << 17);	/* The head of the respective clan   */
static const int PLR_UNUSED = (1 << 18);
static const int PLR_OLC = (1 << 19);	/* Player is descripting olc         */
static const int PLR_HALT = (1 << 20);	/* Player is halted                  */
static const int PLR_OLCGOD = (1 << 21);	/* Player can edit at will           */
static const int PLR_TESTER = (1 << 22);	/* Player is a tester                */
static const int PLR_QUESTOR = (1 << 23);	/* Quest god                         */
static const int PLR_MORTALIZED = (1 << 24);	/* God can be killed                 */
static const int PLR_UNUSED2 = (1 << 25);
static const int PLR_COUNCIL = (1 << 26);
static const int PLR_NOPOST = (1 << 27);
static const int PLR_LOG = (1 << 28);	/* log all cmds */
static const int PLR_POLC = (1 << 29);	/* player approved for port olc      */
static const int PLR_NOPK = (1 << 30);	/* player cannot pk */

// Player Flags Mark II
static const int PLR2_SOULLESS = (1 << 0);	// Signing the Unholy Compact.
static const int PLR2_BURIED = (1 << 1);	// Player has died way too many times.
static const int PLR2_COMBAT = (1 << 2);	// The player is engaged in a combat    

/* Mobile flags: used by Creature.char_specials.act */
static const int MOB_SPEC = (1 << 0);	/* Mob has a callable spec-proc    */
static const int MOB_SENTINEL = (1 << 1);	/* Mob should not move        */
static const int MOB_SCAVENGER = (1 << 2);	/* Mob picks up stuff on the ground    */
static const int MOB_ISNPC = (1 << 3);	/* (R) Automatically set on all Mobs    */
static const int MOB_AWARE = (1 << 4);	/* Mob can't be backstabbed        */
static const int MOB_AGGRESSIVE = (1 << 5);	/* Mob hits players in the room    */
static const int MOB_STAY_ZONE = (1 << 6);	/* Mob shouldn't wander out of zone    */
static const int MOB_WIMPY = (1 << 7);	/* Mob flees if severely injured    */
static const int MOB_AGGR_EVIL = (1 << 8);	/* auto attack evil PC's        */
static const int MOB_AGGR_GOOD = (1 << 9);	/* auto attack good PC's        */
static const int MOB_AGGR_NEUTRAL = (1 << 10);	/* auto attack neutral PC's        */
static const int MOB_MEMORY = (1 << 11);	/* remember attackers if attacked    */
static const int MOB_HELPER = (1 << 12);	/* attack PCs fighting other NPCs    */
static const int MOB_NOCHARM = (1 << 13);	/* Mob can't be charmed        */
static const int MOB_NOSUMMON = (1 << 14);	/* Mob can't be summoned        */
static const int MOB_NOSLEEP = (1 << 15);	// Mob can't be slept
static const int MOB_NOBASH = (1 << 16);	// Mob can't be bashed (e.g. trees)
static const int MOB_NOBLIND = (1 << 17);	// Mob can't be blinded
static const int MOB_NOTURN = (1 << 18);	// Hard to turn
static const int MOB_NOPETRI = (1 << 19);	// Cannot be petrified
static const int MOB_PET = (1 << 20);	// Mob is a conjured pet and shouldn't
										 // get nor give any xp in any way.
static const int MOB_SOULLESS = (1 << 21);	// Mobile is Soulless - Unholy compact.
static const int MOB_SPIRIT_TRACKER = (1 << 22);	// Can track through !track
static const int MOB_ISCRIPT = (1 << 23);	// Mob has an iscript
#define NUM_MOB_FLAGS             24

static const int MOB2_SCRIPT = (1 << 0);
static const int MOB2_MOUNT = (1 << 1);
static const int MOB2_STAY_SECT = (1 << 2);	/* Can't leave SECT_type.   */
static const int MOB2_ATK_MOBS = (1 << 3);	/* Aggro Mobs will attack other mobs */
static const int MOB2_HUNT = (1 << 4);	/* Mob will hunt attacker    */
static const int MOB2_LOOTER = (1 << 5);	/* Loots corpses     */
static const int MOB2_NOSTUN = (1 << 6);
static const int MOB2_SELLER = (1 << 7);	/* If shopkeeper, sells anywhere. */
static const int MOB2_WONT_WEAR = (1 << 8);	/* Wont wear shit it picks up (SHPKPER) */
static const int MOB2_SILENT_HUNTER = (1 << 9);
static const int MOB2_FAMILIAR = (1 << 10);	/* mages familiar */
static const int MOB2_NO_FLOW = (1 << 11);	/* Mob doesnt flow */
static const int MOB2_UNAPPROVED = (1 << 12);	/* Mobile not approved for game play */
static const int MOB2_RENAMED = (1 << 13);	/* Mobile renamed */
static const int MOB2_NOAGGRO_RACE = (1 << 14);	/* wont attack members of own race */
static const int MOB2_MUGGER = (1 << 15);
#define NUM_MOB2_FLAGS            16

/* Preference flags: used by Creature.player_specials.pref */
static const int PRF_BRIEF = (1 << 0);	/* Room descs won't normally be shown    */
static const int PRF_COMPACT = (1 << 1);	/* No extra CRLF pair before prompts    */
static const int PRF_DEAF = (1 << 2);	/* Can't hear shouts            */
static const int PRF_NOTELL = (1 << 3);	/* Can't receive tells        */
static const int PRF_DISPHP = (1 << 4);	/* Display hit points in prompt    */
static const int PRF_DISPMANA = (1 << 5);	/* Display mana points in prompt    */
static const int PRF_DISPMOVE = (1 << 6);	/* Display move points in prompt    */
static const int PRF_AUTOEXIT = (1 << 7);	/* Display exits in a room        */
static const int PRF_NOHASSLE = (1 << 8);	/* Aggr mobs won't attack        */
static const int PRF_QUEST = (1 << 9);	/* On quest                */
static const int PRF_SUMMONABLE = (1 << 10);	/* Can be summoned            */
static const int PRF_NOREPEAT = (1 << 11);	/* No repetition of comm commands    */
static const int PRF_HOLYLIGHT = (1 << 12);	/* Can see in dark            */
static const int PRF_COLOR_1 = (1 << 13);	/* Color (low bit)            */
static const int PRF_COLOR_2 = (1 << 14);	/* Color (high bit)            */
static const int PRF_NOWIZ = (1 << 15);	/* Can't hear wizline            */
static const int PRF_LOG1 = (1 << 16);	/* On-line System Log (low bit)    */
static const int PRF_LOG2 = (1 << 17);	/* On-line System Log (high bit)    */
static const int PRF_NOAUCT = (1 << 18);	/* Can't hear auction channel        */
static const int PRF_NOGOSS = (1 << 19);	/* Can't hear gossip channel        */
static const int PRF_NOGRATZ = (1 << 20);	/* Can't hear grats channel        */
static const int PRF_ROOMFLAGS = (1 << 21);	/* Can see room flags (ROOM_x)    */
static const int PRF_NOSNOOP = (1 << 22);	/* Can not be snooped by immortals    */
static const int PRF_NOMUSIC = (1 << 23);	/* Can't hear music channel            */
static const int PRF_NOSPEW = (1 << 24);	/* Can't hear spews            */
static const int PRF_GAGMISS = (1 << 25);	/* Doesn't see misses during fight    */
static const int PRF_NOPROJECT = (1 << 26);	/* Cannot hear the remort channel     */
static const int PRF_NOINTWIZ = (1 << 27);
static const int PRF_NOCLANSAY = (1 << 28);	/* Doesnt hear clan says and such     */
static const int PRF_NOIDENTIFY = (1 << 29);	/* Saving throw is made when id'd     */
static const int PRF_NODREAM = (1 << 30);

// PRF 2 Flags

static const int PRF2_DEBUG = (1 << 0);	/* Sees info on fight.              */
static const int PRF2_NEWBIE_HELPER = (1 << 1);	/* sees newbie arrivals             */
static const int PRF2_AUTO_DIAGNOSE = (1 << 2);	/* automatically see condition of enemy */
static const int PRF2_AUTOPAGE = (1 << 3);	/* Beeps when ch receives a tell    */
static const int PRF2_NOAFFECTS = (1 << 4);	/* Affects are not shown in score   */
static const int PRF2_NOHOLLER = (1 << 5);	/* Gods only                        */
static const int PRF2_NOIMMCHAT = (1 << 6);	/* Gods only                        */
static const int PRF2_CLAN_TITLE = (1 << 7);	/* auto-sets title to clan stuff */
static const int PRF2_CLAN_HIDE = (1 << 8);	/* don't show badge in who list */
static const int PRF2_LIGHT_READ = (1 << 9);	/* interrupts while d->showstr_point */
static const int PRF2_AUTOPROMPT = (1 << 10);	/* always draw new prompt */
static const int PRF2_NOWHO = (1 << 11);	/* don't show in who */
static const int PRF2_ANONYMOUS = (1 << 12);	/* dont show char_class, level */
static const int PRF2_NOTRAILERS = (1 << 13);	/* don't show trailer affects */
static const int PRF2_VT100 = (1 << 14);	/* Players uses VT100 inferface */
static const int PRF2_AUTOSPLIT = (1 << 15);
static const int PRF2_AUTOLOOT = (1 << 16);
static const int PRF2_PKILLER = (1 << 17);	// player can attack other players
static const int PRF2_NOGECHO = (1 << 18);	// Silly Gecho things
static const int PRF2_NOWRAP = (1 << 19);	// turns off autowrap temporarily.
static const int PRF2_DISPALIGN = (1 << 20);
static const int PRF2_WORLDWRITE = (1 << 21); // allows worldwrite to work
static const int PRF2_NOGUILDSAY = (1 << 22);

/* Affect bits: used in Creature.char_specials.saved.affected_by */
/* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved") */
static const int AFF_BLIND = (1 << 0);	/* (R) Char is blind        */
static const int AFF_INVISIBLE = (1 << 1);	/* Char is invisible        */
static const int AFF_DETECT_ALIGN = (1 << 2);	/* Char is sensitive to align */
static const int AFF_DETECT_INVIS = (1 << 3);	/* Char can see invis chars  */
static const int AFF_DETECT_MAGIC = (1 << 4);	/* Char is sensitive to magic */
static const int AFF_SENSE_LIFE = (1 << 5);	/* Char can sense hidden life */
static const int AFF_WATERWALK = (1 << 6);	/* Char can walk on water    */
static const int AFF_SANCTUARY = (1 << 7);	/* Char protected by sanct.    */
static const int AFF_GROUP = (1 << 8);	/* (R) Char is grouped    */
static const int AFF_CURSE = (1 << 9);	/* Char is cursed        */
static const int AFF_INFRAVISION = (1 << 10);	/* Char can see in dark    */
static const int AFF_POISON = (1 << 11);	/* (R) Char is poisoned    */
static const int AFF_PROTECT_EVIL = (1 << 12);	/* Char protected from evil  */
static const int AFF_PROTECT_GOOD = (1 << 13);	/* Char protected from good  */
static const int AFF_SLEEP = (1 << 14);	/* (R) Char magically asleep    */
static const int AFF_NOTRACK = (1 << 15);	/* Char can't be tracked    */
static const int AFF_INFLIGHT = (1 << 16);	/* Room for future expansion    */
static const int AFF_TIME_WARP = (1 << 17);	/* Room for future expansion    */
static const int AFF_SNEAK = (1 << 18);	/* Char can move quietly    */
static const int AFF_HIDE = (1 << 19);	/* Char is hidden        */
static const int AFF_WATERBREATH = (1 << 20);	/* Room for future expansion    */
static const int AFF_CHARM = (1 << 21);	/* Char is charmed        */
static const int AFF_CONFUSION = (1 << 22);	/* Char is confused         */
static const int AFF_NOPAIN = (1 << 23);	/* Char feels no pain    */
static const int AFF_RETINA = (1 << 24);	/* Char's retina is stimulated */
static const int AFF_ADRENALINE = (1 << 25);	/* Char's adrenaline is pumping */
static const int AFF_CONFIDENCE = (1 << 26);	/* Char is confident           */
static const int AFF_REJUV = (1 << 27);	/* Char is rejuvenating */
static const int AFF_REGEN = (1 << 28);	/* Body is regenerating */
static const int AFF_GLOWLIGHT = (1 << 29);	/* Light spell is operating   */
static const int AFF_BLUR = (1 << 30);	/* Blurry image */
#define NUM_AFF_FLAGS         31

static const int AFF2_FLUORESCENT = (1 << 0);
static const int AFF2_TRANSPARENT = (1 << 1);
static const int AFF2_SLOW = (1 << 2);
static const int AFF2_HASTE = (1 << 3);
static const int AFF2_MOUNTED = (1 << 4);	/*DO NOT SET THIS IN MOB FILE */
static const int AFF2_FIRE_SHIELD = (1 << 5);	/* affected by Fire Shield  */
static const int AFF2_BERSERK = (1 << 6);
static const int AFF2_INTIMIDATED = (1 << 7);
static const int AFF2_TRUE_SEEING = (1 << 8);
static const int AFF2_DIVINE_ILLUMINATION = (1 << 9);
static const int AFF2_PROTECT_UNDEAD = (1 << 10);
static const int AFF2_INVIS_TO_UNDEAD = (1 << 11);
static const int AFF2_INVIS_TO_ANIMALS = (1 << 12);
static const int AFF2_ENDURE_COLD = (1 << 13);
static const int AFF2_PARALYZED = (1 << 14);
static const int AFF2_PROT_LIGHTNING = (1 << 15);
static const int AFF2_PROT_FIRE = (1 << 16);
static const int AFF2_TELEKINESIS = (1 << 17);	/* Char can carry more stuff */
static const int AFF2_PROT_RAD = (1 << 18);	/* Enables Autoexits ! :)    */
static const int AFF2_ABLAZE = (1 << 19);
static const int AFF2_NECK_PROTECTED = (1 << 20);	/* Can't be beheaded         */
static const int AFF2_DISPLACEMENT = (1 << 21);
static const int AFF2_PROT_DEVILS = (1 << 22);
static const int AFF2_MEDITATE = (1 << 23);
static const int AFF2_EVADE = (1 << 24);
static const int AFF2_BLADE_BARRIER = (1 << 25);
static const int AFF2_OBLIVITY = (1 << 26);
static const int AFF2_ENERGY_FIELD = (1 << 27);
static const int AFF2_PETRIFIED = (1 << 28);
static const int AFF2_VERTIGO = (1 << 29);
static const int AFF2_PROT_DEMONS = (1 << 30);
#define NUM_AFF2_FLAGS          31

static const int AFF3_ATTRACTION_FIELD = (1 << 0);
static const int AFF3_FEEDING = (1 << 1);
static const int AFF3_POISON_2 = (1 << 2);
static const int AFF3_POISON_3 = (1 << 3);
static const int AFF3_SICKNESS = (1 << 4);
static const int AFF3_SELF_DESTRUCT = (1 << 5);	/* Self-destruct sequence init */
static const int AFF3_DAMAGE_CONTROL = (1 << 6);	/* Damage control for cyborgs  */
static const int AFF3_STASIS = (1 << 7);	/* Borg is in static state     */
static const int AFF3_PRISMATIC_SPHERE = (1 << 8);	/* Defensive */
static const int AFF3_RADIOACTIVE = (1 << 9);
static const int AFF3_UNUSED_RAD_SICKNESS = (1 << 10);	// unused
static const int AFF3_MANA_TAP = (1 << 11);
static const int AFF3_ENERGY_TAP = (1 << 12);
static const int AFF3_SONIC_IMAGERY = (1 << 13);
static const int AFF3_SHROUD_OBSCUREMENT = (1 << 14);
static const int AFF3_NOBREATHE = (1 << 15);
static const int AFF3_PROT_HEAT = (1 << 16);
static const int AFF3_PSISHIELD = (1 << 17);
static const int AFF3_PSYCHIC_CRUSH = (1 << 18);
static const int AFF3_DOUBLE_DAMAGE = (1 << 19);
static const int AFF3_ACIDITY = (1 << 20);
static const int AFF3_HAMSTRUNG = (1 << 21);	// Bleeding badly from the leg 
static const int AFF3_GRAVITY_WELL = (1 << 22);	// Pissed off a phyz and got hit by gravity well
static const int AFF3_SYMBOL_OF_PAIN = (1 << 23);	// Char's mind is burning with pain
static const int AFF3_EMP_SHIELD = (1 << 24);	// EMP SHIELDING
static const int AFF3_INST_AFF = (1 << 25);	// Affected by an instant affect
static const int AFF3_TAINTED = (1 << 27);	// Knight spell, "taint"
static const int AFF3_INFILTRATE = (1 << 28);	// Merc skill infiltrate
static const int AFF3_DIVINE_POWER = (1 << 29);
#define NUM_AFF3_FLAGS                30

static const int ARRAY_AFF_1 = 1;
static const int ARRAY_AFF_2 = 2;
static const int ARRAY_AFF_3 = 3;

/* Character equipment positions: used as index for Creature.equipment[] */
/* NOTE: Don't confuse these constants with the ITEM_ bitvectors
   which control the valid places you can wear a piece of equipment */
static const int WEAR_LIGHT = 0;
static const int WEAR_FINGER_R = 1;
static const int WEAR_FINGER_L = 2;
static const int WEAR_NECK_1 = 3;
static const int WEAR_NECK_2 = 4;
static const int WEAR_BODY = 5;
static const int WEAR_HEAD = 6;
static const int WEAR_LEGS = 7;
static const int WEAR_FEET = 8;
static const int WEAR_HANDS = 9;
static const int WEAR_ARMS = 10;
static const int WEAR_SHIELD = 11;
static const int WEAR_ABOUT = 12;
static const int WEAR_WAIST = 13;
static const int WEAR_WRIST_R = 14;
static const int WEAR_WRIST_L = 15;
static const int WEAR_WIELD = 16;
static const int WEAR_HOLD = 17;
static const int WEAR_CROTCH = 18;
static const int WEAR_EYES = 19;
static const int WEAR_BACK = 20;
static const int WEAR_BELT = 21;
static const int WEAR_FACE = 22;
static const int WEAR_EAR_L = 23;
static const int WEAR_EAR_R = 24;
static const int WEAR_WIELD_2 = 25;
static const int WEAR_ASS = 26;
static const int NUM_WEARS = 27;	/* This must be the # of eq positions!! */
static const int WEAR_RANDOM = 28;


/* Modifier constants used with obj affects ('A' fields) */
static const int APPLY_NONE = 0;	/* No effect            */
static const int APPLY_STR = 1;	/* Apply to strength        */
static const int APPLY_DEX = 2;	/* Apply to dexterity        */
static const int APPLY_INT = 3;	/* Apply to intellegence    */
static const int APPLY_WIS = 4;	/* Apply to wisdom        */
static const int APPLY_CON = 5;	/* Apply to constitution    */
static const int APPLY_CHA = 6;	/* Apply to charisma        */
static const int APPLY_CLASS = 7;	/* Reserved            */
static const int APPLY_LEVEL = 8;	/* Reserved            */
static const int APPLY_AGE = 9;	/* Apply to age            */
static const int APPLY_CHAR_WEIGHT = 10;	/* Apply to weight        */
static const int APPLY_CHAR_HEIGHT = 11;	/* Apply to height        */
static const int APPLY_MANA = 12;	/* Apply to max mana        */
static const int APPLY_HIT = 13;	/* Apply to max hit points    */
static const int APPLY_MOVE = 14;	/* Apply to max move points    */
static const int APPLY_GOLD = 15;	/* Reserved            */
static const int APPLY_EXP = 16;	/* Reserved            */
static const int APPLY_AC = 17;	/* Apply to Armor Class        */
static const int APPLY_HITROLL = 18;	/* Apply to hitroll        */
static const int APPLY_DAMROLL = 19;	/* Apply to damage roll        */
static const int APPLY_SAVING_PARA = 20;	/* Apply to save throw: paralz    */
static const int APPLY_SAVING_ROD = 21;	/* Apply to save throw: rods    */
static const int APPLY_SAVING_PETRI = 22;	/* Apply to save throw: petrif    */
static const int APPLY_SAVING_BREATH = 23;	/* Apply to save throw: breath    */
static const int APPLY_SAVING_SPELL = 24;	/* Apply to save throw: spells    */
static const int APPLY_SNEAK = 25;
static const int APPLY_HIDE = 26;
static const int APPLY_RACE = 27;
static const int APPLY_SEX = 28;
static const int APPLY_BACKSTAB = 29;
static const int APPLY_PICK_LOCK = 30;
static const int APPLY_PUNCH = 31;
static const int APPLY_SHOOT = 32;
static const int APPLY_KICK = 33;
static const int APPLY_TRACK = 34;
static const int APPLY_IMPALE = 35;
static const int APPLY_BEHEAD = 36;
static const int APPLY_THROWING = 37;
static const int APPLY_RIDING = 38;
static const int APPLY_TURN = 39;
static const int APPLY_SAVING_CHEM = 40;
static const int APPLY_SAVING_PSI = 41;
static const int APPLY_ALIGN = 42;
static const int APPLY_SAVING_PHY = 43;
static const int APPLY_CASTER = 44;	// special usage
static const int APPLY_WEAPONSPEED = 45;
static const int APPLY_DISGUISE = 46;
static const int APPLY_NOTHIRST = 47;
static const int APPLY_NOHUNGER = 48;
static const int APPLY_NODRUNK = 49;
static const int APPLY_SPEED = 50;
static const int NUM_APPLIES = 51;


/* other miscellaneous defines *******************************************/


/* Player conditions */
static const int DRUNK = 0;
static const int FULL = 1;
static const int THIRST = 2;



/* other #defined constants **********************************************/

static const int LVL_GRIMP = 72;
static const int LVL_FISHBONE = 71;
static const int LVL_LUCIFER = 70;
static const int LVL_IMPL = 69;
static const int LVL_ENTITY = 68;
static const int LVL_ANCIENT = LVL_ENTITY;
static const int LVL_CREATOR = 67;
static const int LVL_GRGOD = 66;
static const int LVL_TIMEGOD = 65;
static const int LVL_DEITY = 64;
static const int LVL_GOD = 63;	/* Lesser God */
static const int LVL_ENERGY = 62;
static const int LVL_FORCE = 61;
static const int LVL_POWER = 60;
static const int LVL_BEING = 59;
static const int LVL_SPIRIT = 58;
static const int LVL_ELEMENT = 57;
static const int LVL_DEMI = 56;
static const int LVL_ETERNAL = 55;
static const int LVL_ETHEREAL = 54;
static const int LVL_LUMINARY = 53;
static const int LVL_BUILDER = 52;
static const int LVL_IMMORT = 51;
static const int LVL_AMBASSADOR = 50;

static const int LVL_FREEZE = LVL_IMMORT;
static const int LVL_CAN_BAN = LVL_GOD;
static const int LVL_VIOLENCE = LVL_POWER;
static const int LVL_LOGALL = LVL_CREATOR;


/* char-related structures ************************************************/

/* memory structure for characters */
struct memory_rec_struct {
	long id;
	struct memory_rec_struct *next;
};

typedef struct memory_rec_struct memory_rec;


/* These data contain information about a players time data */
struct time_data {
	time_t birth;				/* This represents the characters age                */
	time_t death;				// when did we die
	time_t logon;				/* Time of the last logon (used to calculate played) */
	int played;					/* This is the total accumulated time played in secs */
};

static const int MAX_WEAPON_SPEC = 6;
typedef struct weapon_spec {
	int vnum;
	ubyte level;
} weapon_spec;

/* general player-related info, usually PC's and NPC's */
struct char_player_data {
	char passwd[MAX_PWD_LENGTH + 1];	/* character's password      */
	char *name;					/* PC / NPC s name (kill ...  )         */
	char *short_descr;			/* for NPC 'actions'                    */
	char *long_descr;			/* for 'look'                   */
	char *description;			/* Extra descriptions                   */
	char *title;				/* PC / NPC's title                     */
	sh_int char_class;			/* PC / NPC's char_class               */
	sh_int remort_char_class;	/* PC / NPC REMORT CLASS (-1 for none) */
	sh_int weight;				/* PC / NPC's weight                    */
	short getWeight() {
		return weight;
	} 
	short setWeight(const short new_weight) {
		return (weight = new_weight);
	}
	short modifyWeight(const short mod_weight);

	sh_int height;				/* PC / NPC's height                    */
	sh_int hometown;			/* PC s Hometown (zone)                 */
	byte sex;					/* PC / NPC's sex                       */
	byte race;					/* PC / NPC's race                  */
	byte level;					/* PC / NPC's level                     */
	byte age_adjust;			/* PC age adjust to maintain sanity     */
	struct time_data time;		/* PC's AGE in days                 */
};


/* Char's abilities.  Used in char_file_u *DO*NOT*CHANGE* */
struct char_ability_data {
	sbyte str;
	sbyte str_add;				/* 000 - 100 if strength 18             */
	sbyte intel;
	sbyte wis;
	sbyte dex;
	sbyte con;
	sbyte cha;
};


/* Char's points.  Used in char_file_u *DO*NOT*CHANGE* */
struct char_point_data {
	sh_int mana;
	sh_int max_mana;			/* Max move for PC/NPC               */
	sh_int hit;
	sh_int max_hit;				/* Max hit for PC/NPC                      */
	sh_int move;
	sh_int max_move;			/* Max move for PC/NPC                     */

	sh_int armor;				/* Internal -100..100, external -10..10 AC */
	int gold;					/* Money carried                           */
	int bank_gold;				/* Gold the char has in a bank account       */
	int cash;					// cash on hand
	int credits;				/* Internal net credits            */
	int exp;					/* The experience of the player            */

	sbyte hitroll;				/* Any bonus or penalty to the hit roll    */
	sbyte damroll;				/* Any bonus or penalty to the damage roll */
};


/* 
 * char_special_data_saved: specials which both a PC and an NPC have in
 * common, but which must be saved to the playerfile for PC's.
 *
 * WARNING:  Do not change this structure.  Doing so will ruin the
 * playerfile.  If you want to add to the playerfile, use the spares
 * in player_special_data.
 */
struct char_special_data_saved {
	int alignment;				/* +-1000 for alignments                */
	long idnum;					/* player's idnum; -1 for mobiles    */
	long act;					/* act flag for NPC's; player flag for PC's */
	long act2;

	long affected_by;			/* Bitvector for spells/skills affected by */
	long affected2_by;
	long affected3_by;

	sh_int apply_saving_throw[10];	/* Saving throw (Bonuses)        */
};


/* Special playing constants shared by PCs and NPCs which aren't in pfile */
struct char_special_data {

	int setCarriedWeight(int new_weight) {
		return (carry_weight = new_weight);
	} 
	int setWornWeight(int new_weight) {
		return (worn_weight = new_weight);
	}
	// Set position and Get position.
	// Set returns success or failure
	// Get returns current pos
	void setPosition(int new_pos) {
		position = new_pos;
	}
	int getPosition(void) {
		return position;
	}

	int getCarriedWeight(void) {
		return carry_weight;
	}
	int getWornWeight(void) {
		return worn_weight;
	}

	struct Creature *fighting;	/* Opponent                */
	struct Creature *hunting;	/* Char hunted by this char        */
	struct Creature *mounted;	/* MOB mounted by this char        */

	int carry_weight;			/* Carried weight                     */
	int worn_weight;			/* Total weight equipped                */
	int timer;					/* Timer for update            */
	int meditate_timer;			/* How long has been meditating           */
	int cur_flow_pulse;			/* Keeps track of whether char has flowed */

	byte breath_count;			/* for breathing and falling              */
	byte fall_count;
	byte position;				/* Standing, fighting, sleeping, etc.    */
	byte carry_items;			/* Number of items carried        */
	byte weapon_proficiency;	/* Scale of learnedness of weapon prof.   */

	struct char_special_data_saved saved;	/* constants saved in plrfile    */
};


/*
 *  If you want to add new values to the playerfile, do it here.  DO NOT
 * ADD, DELETE OR MOVE ANY OF THE VARIABLES - doing so will change the
 * size of the structure and ruin the playerfile.  However, you can change
 * the names of the spares to something more meaningful, and then use them
 * in your new code.  They will automatically be transferred from the
 * playerfile into memory when players log in.
 */
struct player_special_data_saved {
	byte skills[MAX_SKILLS + 1];	/* array of skills plus skill 0        */
	weapon_spec weap_spec[MAX_WEAPON_SPEC];
	int wimp_level;				/* Below this # of hit points, flee!    */
	byte freeze_level;			/* Level of god who froze char, if any    */
	sh_int invis_level;			/* level of invisibility        */
	room_num load_room;			/* Which room to place char in        */
	long pref;					/* preference flags for PC's.        */
	long pref2;					/* 2nd pref flag                        */
	ubyte bad_pws;				/* number of bad password attemps    */
	sbyte conditions[3];		/* Drunk, full, thirsty            */

	/* spares below for future expansion.  You can change the names from
	   'sparen' to something meaningful, but don't change the order.  */

	ubyte clan;
	ubyte hold_home;
	ubyte spare22;
	ubyte broken_component;
	ubyte remort_generation;
	ubyte quest_points;
	ubyte qlog_level;			// what level of awareness we have to qlog
	ubyte speed;				// percentage of speedup
	ubyte qp_allowance;			// Quest point allowance 
	ubyte occupation;
	int deity;
	int spells_to_learn;
	int life_points;
	int pkills;
	int mobkills;
	int deaths;
	int old_char_class;			/* Type of borg, or char_class before vamprism. */
	int page_length;
	int total_dam;
	int columns;
	int hold_load_room;
	int quest_id;
	int plr2_bits;
	int spare_i[2];
	long mana_shield_low;
	long mana_shield_pct;
	int reputation;
	long spare20;
	long spare21;

};

/*
 * Specials needed only by PCs, not NPCs.  Space for this structure is
 * not allocated in memory for NPCs, but it is for PCs and the portion
 * of it labelled 'saved' is saved in the playerfile.  This structure can
 * be changed freely; beware, though, that changing the contents of
 * player_special_data_saved will corrupt the playerfile.
 */
static const int MAX_IMPRINT_ROOMS = 6;

struct player_special_data {

	struct player_special_data_saved saved;
	char *poofin;				/* Description on arrival of a god.     */
	char *poofout;				/* Description upon a god's exit.       */
	struct alias_data *aliases;	/* Character's aliases            */
	long last_tell;				/* idnum of last tell from        */
	int imprint_rooms[MAX_IMPRINT_ROOMS];
	unsigned int soilage[NUM_WEARS];
	struct obj_data *olc_obj;	/* which obj being edited               */
	struct Creature *olc_mob;	/* which mob being edited               */
	struct shop_data *olc_shop;	/* which shop being edited              */
	struct olc_help_r *olc_help;	/* which help record being edited       */
	struct special_search_data *olc_srch;	/* which srch being edited */
	struct ticl_data *olc_ticl;	/* which ticl being edited              */
	class CIScript *olc_iscript;	/* which iscript being edited           */
	class CHandler *olc_handler;	/* which iscript handler being edited   */
	struct room_data *was_in_room;	/* location for linkdead people         */
	class HelpItem *olc_help_item;
    int thaw_time;
    int freezer_id;
};

struct mob_shared_data {
	int vnum;
	int svnum;
	int number;
	int attack_type;			/* The Attack Type integer for NPC's     */
	int lair;					/* room the mob always returns to */
	int leader;					// mob vnum the mob always helps and follows
	int kills;
	int loaded;
	byte default_pos;			/* Default position for NPC              */
	byte damnodice;				/* The number of damage dice's           */
	byte damsizedice;			/* The size of the damage dice's         */
	byte morale;
	char *move_buf;				/* custom move buf */
	struct Creature *proto;	/* pointer to prototype */
	struct ticl_data *ticl_ptr;	/* Pointer to TICL procedure */
	 SPECIAL(*func);
	char *func_param;			/* mobile's special parameter str */
};


struct mob_mugger_data {
	int idnum;					/* idnum of player on shit list */
	int vnum;					/* vnum of object desired */
	byte timer;					/* how long has the mob been waiting */
};

/* Specials used by NPCs, not PCs */
struct mob_special_data {
	memory_rec *memory;			/* List of attackers to remember           */
	struct extra_descr_data *response;	/* for response processing */
	struct mob_mugger_data *mug;
	struct mob_shared_data *shared;
	int wait_state;				/* Wait state for bashed mobs           */
	byte last_direction;		/* The last direction the monster went     */
	unsigned int mob_idnum;		/* mobile's unique idnum */
};


/* An affect structure.  Used in char_file_u *DO*NOT*CHANGE* */
struct affected_type {
	int type;				/* The type of spell that caused this      */
	int duration;			/* For how long its effects will last      */
	int modifier;			/* This is added to apropriate ability     */
	int location;			/* Tells which ability to change(APPLY_XXX) */
	ubyte level;
	ubyte is_instant;
	long bitvector;				/* Tells which bits to set (AFF_XXX)       */
	int aff_index;
	struct affected_type *next;
};


/* Structure used for chars following other chars */
struct follow_type {
	struct Creature *follower;
	struct follow_type *next;
};

struct rent_info {
    rent_info() : time(0), rentcode(0), net_cost_per_diem(0), 
                  gold(0), account(0), currency(0) {}
	int time;
	int rentcode;
	int net_cost_per_diem;
	int gold;
	int account;
	int spare0;
	int spare1;
	int currency;
	int spare2;
	int spare3;
	int spare4;
	int spare5;
	int spare6;
	int spare7;
    
    void saveToXML( FILE *ouf ) const {
        fprintf( ouf, "<rent time=\"%d\" code=\"%d\" perdiem=\"%d\" "
                      "gold=\"%d\" bank=\"%d\" currency=\"%d\"/>\n",
                 time, rentcode, net_cost_per_diem, gold, account, currency );
    }
};


/* ================== Structure for player/non-player ===================== */
struct Creature {

  public:						// *******   METHODS ******
  	Creature(void);	// constructor
	~Creature(void);

	// Reset creature to initial state
	void clear();    

	// carried weight
	int getCarriedWeight(void) {
		return char_specials.getCarriedWeight();
	} int setCarriedWeight(int new_weight) {
		return char_specials.setCarriedWeight(new_weight);
	}
	int modifyCarriedWeight(int mod_weight);

	// worn weight
	int getWornWeight(void) {
		return char_specials.getWornWeight();
	}
	int setWornWeight(int new_weight) {
		return char_specials.setWornWeight(new_weight);
	}
	int modifyWornWeight(int mod_weight);

	// char weight
	short getWeight(void) {
		return player.getWeight();
	}
	short setWeight(short new_weight) {
		return player.setWeight(new_weight);
	}
	int getLevel(void) {
		return player.level;
	}
	bool isTester();

	/**
	 * Modifies the given experience to be appropriate for this character's
	 *  level/gen and class.
	 * If victim != NULL, assume that this char is fighting victim to gain experience.
	 **/
	int getPenalizedExperience( int experience, Creature *victim = NULL );
	
	short modifyWeight(short mod_weight) {
		return player.modifyWeight(mod_weight);
	}

	// breath count
	int getBreathCount(void) {
		return char_specials.breath_count;
	}
	int getBreathThreshold(void) {
		return (getLevel() >> 5) + 2;
	}
	int setBreathCount(int new_count) {
		return (char_specials.breath_count = new_count);
	}
	int modifyBreathCount(int mod_count) {
		return setBreathCount(getBreathCount() + mod_count);
	}
	// Set current position. returns success or failure
	bool setPosition(int new_pos, int mode = 0);
	// Get returns current pos (standing sitting etc.)
	int getPosition(void) { return char_specials.getPosition(); }
	// retrieve the char's speed attribute ( or 0 for mobiles )
	int getSpeed(void);
	// Assign the char's speed attribute. ( does nothing for mobs )
	void setSpeed(int speed);
	bool isNewbie();
	int getLevelBonus(int skill);
	int getLevelBonus(bool primary);
	// Various combat utility functions
	bool affBySanc(Creature * attacker = NULL);
	float getDamReduction(Creature * attacker = NULL);
	bool isFighting();
	Creature *getFighting() { return (char_specials.fighting); }
	void setFighting(Creature * ch);
	void extract(bool destroy_objs, bool save, cxn_state con_state);
	void clearMemory();
    bool loadFromXML( long id );
    void saveToXML();

	int loadObjects();
    room_data *getLoadroom(); // Retrieves the characters appropriate loadroom.

	bool displayUnrentables(void);
	long calcDailyRent(void);
	obj_data *findCostliestObj(void);
	bool payRent(time_t last_time, int code, int currency);
    // Saves the given characters equipment to a file. Intended for use while 
    // the character is still in the game. 
    bool crashSave();
    bool rentSave(int cost, int rentcode=RENT_RENTED);
    bool idleSave();
    // Drops all !cursed eq to the floor, breaking implants, then calls rentSave(0)
    bool curseSave();
    bool cryoSave(int cost);
  private:
    bool saveObjects(void);
    /** Extracts all unrentable objects carried or worn by this creature **/
    void extractUnrentables();
  public:						// ******  Data ****
	int pfilepos;				/* playerfile pos          */
	struct room_data *in_room;	/* Location (real room number)      */

	struct char_player_data player;	/* Normal data                   */
	struct char_ability_data real_abils;	/* Abilities without modifiers   */
	struct char_ability_data aff_abils;	/* Abils with spells/stones/etc  */
	struct char_point_data points;	/* Points                        */
	struct char_special_data char_specials;	/* PC/NPC specials      */
	struct player_special_data *player_specials;	/* PC specials          */
	struct mob_special_data mob_specials;	/* NPC specials          */

	struct affected_type *affected;	/* affected by what spells       */
	struct obj_data *equipment[NUM_WEARS];	/* Equipment array               */
	struct obj_data *implants[NUM_WEARS];

	struct obj_data *carrying;	/* Head of list                  */
	struct descriptor_data *desc;	/* NULL for mobiles              */
	Account *account;

	struct follow_type *followers;	/* List of chars followers       */
	struct Creature *master;	/* Who is char following?        */
	rent_info rent;
	char host[HOST_LENGTH + 1];	/* host of last logon */
};

/* ====================================================================== */


struct dex_skill_type {
	sh_int p_pocket;
	sh_int p_locks;
	sh_int traps;
	sh_int sneak;
	sh_int hide;
};


struct dex_app_type {
	sh_int reaction;
	sh_int miss_att;
	sh_int defensive;
};


struct str_app_type {
	sh_int tohit;				/* To Hit (THAC0) Bonus/Penalty        */
	sh_int todam;				/* Damage Bonus/Penalty                */
	sh_int carry_w;				/* Maximum weight that can be carrried */
	sh_int wield_w;				/* Maximum weight that can be wielded  */
};


struct wis_app_type {
	byte bonus;					/* how many practices player gains per lev */
};


struct int_app_type {
	byte learn;					/* how many % a player learns a spell/skill */
};


struct con_app_type {
	sh_int hitp;
	sh_int shock;
};

struct title_type {
	char *title_m;
	char *title_f;
};



#endif
