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

#define VARIABLE_MAX_PRESETS 32

typedef enum{
	DATA_TYPE_INT,
	DATA_TYPE_FLOAT,
	DATA_TYPE_BYTE,
	DATA_TYPE_COUNT
}DataType_t;

/*******
 * Representation of the pointer chain path to the actual memory value. Eg:
 * A location in memory can pint to a value which in turn is a pointer to the
 * actual variable. 
 */
typedef struct VariablePath_s{
	char 	module[256];
	address_t address;
	struct VariablePath_s *next;
}VariablePath;

typedef struct VariablePreset_s{
	unsigned int    hotkey;
	char            description[256];
	char            value[256];
}VariablePreset;

/*******
 * This structure holds information about the location of a variable, it's name,
 * it's value presets and it's type. 
 */
typedef struct Variable_s{
	DataType_t         type;
	VariablePath       path;
	char               value[256];
	char               description[256];
	int                read_only;
	VariablePreset     *presets[VARIABLE_MAX_PRESETS];
}Variable;


const char *DataType_toString(DataType_t type);
DataType_t DataType_fromString(const char *string);

/*****
 * Functions to handle variable paths
 */
void VariablePath_init(VariablePath *self);
void VariablePath_copy(const VariablePath *src, VariablePath *dst);
void VariablePath_free(VariablePath *loc);

/* Compile a string representation of a memory location into a VariablePath
 * structure. Path expression represents a chain of pointers where value
 * [number] is interpreted as the value of memory location at address "number".
 * It is possible to add ofsets to the memory locations and also use module base
 * addresses instead of constant values to retreive the final memory location. A
 * few valid path expressions:
 * [SomeDLL.dll+0x24]+23
 * [0x401000]+0x100
 */
int VariablePath_compile(VariablePath *self, const char *expr);
/* This function does the opposite of the function above. */
int VariablePath_toString(VariablePath *self, char *string, int length);

/* 
 * Implementation of the Variable object 
 */
void Variable_copy(Variable *self, Variable *dst);
void Variable_init(Variable *self);
void Variable_free(Variable *self);

// Returns size in bytes depending on the type of variable
size_t Variable_size(Variable *self);

void Variable_fromBinary(Variable *self, void *data, gboolean hex_view);
int Variable_toBinary(Variable *self, void *data);

Handle_t Variable_add_preset(Variable *self, VariablePreset *value);
void Variable_remove_preset(Variable *self, Handle_t handle);
VariablePreset *Variable_get_preset(Variable *self, Handle_t handle);
void Variable_foreach_preset(Variable *self, void (*callback)(VariablePreset *val, Handle_t handle, gpointer data), gpointer data);
// Sets the first preset that maps to the given hotkey
int Variable_set_preset_by_hotkey(Variable *self, unsigned int hotkey);