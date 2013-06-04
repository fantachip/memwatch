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

/*********************************
 * A profile is basically the root object that represents current state of the
 * application. The main window reads and displays a profile object. A profile
 * can be saved into a file and loaded again later.  
 *********************************/

// Defines maximum number of variables that can be referenced inside a target
// process. 
#define PROFILE_MAX_VARIABLES 128

// Profile information
typedef struct Profile_s{
	char 		process_name[256];
	Variable 	*variables[PROFILE_MAX_VARIABLES];
	int         modified;
}Profile;

//Adds a variable into the profile and returns its id
Handle_t Profile_add_variable(Profile *self, const Variable *var);
//Retreive a pointer to the variable objec
Variable *Profile_get_variable(Profile *self, Handle_t id);
//Deletes the variable from the list
void Profile_remove_variable(Profile *self, Handle_t id);
//Executes a callback for each variable inside the profile
void Profile_foreach_variable(Profile *self, void(*callback)(Variable *var, Handle_t handle, gpointer data), gpointer data);

/*
 * Functions to save and load profiles
 */
int Profile_save(Profile *self, const char *filename);
int Profile_load(Profile *self, const char *filename);

