//
// File: maileditor.cc                        -- part of TempusMUD
// 

#include <string>
#include <list>
using namespace std;
#include <ctype.h>
#include <fcntl.h>
// Tempus Includes
#include "screen.h"
#include "desc_data.h"
#include "comm.h"
#include "db.h"
#include "utils.h"
#include "login.h"
#include "interpreter.h"
#include "boards.h"
#include "mail.h"
#include "editor.h"
#include "tmpstr.h"
#include "accstr.h"
#include "help.h"
#include "comm.h"
#include "player_table.h"

CMailEditor::CMailEditor(descriptor_data *desc,
                         mail_recipient_data *recipients)
    :CEditor(desc, MAX_MAIL_SIZE)
{
    mail_to = recipients;

    SendStartupMessage();
    DisplayBuffer();
}

CMailEditor::~CMailEditor(void)
{
    mail_recipient_data *next_rcpt;

    while (mail_to) {
        next_rcpt = mail_to->next;
        free(mail_to);
        mail_to = next_rcpt;
    }
}

void
CMailEditor::DisplayBuffer(unsigned int start_line)
{
    if (start_line == 1) {
        ListRecipients();
        send_to_desc(desc, "\r\n");
    }
    CEditor::DisplayBuffer(start_line);
}

bool
CMailEditor::PerformCommand(char cmd, char *args)
{
    switch (cmd) {
	case 't':
		if (PLR_FLAGGED(desc->creature, PLR_MAILING)) {
			ListRecipients();
			break;
		}
	case 'a':
		if (PLR_FLAGGED(desc->creature, PLR_MAILING)) {
			AddRecipient(args);
			break;
		}
	case 'e':
		if (PLR_FLAGGED(desc->creature, PLR_MAILING)) {
			RemRecipient(args);
			break;
		}
    default:
        return CEditor::PerformCommand(cmd, args);
    }

    return true;
}

void
CMailEditor::Finalize(const char *text)
{
	int stored_mail = 0;
	struct descriptor_data *r_d;
	struct mail_recipient_data *mail_rcpt = NULL;
    list<string> cc_list;

	// If they're trying to send a blank message
	if (!*text) {
		SendMessage("Why would you send a blank message?\r\n");
        return;
	}

    for (mail_rcpt = mail_to; mail_rcpt; mail_rcpt = mail_rcpt->next) {
        cc_list.push_back(playerIndex.getName(mail_rcpt->recpt_idnum));
	}
    cc_list.sort();
    cc_list.unique();

 	mail_rcpt = mail_to;
	while (mail_rcpt) {
		mail_to = mail_to->next;
		free(mail_rcpt);
		mail_rcpt = mail_to;
	}
   
    list<string>::iterator si;
    for (si = cc_list.begin(); si != cc_list.end(); si++) {
        long id = playerIndex.getID(si->c_str());
        stored_mail = store_mail(id, GET_IDNUM(desc->creature), text,  cc_list);
        if (stored_mail == 1) {
            for (r_d = descriptor_list; r_d; r_d = r_d->next) {
                if (IS_PLAYING(r_d) && r_d->creature &&
                    (r_d->creature != desc->creature) &&
                    (GET_IDNUM(r_d->creature) == id) &&
                    (!PLR_FLAGGED(r_d->creature, PLR_WRITING | PLR_MAILING | PLR_OLC))) {
                    send_to_char(r_d->creature, "A strange voice in your head says, "
                                                "'You have new mail.'\r\n");
                }
            }
        }
    }

	if (stored_mail) {
		SendMessage("Message sent!\r\n");
    }
    else {
        SendMessage("Your message was not received by one or more recipients.\r\n"
                    "Please try again later!\r\n");
        errlog("store_mail() has returned <= 0");
    } 
    if (GET_LEVEL(desc->creature) >= LVL_AMBASSADOR) {
        // Presumably, this message is only displayed for imms for
        // pk avoidance reasons.
        act("$n postmarks and dispatches $s mail.", false, desc->creature, 0, 0, TO_NOTVICT);
    }
    
}

void
CMailEditor::ListRecipients(void)
{
	struct mail_recipient_data *mail_rcpt = NULL;

    send_to_desc(desc, "    &yTo&b: &c");
	for (mail_rcpt = mail_to; mail_rcpt;mail_rcpt = mail_rcpt->next) {
        send_to_desc(desc, tmp_capitalize(playerIndex.getName(mail_rcpt->recpt_idnum)));
        if (mail_rcpt->next)
            send_to_desc(desc, ", ");
	}
    send_to_desc(desc, "&n\r\n");
}

void
CMailEditor::AddRecipient(char *name)
{
	long new_id_num = 0;
	struct mail_recipient_data *cur = NULL;
	struct mail_recipient_data *new_rcpt = NULL;
	const char *money_desc;
	int money, cost;

	new_id_num = playerIndex.getID(name);
	if (!new_id_num) {
		SendMessage("Cannot find anyone by that name.\r\n");
		return;
	}

	new_rcpt = (mail_recipient_data *)malloc(sizeof(mail_recipient_data));
	new_rcpt->recpt_idnum = new_id_num;
	new_rcpt->next = NULL;

	// Now find the end of the current list and add the new cur
	cur = mail_to;
	while (cur && cur->recpt_idnum != new_id_num && cur->next)
		cur = cur->next;

	if (cur->recpt_idnum == new_id_num) {
		SendMessage(tmp_sprintf(
			"%s is already on the recipient list.\r\n",
			tmp_capitalize(playerIndex.getName(new_id_num))));
		free(new_rcpt);
		return;
	}

	if (GET_LEVEL(desc->creature) < LVL_AMBASSADOR) {
		//mailing the grimp, charge em out the ass
		if (desc->creature->in_room->zone->time_frame == TIME_ELECTRO) {
			money_desc = "creds";
			money = GET_CASH(desc->creature);
		} else {
			money_desc = "gold";
			money = GET_GOLD(desc->creature);
		}

		if (new_id_num == 1)
			cost = 1000000;
		else
			cost = STAMP_PRICE;

		if (money < cost) {
			SendMessage(tmp_sprintf("You don't have the %d %s necessary to add %s.\r\n",
                                    cost, money_desc, 
                                    tmp_capitalize(playerIndex.getName(new_id_num))));
			free(new_rcpt);
			return;
		} else {
			SendMessage(tmp_sprintf("%s added to recipient list.  You have been charged %d %s.\r\n",
                                    tmp_capitalize(playerIndex.getName(new_id_num)), cost,
                                    money_desc));
			if (desc->creature->in_room->zone->time_frame == TIME_ELECTRO)
				GET_CASH(desc->creature) -= cost;
			else
				GET_GOLD(desc->creature) -= cost;
		}
	} else {
        SendMessage(tmp_sprintf("%s added to recipient list.\r\n",
                                tmp_capitalize(playerIndex.getName(new_id_num))));
	}
	cur->next = new_rcpt;
	ListRecipients();
}

void
CMailEditor::RemRecipient(char *name)
{
	int removed_idnum = -1;
	struct mail_recipient_data *cur = NULL;
	struct mail_recipient_data *prev = NULL;
	char *msg;

	removed_idnum = playerIndex.getID(name);

	if (!removed_idnum) {
		SendMessage("Cannot find anyone by that name.\r\n");
		return;
	}
	// First case...the mail only has one recipient
	if (!mail_to->next) {
		SendMessage("You cannot remove the last recipient of the letter.\r\n");
		return;
	}
	// Second case... Its the first one.
	if (mail_to->recpt_idnum == removed_idnum) {
		cur = mail_to;
		mail_to = mail_to->next;
		free(cur);
		if (desc->creature->in_room->zone->time_frame == TIME_ELECTRO) {
			if (GET_LEVEL(desc->creature) < LVL_AMBASSADOR) {

				if (removed_idnum == 1) {	//fireball :P
                    msg = tmp_sprintf(
						"%s removed from recipient list.  %d credits have been refunded.\r\n",
						tmp_capitalize(playerIndex.getName(removed_idnum)), 1000000);
					GET_CASH(desc->creature) += 1000000;	//credit mailer for removed recipient 
				} else {
                    msg = tmp_sprintf(
						"%s removed from recipient list.  %d credits have been refunded.\r\n",
						tmp_capitalize(playerIndex.getName(removed_idnum)), STAMP_PRICE);
					GET_CASH(desc->creature) += STAMP_PRICE;	//credit mailer for removed recipient 
				}
			} else {
                msg = tmp_sprintf("%s removed from recipient list.\r\n",
					tmp_capitalize(playerIndex.getName(removed_idnum)));
			}
		} else {				//not in the future, refund gold
			if (GET_LEVEL(desc->creature) < LVL_AMBASSADOR) {
				if (removed_idnum == 1) {	//fireball :P
                    msg = tmp_sprintf(
						"%s removed from recipient list.  %d gold has been refunded.\r\n",
						tmp_capitalize(playerIndex.getName(removed_idnum)), 1000000);
					GET_GOLD(desc->creature) += 1000000;	//credit mailer for removed recipient 
				} else {
					msg = tmp_sprintf(
						"%s removed from recipient list.  %d gold has been refunded.\r\n",
						tmp_capitalize(playerIndex.getName(removed_idnum)), STAMP_PRICE);
					GET_GOLD(desc->creature) += STAMP_PRICE;	//credit mailer for removed recipient 
				}
			} else {
                msg = tmp_sprintf("%s removed from recipient list.\r\n",
					tmp_capitalize(playerIndex.getName(removed_idnum)));
			}
		}
		SendMessage(msg);
		return;
	}
	// Last case... Somewhere past the first recipient.
	cur = mail_to;
	// Find the recipient in question
	while (cur && cur->recpt_idnum != removed_idnum) {
		prev = cur;
		cur = cur->next;
	}
	// If the name isn't in the recipient list
	if (!cur) {
		SendMessage
			("You can't remove someone who's not on the list genious.\r\n");
		return;
	}
	// Link around the recipient to be removed.
	prev->next = cur->next;
	free(cur);
	if (desc->creature->in_room->zone->time_frame == TIME_ELECTRO) {
		if (GET_LEVEL(desc->creature) < LVL_AMBASSADOR) {
			if (removed_idnum == 1) {	//fireball :P
				msg = tmp_sprintf(
					"%s removed from recipient list.  %d credits have been refunded.\r\n",
					tmp_capitalize(playerIndex.getName(removed_idnum)), 1000000);
				GET_CASH(desc->creature) += 1000000;	//credit mailer for removed recipient 
			} else {
				msg = tmp_sprintf(
					"%s removed from recipient list.  %d credits have been refunded.\r\n",
					tmp_capitalize(playerIndex.getName(removed_idnum)), STAMP_PRICE);
				GET_CASH(desc->creature) += STAMP_PRICE;	//credit mailer for removed recipient 
			}
		} else {
			msg = tmp_sprintf("%s removed from recipient list.\r\n",
				tmp_capitalize(playerIndex.getName(removed_idnum)));
		}
	} else {					//not in future, refund gold
		if (GET_LEVEL(desc->creature) < LVL_AMBASSADOR) {
			if (removed_idnum == 1) {	//fireball :P
                msg = tmp_sprintf(
					"%s removed from recipient list.  %d gold has been refunded.\r\n",
					tmp_capitalize(playerIndex.getName(removed_idnum)), 1000000);
				GET_GOLD(desc->creature) += 1000000;	//credit mailer for removed recipient 
			} else {
                msg = tmp_sprintf(
					"%s removed from recipient list.  %d gold has been refunded.\r\n",
					tmp_capitalize(playerIndex.getName(removed_idnum)), STAMP_PRICE);
				GET_GOLD(desc->creature) += STAMP_PRICE;	//credit mailer for removed recipient 
			}
		} else {
			msg = tmp_sprintf("%s removed from recipient list.\r\n",
				tmp_capitalize(playerIndex.getName(removed_idnum)));
		}
	}
	SendMessage(msg);

	return;
}