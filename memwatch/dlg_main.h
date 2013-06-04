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

void winmain_init();
void winmain_quit();

void winmain_varlist_on_realize(GtkTreeView *view);
void winmain_varlist_on_button_press(GtkTreeView *treeview, GdkEventButton *event);
void winmain_varlist_on_right_click(GtkTreeView *list, GdkEventButton *event);

void winmain_process_list_on_realize(GtkComboBox *box);
void winmain_process_list_on_change(GtkComboBox *box);
void winmain_process_list_update();

int winmain_save();
void winmain_load();
void winmain_menu_hex_on_activate(GtkMenuItem *item);

void winmain_update_list();
void winmain_update_data();

