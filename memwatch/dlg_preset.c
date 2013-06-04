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

typedef struct winvalue_data_s{
	unsigned int vk;
	char hotkey[256];
	char value[256];
	char description[256];
	unsigned int hook;
}winvalue_data_t;

int winvalue_do_dialog(VariablePreset *dst){
	GtkWidget *win = create_win_value();
	int ret = 0;
	winvalue_data_t *win_data = calloc(1, sizeof(winvalue_data_t));

	assert(dst);

	// set the data
	if(strlen(dst->description)){
		strncpy(win_data->description, dst->description, sizeof(win_data->description));
	}
	strcpy(win_data->value, dst->value);
	win_data->vk=dst->hotkey;
	sys_vkcode_to_string(dst->hotkey, win_data->hotkey, sizeof(win_data->hotkey));

	g_object_set_data(G_OBJECT(win), "value", dst);
	g_object_set_data(G_OBJECT(win), "tmp", win_data);

	ret = gtk_dialog_run(GTK_DIALOG(win));
	free(win_data);
	gtk_widget_destroy(win);

	return (ret)?TRUE:FALSE;
}

void winvalue_add_on_click(GtkButton *widget){
	VariablePreset *val = g_object_get_data(GET_TOPLEVEL(widget), "value");
	winvalue_data_t *data = (winvalue_data_t*)g_object_get_data(GET_TOPLEVEL(widget), "tmp");

	if(!strlen(data->description) || !strlen(data->value)){
		msg_error("You have not entered required fields!");
		return;
	}

	strncpy(val->description, data->description, sizeof(val->description));
	strncpy(val->value, data->value, sizeof(val->value));
	val->hotkey = data->vk;

	gtk_dialog_response(GTK_DIALOG(gtk_widget_get_toplevel(GTK_WIDGET(widget))), 1);
}

void winvalue_cancel_on_click(GtkButton *widget){
	gtk_dialog_response(GTK_DIALOG(gtk_widget_get_toplevel(GTK_WIDGET(widget))), 0);
}

void winvalue_hotkey_on_realize(GtkEntry *widget){
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");
	gtk_entry_set_text(widget, data->hotkey);
}

#define VK_BACKSPACE 0x108
void key_hook(unsigned int vk, gpointer data){
	GtkEntry *widget = (GtkEntry*)data;
	winvalue_data_t *wdata = g_object_get_data(GET_TOPLEVEL(widget), "tmp");

	if(vk == VK_BACKSPACE){
		wdata->hotkey[0]=0;
		wdata->vk=0;
		return;
	}

	wdata->vk = vk;
	sys_vkcode_to_string(vk, wdata->hotkey, 256);
	
	gtk_entry_set_text(widget, wdata->hotkey);
}
void winvalue_hotkey_focus_in(GtkEntry *widget){
	VariablePreset *val = g_object_get_data(GET_TOPLEVEL(widget), "value");
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");

	//Add a system hook to listen for global key events
	data->hook = sys_add_keyboard_hook(key_hook, 1, widget);
}

void winvalue_hotkey_focus_out(GtkEntry *widget){
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");
	//Remove the global hook
	sys_remove_keyboard_hook(data->hook);
}

// "Value" box {{
void winvalue_value_on_realize(GtkEntry *widget){
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");
	gtk_entry_set_text(widget, data->value);
}

void winvalue_value_focus_out(GtkEntry *widget){
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");
	strncpy(data->value, gtk_entry_get_text(widget), sizeof(data->value));
}
//}}}

void winvalue_description_on_realize(GtkEntry *widget){
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");
	gtk_entry_set_text(widget, data->description);
}

void winvalue_description_focus_out(GtkEntry *widget){
	winvalue_data_t *data = g_object_get_data(GET_TOPLEVEL(widget), "tmp");
	strncpy(data->description, gtk_entry_get_text(widget), sizeof(data->description));
}
