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

int winaddptr_do_dialog(Variable *variable);
void winaddptr_on_close();
void winaddptr_add_on_click(GtkWidget *button);
void winaddptr_cancel_on_click(GtkWidget *button);
void winaddptr_values_on_init(GtkTreeView *view);
void winaddptr_values_on_right_click(GtkTreeView *view);
void winaddptr_on_type_realize(GtkComboBox *box);
void winaddptr_on_type_changed(GtkComboBox *box);
void winaddptr_on_location_realize(GtkEntry *box);
void winaddptr_on_location_focus_out(GtkEntry *box);
void winaddptr_on_description_realize(GtkEntry *box);
void winaddptr_on_description_changed(GtkEntry *box);
void winaddptr_on_auto_update_toggled(GtkToggleButton *);

void winaddptr_update_list(GtkTreeView *view);

