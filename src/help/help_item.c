#ifdef HAS_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include "structs.h"
#include "creature.h"
#include "utils.h"
#include "help.h"
#include "interpreter.h"
#include "db.h"
#include "editor.h"
#include "comm.h"
#include "screen.h"
#include "handler.h"

extern const char *Help_Directory;
extern HelpCollection *Help;
extern char gHelpbuf[];
char *one_word(char *argument, char *first_arg);
extern const char *help_group_names[];
extern const char *help_group_bits[];
extern const char *help_bit_descs[];
extern const char *help_bits[];

// Sets the flags bitvector for things like !approved and
// IMM+ and changed and um... other flaggy stuff.
void
HelpItem_SetFlags(char *argument)
{
	int state, cur_flags = 0, tmp_flags = 0, flag = 0, old_flags = 0;
	char arg1[MAX_INPUT_LENGTH];
	argument = one_argument(argument, arg1);
	skip_spaces(&argument);
	if (!*argument) {
		send_to_char(editor, "Invalid flags. \r\n");
		return;
	}

	if (*arg1 == '+') {
		state = 1;
	} else if (*arg1 == '-') {
		state = 2;
	} else {
		send_to_char(editor, "Invalid flags.\r\n");
		return;
	}

	argument = one_argument(argument, arg1);
	old_flags = cur_flags = flags;
	while (*arg1) {
		if ((flag = search_block(arg1, help_bits, false)) == -1) {
			send_to_char(editor, "Invalid flag %s, skipping...\r\n", arg1);
		} else {
			tmp_flags = tmp_flags | (1 << flag);
		}
		argument = one_argument(argument, arg1);
	}
	REMOVE_BIT(tmp_flags, HFLAG_UNAPPROVED);
	if (state == 1) {
		flags |= tmp_flags;
	} else {
        flags &= ~tmp_flags;
	}

	tmp_flags = old_flags ^ flags;

	if (tmp_flags == 0) {
		send_to_char(editor, "Flags for help item %d not altered.\r\n", idnum);
	} else {
		SET_BIT(flags, HFLAG_MODIFIED);
		send_to_char(editor, "[%s] flags %s for help item %d.\r\n", tmp_printbits(tmp_flags, help_bits),
			state == 1 ? "added" : "removed", idnum);
	}
}

// Crank up the text editor and lets hit it.
void
HelpItem_EditText(void)
{

	LoadText();
	SET_BIT(flags, HFLAG_MODIFIED);
	start_editing_text(editor->desc, &text, MAX_HELP_TEXT_LENGTH);
	SET_BIT(PLR_FLAGS(editor), PLR_OLC);

	act("$n begins to edit a help file.\r\n", true, editor, 0, 0, TO_ROOM);
}

// Sets the groups bitvector much like the
// flags in quests.
void
HelpItem_SetGroups(char *argument)
{
	int state, cur_groups = 0, tmp_groups = 0, flag = 0, old_groups = 0;
	char arg1[MAX_INPUT_LENGTH];
	argument = one_argument(argument, arg1);
	skip_spaces(&argument);
	if (!*argument) {
		send_to_char(editor, "Invalid group. \r\n");
		return;
	}

	if (*arg1 == '+') {
		state = 1;
	} else if (*arg1 == '-') {
		state = 2;
	} else {
		send_to_char(editor, "Invalid Groups.\r\n");
		return;
	}

	argument = one_argument(argument, arg1);
	old_groups = cur_groups = groups;
	while (*arg1) {
		if ((flag = search_block(arg1, help_group_names, false)) == -1) {
			send_to_char(editor, "Invalid group: %s, skipping...\r\n", arg1);
		} else {
			tmp_groups = tmp_groups | (1 << flag);
		}
		argument = one_argument(argument, arg1);
	}
	if (state == 1) {
		groups = cur_groups | tmp_groups;
	} else {
        groups = cur_groups & ~tmp_groups;
	}
	tmp_groups = old_groups ^ cur_groups;

	if (tmp_groups == 0) {
		send_to_char(editor, "Groups for help item %d not altered.\r\n", idnum);
	} else {
		send_to_char(editor, "[%s] groups %s for help item %d.\r\n",
                     tmp_printbits(tmp_groups, help_group_bits),
                     state == 1 ? "added" : "removed", idnum);
	}
	REMOVE_BIT(groups, HGROUP_HELP_EDIT);
	SET_BIT(flags, HFLAG_MODIFIED);
}

bool
HelpItem_IsInGroup(int thegroup)
{
	if (IS_SET(groups, thegroup))
		return true;
	return false;
}

// Don't call me Roger.
void
HelpItem_SetName(char *argument)
{
	skip_spaces(&argument);
	if (name)
		delete[]name;
	name = new char[strlen(argument) + 1];
	strcpy(name, argument);
	SET_BIT(flags, HFLAG_MODIFIED);
	if (editor)
		send_to_char(editor, "Name set!\r\n");
}

// Set the...um. keywords and stuff.
void
HelpItem_SetKeyWords(char *argument)
{
	skip_spaces(&argument);
	if (keys)
		delete[]keys;
	keys = new char[strlen(argument) + 1];
	strcpy(keys, argument);
	SET_BIT(flags, HFLAG_MODIFIED);
	if (editor)
		send_to_char(editor, "Keywords set!\r\n");
}

// Help Item
HelpItem_HelpItem()
{
	idnum = 0;
	next = NULL;
	next_show = NULL;
	editor = NULL;
	text = NULL;
	name = NULL;
	keys = NULL;
	Clear();
}

HelpItem_~HelpItem()
{
	delete[]text;
	delete[]keys;
	delete[]name;
}

void
SwapItems(HelpItem * A, HelpItem * Ap, HelpItem * B, HelpItem * Bp)
{
	// Anext and Bnext.
	HelpItem *An = NULL, *Bn = NULL;
	int id;

	A->LoadText();
	B->LoadText();
	A->editor = B->editor = NULL;

	// Grab next pointers
	An = A->Next();
	Bn = B->Next();
	// Swap the idnums
	id = A->idnum;
	A->idnum = B->idnum;
	B->idnum = id;

	// First case: Topics right next to each other.
	if (A->Next() == B) {
		if (Ap == NULL) {		// Right next to each other and A is the first
			Help->items = B;
			A->SetNext(Bn);
			B->SetNext(A);
		} else {				// A is not first
			A->SetNext(B->Next());
			Ap->SetNext(B);
			B->SetNext(A);
		}
		// If B was the last element
		// Reset the bottom pointer.
		if (Bn == NULL) {
			Help->bottom = A;
		}
		return;
	}
	// Second Case:
	// Topics aren't side by side.
	// Move A
	if (Ap == NULL) {			// A is first
		Help->items = B;
		A->SetNext(B->Next());
		Bp->SetNext(A);
	} else {					// A is not first
		Ap->SetNext(A->Next());
		A->SetNext(B->Next());
		Bp->SetNext(A);
	}
	//  Now move B
	if (Ap != NULL) {
		B->SetNext(Ap->Next());
		Ap->SetNext(B);
	} else {
		B->SetNext(An);
	}
	// If B was the last element
	// Reset the bottom pointer.
	if (Bn == NULL) {
		Help->bottom = A;
	}
}

// Clear out the item.
bool
HelpItem_Clear(void)
{
	counter = 0;
	flags = (HFLAG_UNAPPROVED | HFLAG_MODIFIED);
	groups = 0;

	if (text) {
		delete[]text;
		text = NULL;
	}
	if (name) {
		delete[]name;
		name = NULL;
	}
	if (keys) {
		delete[]keys;
		keys = NULL;
	}
	name = new char[32];
	strcpy(name, "A New Help Entry");
	keys = new char[32];
	strcpy(keys, "new help entry");
	return true;
}

// Begin editing an item
// much like olc oedit.
// Sets yer currently editable item to this one.
bool
HelpItem_Edit(Creature * ch)
{
	if (editor) {
		if (editor != ch) {
			send_to_char(ch, "%s is already editing that item. Tough!\r\n",
				GET_NAME(editor));
		} else {
			send_to_char(ch,
				"I don't see how editing it _again_ will help any.\r\n");
		}
		return false;
	}
	if (GET_OLC_HELP(ch))
		GET_OLC_HELP(ch)->editor = NULL;
	GET_OLC_HELP(ch) = this;
	editor = ch;
	send_to_char(ch, "You are now editing help item #%d.\r\n", idnum);
	SET_BIT(flags, HFLAG_MODIFIED);
	return true;
}

// Saves the current HelpItem. Does NOT alter the index.
// You have to save that too.
// (SaveAll does everything)
bool
HelpItem_Save()
{
	char fname[256];
	ofstream file;
	sprintf(fname, "%s/%04d.topic", Help_Directory, idnum);
	// If we don't have the text to edit, get from the file.
	if (!text)
		LoadText();
	remove(fname);
	file.open(fname, ios_out | ios::trunc);
	if (!file && editor) {
		send_to_char(editor, "Error, could not open help file for write.\r\n");
		return false;
	}
	file << idnum << " " << (text ? strlen(text) : 0)
		<< endl << name << endl;
	if (text && strlen(text) > 0) {
		file.write(text, strlen(text));
		if( editor != NULL ) {
			delete[]text;
			text = NULL;
		}
	}
	file.close();
	REMOVE_BIT(flags, HFLAG_MODIFIED);
	return true;
}

// Loads in the text for a particular help entry.
bool
HelpItem_LoadText()
{
	char fname[256];
	int di;
	if (text)
		return true;

	text = new char[MAX_HELP_TEXT_LENGTH];
	strcpy(text, "");

	sprintf(fname, "%s/%04d.topic", Help_Directory, idnum);

	if( access(fname, F_OK) < 0 ) { // no file found. Likely just a new entry
		return true;
	}

	if ((access(fname, F_OK) >= 0) && (access(fname, W_OK) < 0)) {
		errlog("Help file (%s) is read-only.", fname);
		return false;
	}

	help_file.open(fname, ios_in);
	if (!help_file) {
		errlog("Unable to open help file to load text (%s).", fname);
		return false;
	}

	help_file >> di >> di;
	help_file.getline(fname, 256, '\n');	// eat the \r\n at the end of the #s
	help_file.getline(fname, 256, '\n');	// then burn up the name since we don't really need it.
	if (di > MAX_HELP_TEXT_LENGTH - 1)
		di = MAX_HELP_TEXT_LENGTH - 1;
	if (di > 0) {
		help_file.read(text, di);
		text[di] = '\0';
	}
	help_file.close();
	return true;
}

// Show the entry.
// buffer is output buffer.
void
HelpItem_Show(Creature * ch, char *buffer, int mode)
{
	char bitbuf[256];
	char groupbuf[256];
	switch (mode) {
	case 0:					// 0 == One Line Listing.
		sprintf(buffer, "Name: %s\r\n    (%s)\r\n", name, keys);
		strcpy(buffer, "");
		break;
	case 1:					// 1 == One Line Stat
		sprintbit(flags, help_bit_descs, bitbuf);
		sprintbit(groups, help_group_bits, groupbuf);
		strcpy(buffer, "");
		sprintf(buffer, "%s%3d. %s%-25s %sGroups: %s%-20s %sFlags:%s %s\r\n",
                CCCYN(ch, C_NRM), idnum, CCYEL(ch, C_NRM),
                name, CCCYN(ch, C_NRM), CCNRM(ch, C_NRM),
                groupbuf, CCCYN(ch, C_NRM), CCNRM(ch, C_NRM), bitbuf);
		break;
	case 2:					// 2 == Entire Entry
		if (!text)
			LoadText();
		strcpy(buffer, "");
		sprintf(buffer, "\r\n%s%s%s\r\n%s\r\n",
			CCCYN(ch, C_NRM), name, CCNRM(ch, C_NRM), text);
		counter++;
		break;
	case 3:					// 3 == Entire Entry Stat
		if (!text)
			LoadText();
		sprintbit(flags, help_bit_descs, bitbuf);
		sprintbit(groups, help_group_bits, groupbuf);
		strcpy(buffer, "");
		sprintf(buffer,
			"\r\n%s%d. %s%-25s %sGroups: %s%-20s %sFlags:%s %s \r\n        %s"
			"Keywords: [ %s%s%s ]\r\n%s%s\r\n",
			CCCYN(ch, C_NRM), idnum, CCYEL(ch, C_NRM),
			name, CCCYN(ch, C_NRM), CCNRM(ch, C_NRM),
			groupbuf, CCCYN(ch, C_NRM),
			CCNRM(ch, C_NRM), bitbuf, CCCYN(ch, C_NRM),
			CCNRM(ch, C_NRM), keys, CCCYN(ch, C_NRM), CCNRM(ch, C_NRM), text);
		break;
	default:
		break;
	}
}
