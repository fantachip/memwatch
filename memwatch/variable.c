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


#include "main.h"
#include <pcre.h>

struct data_type {
	const char *name;
	DataType_t type;
} types[] = {
	{"int", DATA_TYPE_INT},
	{"float", DATA_TYPE_FLOAT},
	{"char", DATA_TYPE_BYTE}
};

const char *DataType_toString(DataType_t type){
	int c;
	// look up the type and set it
	for(c=0;c<(sizeof(types)/sizeof(struct data_type));c++){
		if(types[c].type == type)
			return types[c].name;
	}
	assert(0);
	return 0;
}

DataType_t DataType_fromString(const char *string){
    int c;
    
	assert(string);

	// look up the type and set it
	for(c=0;c<(sizeof(types)/sizeof(struct data_type));c++){
		if(strcmp(types[c].name, string)==0)
			return types[c].type;
	}
	assert(0);
	return -1;
}

// VariablePath implementation {{{
void VariablePath_init(VariablePath *self){
	assert(self);

	memset(self, 0, sizeof(VariablePath));
}

VariablePath *VariablePath_new(){
	return (VariablePath*)calloc(1, sizeof(VariablePath));
}

void VariablePath_copy(const VariablePath *self, VariablePath *dst){
	assert(self);

	strcpy(dst->module, self->module);
	dst->address = self->address;
	if (self->next){
		dst->next = (VariablePath*)calloc(1, sizeof(VariablePath));
		VariablePath_copy(self->next, dst->next);
	}
}

void VariablePath_free(VariablePath *self){
	if(self->next)
		VariablePath_free(self->next);
}

/**
 * Parses a location string. A few examples of valid strings are:
 * 0x123 <= value at address 0x123
 * [0x123]+1 <= value at address pointed to by (value at 0x123)+1
 * module+1 <= value at address module+1
 * [module+1] <= value at address module+1
 */

// Parser implementation {{{

// Parser data types {{{
#define MAX_TOKEN_LENGTH 128
typedef enum {
	TOKEN_NONE,
	TOKEN_OPEN,
	TOKEN_CLOSE,
	TOKEN_INT,
	TOKEN_HEX,
	TOKEN_PLUS,
	TOKEN_MODULE
}token_type_t;

typedef struct parser_s{
	const char *original;
	const char *current;
}parser_t;

typedef struct token_s{
	token_type_t 	type;
	char string[MAX_TOKEN_LENGTH];
	int length;
}token_t;

struct keyword_s{
	const char *string;
	token_type_t type;
} keywords[] = {
	{"[", TOKEN_OPEN},
	{"]", TOKEN_CLOSE},
	{"+", TOKEN_PLUS}
};
// }}}

void parse_init(parser_t *self, const char *string){
	self->original = string;
	self->current = string;
}

int read_keyword(parser_t *self, token_t *token){
    int c;
	for(c=0;c<sizeof(keywords)/sizeof(struct keyword_s);c++){
		// Skip spaces
		if(*self->current == ' '){
			self->current++;
			continue;
		}
		// Try to read in a keyword
		if(strncmp(self->current, keywords[c].string, strlen(keywords[c].string))==0){
			strcpy(token->string, keywords[c].string);
			token->length = (int)strlen(token->string);
			self->current+=token->length;
			token->type = keywords[c].type;
			return 1;
		}
	}
	return 0;
}

//Validators {{{

int is_next_keyword(parser_t *self){
    int c;
	for(c=0;c<sizeof(keywords)/sizeof(struct keyword_s);c++){
		if(strncmp(self->current, keywords[c].string, strlen(keywords[c].string))==0)
			return 1;
	}
	return 0;
}

static const char *errptr;
static int errofs;

int is_value_int(const char *value){
	static pcre *re = 0;
	if(!re)
		re = pcre_compile("^\\d*$", 0, &errptr, &errofs, NULL);
	if(pcre_exec(re, NULL, value, (int)strlen(value), 0, 0, 0, 0) >= 0)
		return 1;
	return 0;
}

int is_value_hex(const char *value){
	static pcre *re_hex = 0;
	if(!re_hex)
		re_hex = pcre_compile("^0x[0-9a-fA-F]*$", 0, &errptr, &errofs, NULL);
	if(pcre_exec(re_hex, NULL, value, (int)strlen(value), 0, 0, 0, 0) >= 0)
		return 1;
	return 0;

}

int is_value_decimal(const char *value){
	static pcre *re = 0;
	if(!re)
		re = pcre_compile("^[\\d]*[.]?[\\d]+$", 0, &errptr, &errofs, NULL);
	if(pcre_exec(re, NULL, value, (int)strlen(value), 0, 0, 0, 0) >= 0)
		return 1;
	return 0;
}


int is_value_module(const char *value){
	static pcre *re_module = 0;
	if(!re_module)
		re_module = pcre_compile("^[a-zA-Z0-9.]*$", 0, &errptr, &errofs, NULL);
	if(pcre_exec(re_module, NULL, value, (int)strlen(value), 0, 0, 0, 0) >= 0)
		return 1;
	return 0;
}
//}}}

int parse_next_token(parser_t *self, token_t *token){
	assert(self && token);
	
	memset(token, 0, sizeof(token_t));
	
	// Try to read a keyword
	if(read_keyword(self, token))
		return 1;
	
	// Otherwise try to read a value
	while(!is_next_keyword(self)){
		if(*self->current==0 || token->length==MAX_TOKEN_LENGTH)
			break;
		// Skip spaces
		if(*self->current==' '){
			self->current++;
			continue;
		}
		// Copy the value one byte at a time
		token->string[token->length]=*self->current;
		token->length++;
		self->current++;
	}
	token->string[token->length]=0;

	if(!token->length)
		return -1;
	
	// Validate the token
	if(is_value_hex(token->string)){
		token->type = TOKEN_HEX;
		return 1;
	} else if(is_value_int(token->string)){
		token->type = TOKEN_INT;
		return 1;
	} else if(is_value_module(token->string)){
		token->type = TOKEN_MODULE;
		return 1;
	} else {
		return 0;
	}
	return 0;
}

void parse_return_token(parser_t *self, token_t *token){
	self->current-=token->length;
}

int parse_VariablePath(VariablePath *self, parser_t *parser, token_t *token){
	int ret;
	while((ret = parse_next_token(parser, token))!=0){
		if(ret<0)
			return 1; // completed
		
		if(token->type==TOKEN_OPEN){
			self->next = (VariablePath*)calloc(1, sizeof(VariablePath));
			VariablePath_init(self->next);
			parse_VariablePath(self->next, parser, token);
			// Expect a closing brace
			if(parse_next_token(parser, token)<=0 || token->type!=TOKEN_CLOSE)
				return 0;
			// The only thing allowed after a closing brace is plus or end of line
			if(parse_next_token(parser, token) && token->type!=TOKEN_PLUS)
				return 0;
		} 
		else if(token->type==TOKEN_CLOSE){
			// Handling the closing brace is the job of previous level
			parse_return_token(parser, token);
			return 1; 
	 	} 
		else if(token->type == TOKEN_MODULE){
			// Module name
			strncpy(self->module, token->string, sizeof(self->module));
			// Check if there is also an adress after module name
			if(parse_next_token(parser, token)>0){
				if(token->type==TOKEN_PLUS){
					if(parse_next_token(parser, token)>0){
						if(token->type==TOKEN_HEX)
							sscanf(token->string, "%x", (int*)&self->address);
						else if(token->type==TOKEN_INT)
							sscanf(token->string, "%d", (int*)&self->address);
					} else { // any other token is unexpected
						return 0;
					}
				} else { // Return any token other then plus
					parse_return_token(parser, token);
				}
			}
		} 
		else if(token->type == TOKEN_INT){
			// A base address represented by an int 
			sscanf(token->string, "%d", (int*)&self->address);
		} 
		else if(token->type == TOKEN_HEX){
			// A base address in hex
			sscanf(token->string, "%x", (int*)&self->address);
		}
		else // Unexpected token
			return 0;
	}
	return 0;
}

//}}}

int VariablePath_compile(VariablePath *self, const char *str){
	parser_t parser;
	token_t token;

	//Parse the expression
	parse_init(&parser, str);
	
	VariablePath_free(self);
	VariablePath_init(self);

	return parse_VariablePath(self, &parser, &token);
}

int VariablePath_toString(VariablePath *self, char *string, int length){
	int len, orig_len=length;

	// Convert the location into a string
	if(self->next){
		snprintf(string++, length--, "[");
		len = VariablePath_toString(self->next, string, length);
		string+=len; length-=len;
		snprintf(string, length, "]+");
		string+=2; length-=2;
	}
	if(strlen(self->module)){
		len = snprintf(string, length, "%s+", self->module);
		string += len; length-=len;
	}
	length -= snprintf(string, length, "0x%x", (long long)self->address);
	return orig_len-length;
}

//}}}

// Variable implementation {{{
void Variable_init(Variable *self){
	assert(self);

	// Delete any pointers that are set
	VariablePath_free(&self->path);

	memset(self, 0, sizeof(Variable));
}

void Variable_copy(Variable *self, Variable *dst){
	int c;
	assert(self && dst);

	dst->type = self->type;
	memcpy(dst->description, self->description, sizeof(dst->description));

	VariablePath_copy(&self->path, &dst->path);

	for(c=0;c<VARIABLE_MAX_PRESETS;c++){
		if(self->presets[c]){
			dst->presets[c]=malloc(sizeof(VariablePreset));
			memcpy(dst->presets[c], self->presets[c], sizeof(VariablePreset));
		}
	}
}

void Variable_free(Variable *self){
	assert(self);

	VariablePath_free(&self->path);
	memset(self, 0, sizeof(Variable));
}

size_t Variable_size(Variable *self){
	switch(self->type){
		case DATA_TYPE_INT:
			return sizeof(int);
		case DATA_TYPE_FLOAT:
			return sizeof(float);
		case DATA_TYPE_BYTE:
			return sizeof(char);
		default:
			assert(0); // error
	}
	return 0;
}

/* Converts current value of the variable into its binary representation */
int Variable_toBinary(Variable *self, void *data){
	address_t address = 0;
	int size;
	switch(self->type){
		case DATA_TYPE_INT:
			if(is_value_int(self->value))
				sscanf(self->value, "%d", (int*)data);
			else if(is_value_hex(self->value))
				sscanf(self->value, "0x%x", (int*)data);
			else
				return FALSE;
			break;
		case DATA_TYPE_FLOAT:
			if(is_value_decimal(self->value))
				sscanf(self->value, "%f", (float*)data);
			else
				return FALSE;
			break;
		case DATA_TYPE_BYTE:
			if(is_value_decimal(self->value)){
				int tmp;
				sscanf(self->value, "%d", &tmp);
				*((char*)data)=tmp&0xff;
			}
			else if(is_value_hex(self->value)){
				int tmp;
				sscanf(self->value, "0x%x", &tmp);
				*((char*)data)=tmp&0xff;
			}
			else
				return FALSE;
			break;
		default:
			break;
	}

	return TRUE;
}

/* Reads the actual value from process memory */
void Variable_fromBinary(Variable *self, void *data, gboolean hex_view){
	assert(self && data);

	// Read the actual data
	if(self->type==DATA_TYPE_INT){
		int val = *((int*)data);
		if(hex_view)
			snprintf(self->value, sizeof(self->value), "0x%08x", val);
		else
			snprintf(self->value, sizeof(self->value), "%d", val);
	}
	else if(self->type == DATA_TYPE_FLOAT){
		float val = *((float*)data);
		snprintf(self->value, sizeof(self->value), "%f", val);
	}
	else if(self->type == DATA_TYPE_BYTE){
		char val = *((char*)data);
		if(hex_view)
			snprintf(self->value, sizeof(self->value), "0x%02x", val);
		else
			snprintf(self->value, sizeof(self->value), "%d", val);
	}
	else
		assert(0); //error
}

int Variable_set_preset_by_hotkey(Variable *self, unsigned int key){
	//loop through all values and set the first one with supplied hotkey
	int c;
	for(c=0;c<VARIABLE_MAX_PRESETS;c++){
		if(self->presets[c] && self->presets[c]->hotkey == key){
			strncpy(self->value, self->presets[c]->value, sizeof(self->value));
			return 1;
		}
	}
	return 0;
}

Handle_t Variable_add_preset(Variable *self, VariablePreset *value){
	int c;
	assert(self && value);
	for(c=0;c<VARIABLE_MAX_PRESETS;c++){
		if(!self->presets[c]){
			self->presets[c] = malloc(sizeof(VariablePreset));
			memcpy(self->presets[c], value, sizeof(VariablePreset));
			return c;
		}
	}
	assert(0);
	return -1;
}

VariablePreset *Variable_get_preset(Variable *self, Handle_t handle){
	assert(handle>=0 && handle<VARIABLE_MAX_PRESETS);

	return self->presets[handle];
}

void Variable_remove_preset(Variable *self, Handle_t handle){
	assert(handle>=0 && handle<VARIABLE_MAX_PRESETS);

	if(self->presets[handle]){
		free(self->presets[handle]);
		self->presets[handle]=0;
	}
}
void Variable_foreach_preset(Variable *self, void (*callback)(VariablePreset *val, Handle_t handle, gpointer data), gpointer data){
	int c;
	for(c=0;c<VARIABLE_MAX_PRESETS;c++){
		if(self->presets[c])
			callback(self->presets[c], c, data);
	}
}

//}}}
