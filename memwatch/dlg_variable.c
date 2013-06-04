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

/*********************
 * This dialog reads user input into a Variable
 */

// Temporary variables for each instance of this dialog
//TODO: Make all char values static
typedef struct window_data_s{
	const char *location;
	const char *type;
	const char *description;
	GtkTreeView *values;
}window_data_t;

#define GET_PROP(obj, name) \
	g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(obj))), name)

Handle_t winaddptr_values_get_selected(GtkWidget *widget){
	Variable *var = GET_PROP(widget, "variable");
	window_data_t *data = GET_PROP(widget, "tmp");

	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	Handle_t ptr;

	select = gtk_tree_view_get_selection(data->values);
	if(gtk_tree_selection_get_selected(select, &model, &iter)){
		gtk_tree_model_get (model, &iter, 0, &ptr, -1);
		return ptr;
	}
	return -1;
}

// Dialog functions {{{
/*
 * Shows a modal dialog and returns only when the dialog is dismised
 * If user has entered correct data then this data is copied into dst and this 
 * function returns true, otherwise it returns false. */
int winaddptr_do_dialog(Variable *dst){
	GtkWidget *win = create_win_addptr();
	int ret = 0;
	window_data_t *win_data = (window_data_t*)calloc(1, sizeof(window_data_t));

	assert(dst);

	// set the data
	if(strlen(dst->description)){
		win_data->description = (const char*)malloc(strlen(dst->description)+1);
		strcpy((char*)win_data->description, dst->description);
	}
	win_data->location = (const char *)malloc(256);
	VariablePath_toString(&dst->path, (char*)win_data->location, 256);
	win_data->type = DataType_toString(dst->type);

	g_object_set_data(G_OBJECT(win), "variable", dst);
	g_object_set_data(G_OBJECT(win), "tmp", win_data);
	
	ret = gtk_dialog_run(GTK_DIALOG(win));
	gtk_widget_destroy(win);

	return (ret)?TRUE:FALSE;
}
//}}}

// Button actions {{{
void winaddptr_add_on_click(GtkWidget *button){
	Variable *variable;
	window_data_t *win_data;

	win_data = (window_data_t*)g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(button)), "tmp");
	variable = (Variable*)g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(button)), "variable");

	// check if user entered all fields
	if(!win_data->location || !win_data->description || !win_data->type){
		msg_error("There are errors with your input: \n%s%s%s", 
			(!win_data->type)?"* Select type\n":"",
			(!win_data->location)?"* Enter location exception\n":"",
			(!win_data->description)?"* Enter a description\n":"");
		return;
	}

	// Reinit old variable
	//Variable_init(variable);
	if(!VariablePath_compile(&variable->path, win_data->location)){
		msg_error("Could not compile path string!");
		return;
	}
	variable->type = DataType_fromString(win_data->type);	
	strncpy(variable->description, win_data->description, sizeof(variable->description));
	// End dialog
	gtk_dialog_response(GTK_DIALOG(gtk_widget_get_toplevel(button)), 1);
}

void winaddptr_cancel_on_click(GtkWidget *button){
	gtk_dialog_response(GTK_DIALOG(gtk_widget_get_toplevel(button)), 0);
}
//}}}

// Type combo box {{{
void winaddptr_on_type_realize(GtkComboBox *type){
	window_data_t *data = (window_data_t*)GET_PROP(type, "tmp");
	int index = 0;
	int c;

	// Fill the box with available types
	for(c=0;c<DATA_TYPE_COUNT;c++){
		const char *t = DataType_toString(c);
		gtk_combo_box_append_text(type, t);
		if(strcmp(t, data->type)==0)
			index = c;
	}
	gtk_combo_box_set_active(type, index);
}

void winaddptr_on_type_changed(GtkComboBox *box){
	GObject *obj = G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(box)));
	window_data_t *data = (window_data_t*)g_object_get_data(obj, "tmp");
	
	data->type = gtk_combo_box_get_active_text(box);
}
// }}}

// Location box {{{
void winaddptr_on_location_realize(GtkEntry *box){
	window_data_t *data = GET_PROP(box, "tmp");
	gtk_entry_set_text(box, data->location);
}

void winaddptr_on_location_focus_out(GtkEntry *box){
	GObject *obj = G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(box)));
	window_data_t *data = (window_data_t*)g_object_get_data(obj, "tmp");
	data->location = gtk_entry_get_text(box);
}
//}}}

// Description box {{{
void winaddptr_on_description_realize(GtkEntry *box){
	window_data_t *data = GET_PROP(box, "tmp");
	if(data->description)
		gtk_entry_set_text(box, data->description);
}

void winaddptr_on_description_changed(GtkEntry *box){
	GObject *obj = G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(box)));
	window_data_t *data = (window_data_t*)g_object_get_data(obj, "tmp");
	data->description = gtk_entry_get_text(box);
}
//}}}

// Auto update check box {{{
void winaddptr_on_auto_update_toggled(GtkToggleButton *button){
		
}
// }}}

// Values list {{{
void winaddptr_values_on_init(GtkTreeView *treeview){
	GtkCellRenderer *rend;
	GtkTreeViewColumn *column;
	GtkListStore *model;
	GtkTreeSelection *select;
	
	struct list_column{
		const char *name;
		int 	id;
		int 	editable;
	}	list_columns[]={
		{"#", 0, 0},
		{"Hotkey", 1, 0},
		{"Value", 2, 0},
		{"Description", 3, 0}
	};

	int c;
	window_data_t *data = GET_PROP(treeview, "tmp");

	// Make sure this line is updated when you change gsListColumns
	// First one is id to be passed to profile functions
	model = gtk_list_store_new (4, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));

	for(c = 1;c<4;c++){
		rend = gtk_cell_renderer_text_new();
		if(list_columns[c].editable){
			g_object_set(G_OBJECT(rend), "editable", 1, NULL);

			//g_signal_connect(rend, "edited", G_CALLBACK(varview_value_edited), NULL);
			//g_signal_connect(rend, "editing-started", G_CALLBACK(varview_editing_started), NULL);
		}

		column = gtk_tree_view_column_new_with_attributes (
			"Author", 
			rend, 
			"text", 
			list_columns[c].id, 
			NULL);

		gtk_tree_view_column_set_title (column, list_columns[c].name);
		gtk_tree_view_append_column(treeview, column);
	}
	select = gtk_tree_view_get_selection( treeview );
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	data->values = treeview;
	winaddptr_update_list(treeview);
}

/* Adds a new variable to the list */
static void values_menu_add(GtkWidget *menu, gpointer userdata){
	VariablePreset *value;
	GtkTreeView *list = GTK_TREE_VIEW(userdata);
	Variable *var = GET_PROP(list, "variable");

	value = calloc(1, sizeof(VariablePreset));

	if(winvalue_do_dialog(value)){
		Variable_add_preset(var, value);
		winaddptr_update_list(list);
	}
}

/* Deletes the variable that user right clicked on */
static void values_menu_delete(GtkWidget *menu, gpointer userdata){
	Variable *var = GET_PROP(GTK_WIDGET(userdata), "variable");
	window_data_t *data = GET_PROP(GTK_WIDGET(userdata), "tmp");
	Handle_t selected = winaddptr_values_get_selected(GTK_WIDGET(data->values));
	if(selected!=-1){
		Variable_remove_preset(var, selected);
		winaddptr_update_list(GTK_TREE_VIEW(userdata));
	}
}

/* Show properties dialog for a variable */
static void values_menu_properties(GtkWidget *menu, gpointer data){
	Variable *var = GET_PROP(GTK_WIDGET(data), "variable");
	window_data_t *wdata = GET_PROP(GTK_WIDGET(data), "tmp");
	Handle_t sh = winaddptr_values_get_selected(GTK_WIDGET(wdata->values));
	VariablePreset *selected = Variable_get_preset(var, sh);
	if(selected){
		winvalue_do_dialog(selected);
		winaddptr_update_list(GTK_TREE_VIEW(data));
	}
}

void winaddptr_values_on_right_click(GtkTreeView *list){
	menu_item_t items[] = {
		{"Add", "gtk-add", values_menu_add, list},
		{"Delete", "gtk-remove", values_menu_delete, list},
		{"Properties", "gtk-preferences", values_menu_properties, list}
	};
	util_create_popup_menu(items, 3);
}

void foreach_value(VariablePreset *val, Handle_t handle, gpointer data){
	GtkTreeView *list = GTK_TREE_VIEW(data);
	GtkListStore *model;
	GtkTreeIter iter;
	char hotkey[256];

	// Read current globally loaded profile and update the gui
	model = GTK_LIST_STORE(gtk_tree_view_get_model(list));

	sys_vkcode_to_string(val->hotkey, hotkey, sizeof(hotkey));

	gtk_list_store_append (model, &iter);
	gtk_list_store_set (model, &iter, 0, handle, 1, hotkey, 2, val->value, 3, val->description, -1);
}

void winaddptr_update_list(GtkTreeView *list){
	Variable *var = GET_PROP(GTK_WIDGET(list), "variable");
	
	gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(list)));
	Variable_foreach_preset(var, foreach_value, list);
}
//}}}
