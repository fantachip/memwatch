/*
-----------------------------------------------------------------------------
This source file is part of MemWatch (An application to watch memory inside a
foregin process in real time) 

For the latest info, see http://www.schreder.nu/

Copyright (c) 2008 Martin K. Schreder <mkschreder@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.
-----------------------------------------------------------------------------
*/


#pragma once

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

typedef unsigned int Handle_t;
typedef unsigned int address_t;

#include "callbacks.h"
#include "interface.h"
#include "support.h"


#include "variable.h"
#include "system.h"
#include "profile.h"
#include "dlg_main.h"
#include "dlg_variable.h"
#include "dlg_preset.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif

extern GtkWidget *gWinAddPointer;
extern GtkWidget *gWinMain;

#define msg_error(message, ...) \
{\
	GtkWidget *dlg = gtk_message_dialog_new(\
		GTK_WINDOW(gWinAddPointer), \
		GTK_DIALOG_MODAL, \
		GTK_MESSAGE_ERROR, \
		GTK_BUTTONS_OK, message, ##__VA_ARGS__);\
	gtk_dialog_run(GTK_DIALOG(dlg));\
	gtk_widget_destroy(dlg);\
}

#define GET_TOPLEVEL(obj) \
	G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(obj)))

typedef struct menu_item_s{
	const char *name;
	const char *icon;
	void (*callback)(GtkWidget *, gpointer data);
	gpointer data;
}menu_item_t;

void util_create_popup_menu(menu_item_t *items, int count);
