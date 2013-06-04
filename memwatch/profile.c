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


Handle_t Profile_add_variable(Profile *self, const Variable *loc){
	int c;

	assert(self && loc);

	self->modified=1;
	// find a free slot and copy the variable into it
	for(c=0;c<PROFILE_MAX_VARIABLES;c++){
		if(self->variables[c]==0){
			self->variables[c]=(Variable*)calloc(1, sizeof(Variable));
			Variable_copy((Variable*)loc, self->variables[c]);
			return c;
		}
	}
	assert(0);
	return -1;
}

Variable *Profile_get_variable(Profile *self, Handle_t id){
	assert(self && id>=0 && id < PROFILE_MAX_VARIABLES);

	return self->variables[id];
}

void Profile_remove_variable(Profile *self, Handle_t id){
	assert(self && id >= 0 && id <PROFILE_MAX_VARIABLES);

	self->modified=1;
	if(self->variables[id]){
		Variable_free(self->variables[id]);
		free(self->variables[id]);
		self->variables[id]=0;
	}
}

void Profile_foreach_variable(Profile *self, void(*callback)(Variable *var, Handle_t handle, gpointer data), gpointer data){
	int c;
	assert(self && callback);

	for(c=0;c<PROFILE_MAX_VARIABLES;c++){
		if(self->variables[c])
			callback(self->variables[c], c, data);
	}
}

int Profile_load(Profile *self, const char *file){
	FILE *f;
	char variable[256], value[256];
	Variable *var=0;
	VariablePreset *val=0;
	int c;

	f = fopen(file, "r");
	if(!f)
		return 0;

	// Clear the profile
	for(c=0;c<PROFILE_MAX_VARIABLES;c++){
		if(self->variables[c]){
			Variable_free(self->variables[c]);
			self->variables[c]=0;
		}
	}

	while(fscanf(f, "%[^#=]=%[^#\n]\n", variable, value)!=EOF){
		if(strcmp(variable, "process")==0)
			strcpy(self->process_name, value);
		else if(strcmp(variable, "section")==0){
			if(strcmp(value, "variable")==0){
				if(var){
					if(val)
						Variable_add_preset(var, val);
					Profile_add_variable(self, var);
					Variable_free(var);
					val=0;
				}
				var = (Variable*)calloc(1, sizeof(Variable));
			}
			else if(strcmp(value, "value")==0){
				if(val)
					Variable_add_preset(var, val);
				val = calloc(1, sizeof(VariablePreset));
			}
		}
		else if(strcmp(variable, "type")==0){
			var->type = DataType_fromString(value);
		}
		else if(strcmp(variable, "description")==0){
			strncpy(var->description, value, sizeof(var->description));
		}
		else if(strcmp(variable, "location")==0){
			if(!VariablePath_compile(&var->path, value))
				VariablePath_free(&var->path);
		}
		else if(strcmp(variable, "val_hotkey")==0){
			assert(val);
			sscanf(value, "%d", &val->hotkey);
		}
		else if(strcmp(variable, "val_description")==0){
			assert(val);
			strncpy(val->description, value, sizeof(val->description));
		}
		else if(strcmp(variable, "val_value")==0){
			assert(val);
			strncpy(val->value, value, sizeof(val->value));
		}
	}
	if(var){
		if(val)
			Variable_add_preset(var, val);
		Profile_add_variable(self, var);
		Variable_free(var);
	}
	fclose(f);
	self->modified=0;
	return 1;
}

int Profile_save(Profile *self, const char *file){
	FILE *f;
	int c,i;

	f = fopen(file, "w");
	if(!f)
		return 0;
	fprintf(f, "process=%s\n", self->process_name);
	for(c=0;c<PROFILE_MAX_VARIABLES;c++){
		if(self->variables[c]){
			Variable *var = self->variables[c];
			char loc[256];

			VariablePath_toString(&var->path, loc, 256);

			fprintf(f, "section=variable\n");
			fprintf(f, "type=%s\n", DataType_toString(var->type));
			fprintf(f, "description=%s\n", var->description);
			fprintf(f, "location=%s\n", loc);
			
			for(i=0;i<VARIABLE_MAX_PRESETS;i++){
				if(!var->presets[i])
					continue;
				fprintf(f, "section=value\n");
				fprintf(f, "val_hotkey=%d\n", var->presets[i]->hotkey);
				fprintf(f, "val_value=%s\n", var->presets[i]->value);
				fprintf(f, "val_description=%s\n", var->presets[i]->description);
			}
		}
	}
	fclose(f);
	self->modified=0;
	return 1;
}
