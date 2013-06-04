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

typedef struct Process_s{
	char 		executable[256];
	int			pid;
	Handle_t	handle;
}Process;

struct Variable;

void sys_init();
void sys_finish();

void sys_vkcode_to_string(unsigned int code, char *string, int size);
unsigned int sys_add_keyboard_hook(void (*callback)(unsigned int, gpointer),gpointer);
void sys_remove_keyboard_hook(unsigned int);
int sys_foreach_process(void (*callback)(Process *proc, void *data), void *data);

/* 
 * Functions to read and write values to foregn processes. 
 */
int Process_write(Process *self, Variable *value);
int Process_read(Process *self, Variable *dst, int hex);

int Process_open(Process *self, const char *name);

address_t Process_get_module_address(Process *self, const char *name);