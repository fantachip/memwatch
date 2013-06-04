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

static GtkTreeView *gsVarview=0;
static GtkComboBox *gsProcessList=0;
static GtkWidget *gsTopLevel = 0;
static Process gsProcess;
static gboolean gsHexView;
static Profile *gsProfile = 0;
static int gsQuitting = 0;

// Local types {{{
enum{
COL_ID,
COL_VALUE,
COL_DESCRIPTION,
COL_COUNT
};

// }}}

// Static functions {{{
static Handle_t get_selected_variable(){
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	Handle_t ptr;

	select = gtk_tree_view_get_selection(gsVarview);
	if(gtk_tree_selection_get_selected(select, &model, &iter)){
		gtk_tree_model_get (model, &iter, COL_ID, &ptr, -1);
		return ptr;
	}
	return -1;
}

void foreach_var_hotkey(Variable *var, Handle_t handle, gpointer c){
	if(Variable_set_preset_by_hotkey(var, (unsigned int)c))
		Process_write(&gsProcess, var);
}

// This function is called by the global hook dll for every key that is pressed system wide
static void on_key_down(unsigned int vk, void* data){
	Profile *prof;

	// This is set by winmain_quit when it's about to close the program
	if(gsQuitting)
		return;

	prof = (Profile*)g_object_get_data(G_OBJECT(gsTopLevel), "profile");
	if(!prof)
		return;
	// Check if this key is bound to any variables in the profile
	Profile_foreach_variable(prof, foreach_var_hotkey, (gpointer)vk);
}
//}}}

void winmain_init(){
	sys_add_keyboard_hook(on_key_down, 0);
}

void winmain_quit(){
	int ret = 0;

	//This is to prevent keyboard callback from crashing the program!
	gsQuitting = 1;
	
	if(gsProfile->modified){
		GtkWidget *dlg = gtk_message_dialog_new(
			GTK_WINDOW(gWinAddPointer), 
			GTK_DIALOG_DESTROY_WITH_PARENT, 
			GTK_MESSAGE_QUESTION, 
			GTK_BUTTONS_YES_NO, "Save profile before exiting?");
		ret = gtk_dialog_run(GTK_DIALOG(dlg));
		gtk_widget_destroy(dlg);
	}
	if(ret == GTK_RESPONSE_YES){
		if(!winmain_save()){
			gsQuitting=0;
			return;
		}
	}
	else if(ret == GTK_RESPONSE_CANCEL)
		return;
	gtk_main_quit();
}

// Main menu actions {{{
int winmain_save(){
	GtkWidget *dialog;
	Profile *prof = g_object_get_data(G_OBJECT(gsTopLevel), "profile");

	dialog = gtk_file_chooser_dialog_new ("Save File",
						GTK_WINDOW(gsTopLevel),
						GTK_FILE_CHOOSER_ACTION_SAVE,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
						NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		Profile_save(prof, filename);
		g_free (filename);
		gtk_widget_destroy (dialog);
		return 1;
	}
	gtk_widget_destroy (dialog);
	return 0;
}

void winmain_load(){
	GtkWidget *dialog;
	Profile *prof = g_object_get_data(G_OBJECT(gsTopLevel), "profile");

	dialog = gtk_file_chooser_dialog_new ("Open File",
						GTK_WINDOW(gsTopLevel),
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		Profile_load(prof, filename);
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
	winmain_update_list();
	winmain_process_list_update();
}
void winmain_menu_hex_on_activate(GtkMenuItem *item){
	gsHexView = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item));
}
// }}}

// Context menu functions {{{

/* Adds a new variable to the list */

static void varlist_menu_set_value(GtkWidget *menu, gpointer userdata){
	VariablePreset *val=(VariablePreset*)userdata;
	Variable *var = Profile_get_variable(gsProfile, get_selected_variable());
	
	strncpy(var->value, val->value, sizeof(var->value));
	Process_write(&gsProcess, var);
}

static void varlist_menu_add(GtkWidget *menu, gpointer userdata){
	Variable var;
	Profile *profile;

	memset(&var, 0, sizeof(Variable));

	if(winaddptr_do_dialog(&var)){
		// add the variable to the profile and to the list
		profile = g_object_get_data(G_OBJECT(gsTopLevel), "profile");
		Profile_add_variable(profile, &var);
		winmain_update_list();
	}
}

/* Deletes the variable that user right clicked on */
static void varlist_menu_delete(GtkWidget *menu, gpointer userdata){
	Profile *prof = (Profile*)g_object_get_data(G_OBJECT(gsTopLevel), "profile");
	Handle_t handle = get_selected_variable();

	if(handle==-1)
		return;

	Profile_remove_variable(prof, handle);
	winmain_update_list();
}

/* Show properties dialog for a variable */
static void varlist_menu_properties(GtkWidget *menu, gpointer data){
	Variable *ptr = Profile_get_variable(gsProfile, get_selected_variable());

	if(ptr){
		winaddptr_do_dialog(ptr);
		winmain_update_list();
		gsProfile->modified=1;
	}
}

//}}}

// List view functions {{{

//***************
//The first column of the list view holds a pointer to the variable it represents
//***************

static void varview_editing_started(GtkCellRendererText *rend, GtkEditable *box, gchar *path, gpointer data){
	Profile_get_variable(gsProfile, get_selected_variable())->read_only=1;
}

/*This is called every time user edits a value in the "Value" column*/
static void varview_value_edited(GtkCellRendererText *rend, char *path, char *new_text, gpointer data){
	Variable *ptr = Profile_get_variable(gsProfile, get_selected_variable());
	ptr->read_only=0;

	strncpy(ptr->value, new_text, sizeof(ptr->value));
	if(!Process_write(&gsProcess, ptr)){
		msg_error("Could not set the value you specified!");
	}
	
}

/*Initialize the varlist*/
void winmain_varlist_on_realize(GtkTreeView *treeview){
	GtkCellRenderer *rend;
	GtkTreeViewColumn *column;
	GtkListStore *model;
	GtkTreeModelSort *sorter;
	GtkTreeSelection *select;
	
	struct list_column{
		const char *name;
		int 	id;
		int 	editable;
	}	list_columns[]={
		{"#", COL_ID, 0},
		{"Value", COL_VALUE, 1},
		{"Description", COL_DESCRIPTION, 0}
	};

	//GtkTreeIter iter;
	int c;

	gsVarview = treeview;
	gsTopLevel = gtk_widget_get_toplevel(GTK_WIDGET(treeview));		
	gsProfile = (Profile*)g_object_get_data(G_OBJECT(gsTopLevel), "profile");

	// Make sure this line is updated when you change gsListColumns
	// First one is id to be passed to profile functions
	model = gtk_list_store_new (3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING);
	sorter = gtk_tree_model_sort_new_with_model(model);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(sorter),
                                             2, GTK_SORT_ASCENDING);

	gtk_tree_view_set_model(treeview, sorter);

	for(c = 1;c<COL_COUNT;c++){
		rend = gtk_cell_renderer_text_new();
		if(list_columns[c].editable){
			g_object_set(G_OBJECT(rend), "editable", 1, NULL);

			g_signal_connect(rend, "edited", G_CALLBACK(varview_value_edited), NULL);
			g_signal_connect(rend, "editing-started", G_CALLBACK(varview_editing_started), NULL);
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
}

/*Called upon right clicks inside the list. Pops a context menu*/
void winmain_varlist_on_right_click(GtkTreeView *list, GdkEventButton *event){
	Handle_t selected = get_selected_variable();
	Variable *var = 0;
	VariablePreset *values[VARIABLE_MAX_PRESETS];
	int count = 0, c=0;
	menu_item_t *items;

	menu_item_t def_items[]={
		{"Add", "gtk-add", varlist_menu_add},
		{"Delete", "gtk-remove", varlist_menu_delete},
		{"Properties", "gtk-preferences", varlist_menu_properties}
	};
	if(selected!=-1)
		var = Profile_get_variable(gsProfile, selected);
	
	if(var){
		for(c=0;c<VARIABLE_MAX_PRESETS;c++){
			if(var->presets[c])
				values[count++]=var->presets[c];
		}
		items = calloc(1, sizeof(menu_item_t)*(count+3));
		for(c=0;c<count;c++){
			items[c].name=values[c]->description;
			items[c].icon="";
			items[c].callback=varlist_menu_set_value;
			items[c].data=values[c];
		}
	}
	else
		items = calloc(1, sizeof(menu_item_t)*3);
	for(c=0;c<3;c++){
		items[count+c].name=def_items[c].name;
		items[count+c].icon=def_items[c].icon;
		items[count+c].callback=def_items[c].callback;
	}
	util_create_popup_menu(items, count+3);
	free(items);
}

void winmain_varlist_on_button_press(GtkTreeView *view, GdkEventButton *event){
	if(event->button == 3)
		winmain_varlist_on_right_click(view, event);
}


// }}}

// Process list {{{

struct add_process_arg{
	GtkComboBox *dropdown;
	Profile *profile;
	int c;
	int index;
};

static void add_process_list(Process *proc, void *data) {
	struct add_process_arg *arg = ((struct add_process_arg*)data);
	gtk_combo_box_append_text(arg->dropdown, proc->executable);
	if(arg->profile && _stricmp(proc->executable, arg->profile->process_name)==0)
		arg->index = arg->c;
	arg->c++;
}
void winmain_process_list_on_realize(GtkComboBox *box){
	gsProcessList = GTK_COMBO_BOX(box);
	winmain_process_list_update();
}

void winmain_process_list_on_change(GtkComboBox *box){
	Profile *prof = g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(gsVarview))), "profile");

	// Update the profile variable
	if(Process_open(&gsProcess, gtk_combo_box_get_active_text(box))){
		strncpy(prof->process_name, gsProcess.executable, sizeof(prof->process_name));
	}
}

void winmain_process_list_update(){
	struct add_process_arg data;
	Profile *prof = g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(gsVarview))), "profile");

	memset(&data, 0, sizeof(data));
	data.profile = prof;
	data.dropdown = gsProcessList;
	data.index = -1;

	// Populate the process list
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(gsProcessList)));
	sys_foreach_process(add_process_list, &data);
	if(data.index>=0)
		gtk_combo_box_set_active(gsProcessList, data.index);
} 
// }}}

// WinMain functions {{{

static void foreach_variable(const Variable *var, Handle_t handle, gpointer data){
	GtkListStore *model;
	GtkTreeIter iter;

	// Read current globally loaded profile and update the gui
	model = GTK_LIST_STORE(data);

	gtk_list_store_append (model, &iter);
	gtk_list_store_set (model, &iter, 0, handle, 1, "0", 2, var->description, -1);
}

/* Update the gui with the state of current profile */
void winmain_update_list(){
	GtkListStore *model;
	Profile *prof = g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(gsVarview))), "profile");

	model = GTK_LIST_STORE(gtk_tree_model_sort_get_model(gtk_tree_view_get_model(gsVarview)));
	
	gtk_list_store_clear(model);
	
	Profile_foreach_variable(prof, foreach_variable, model);	
}

static gboolean foreach_row(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data){
	Handle_t handle;
	Variable *var;
	char string[256];

	gtk_tree_model_get (model, iter, COL_ID, &handle, -1);
	var = Profile_get_variable(gsProfile, handle);

	if(var && var->read_only)
		return FALSE;

	// Get process information from the main window
	//TODO	

	// Read current value of the variable from the foregn process
	if(!Process_read(&gsProcess, var, gsHexView))
		strcpy(string, "##");
	else
		strncpy(string, var->value, sizeof(string));

	// Update the column value
	gtk_list_store_set(GTK_LIST_STORE(model), iter, COL_VALUE, string, -1);
	return FALSE;
}

void winmain_update_data(){
	GtkTreeModel *model;
	GtkTreeModelSort *sorter;

	sorter = gtk_tree_view_get_model(gsVarview);
	model = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT (sorter));

	gtk_tree_model_foreach(model, foreach_row, NULL);
}
//}}}

