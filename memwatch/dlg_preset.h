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

/*
 * This function displays a dialog that allows user to edit a
 * given preset. Returns 1 if the user input is correct and 
 * OK was pressed, and 0 if the dialog was dismissed. 
 */
int winvalue_do_dialog(VariablePreset *preset);

/* Callbacks for this dialog */
void winvalue_add_on_click(GtkButton *);
void winvalue_cancel_on_click(GtkButton*);

void winvalue_hotkey_on_realize(GtkEntry*);
void winvalue_hotkey_focus_in(GtkEntry*);
void winvalue_hotkey_focus_out(GtkEntry*);

void winvalue_value_on_realize(GtkEntry*);
void winvalue_value_focus_out(GtkEntry*);

void winvalue_description_on_realize(GtkEntry*);
void winvalue_description_focus_out(GtkEntry*);