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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "main.h"

static void tree_view_select_under_cursor(GtkWidget *treeview, GdkEventButton *event){
	GtkTreeSelection *selection;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_count_selected_rows(selection)  <= 1)
    {
       GtkTreePath *path;

       /* Get tree path for row that was clicked */
       if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview),
                                         (gint) event->x, 
                                         (gint) event->y,
                                         &path, NULL, NULL, NULL))
       {
         gtk_tree_selection_unselect_all(selection);
         gtk_tree_selection_select_path(selection, path);
         gtk_tree_path_free(path);
       }
    }
}

void
on_menubar_activate_current            (GtkMenuShell    *menushell,
                                        gboolean         force_hide,
                                        gpointer         user_data)
{

}


void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_varview_row_activated               (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{

}


gboolean
on_win_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	winmain_quit();
  return FALSE;
}

void
on_menu_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	winmain_quit();
}


void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *about = create_win_about();
	gtk_widget_show(about);
}


gboolean
on_varview_popup_menu                  (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winmain_varlist_on_right_click(GTK_TREE_VIEW(widget), 0);
  return FALSE;
}


void
on_btn_winaddrptr_add_location_clicked (GtkButton       *button,
                                        gpointer         user_data)
{
	winaddptr_add_on_click(GTK_WIDGET(button));
}


void
on_btn_winaddptr_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	winaddptr_cancel_on_click(GTK_WIDGET(button));
}


gboolean
on_win_addptr_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  return FALSE;
}


void
on_varview_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winmain_varlist_on_realize(GTK_TREE_VIEW(widget));
}


void
on_addptr_values_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winaddptr_values_on_init(GTK_TREE_VIEW(widget));
}


void
on_addptr_type_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	winaddptr_on_type_changed(combobox);
}


gboolean
on_varview_button_press                (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	tree_view_select_under_cursor(widget, event);
	winmain_varlist_on_button_press(GTK_TREE_VIEW(widget), event);
  return FALSE;
}


void
on_winmain_menu_save_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	winmain_save();
}


void
on_winmain_menu_process_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


gboolean
on_addptr_values_button_press_event    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	tree_view_select_under_cursor(widget, event);
	if(event->button==3)
		winaddptr_values_on_right_click(GTK_TREE_VIEW(widget));
  return FALSE;
}


void
on_winmain_menu_load_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	winmain_load();
}

void
on_value_auto_update_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	winaddptr_on_auto_update_toggled(togglebutton);
}


void
on_value_type_changed                  (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	winaddptr_on_type_changed(combobox);
}


gboolean
on_addptr_location_focus_out           (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	winaddptr_on_location_focus_out(GTK_ENTRY(widget));
  return FALSE;
}


gboolean
on_addptr_description_focus_out        (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	winaddptr_on_description_changed(GTK_ENTRY(widget));
  return FALSE;
}


void
on_value_type_realize                  (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winaddptr_on_type_realize(GTK_COMBO_BOX(widget));
}


void
on_process_list_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winmain_process_list_on_realize(GTK_COMBO_BOX(widget));
}


void
on_update_proc_list_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	winmain_process_list_update();
}


void
on_menubar_load                        (GtkMenuShell    *menushell,
                                        gboolean         force_hide,
                                        gpointer         user_data)
{

}


void
on_process_list_changed                (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	winmain_process_list_on_change(combobox);
}


void
on_addptr_location_realize             (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winaddptr_on_location_realize(GTK_ENTRY(widget));
}


void
on_addptr_description_realize          (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winaddptr_on_description_realize(GTK_ENTRY(widget));
}


void
on_win_addptr_close                    (GtkDialog       *dialog,
                                        gpointer         user_data)
{
}


gboolean
on_win_addptr_destroy_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_dialog_response(GTK_DIALOG(gtk_widget_get_toplevel(widget)), 0);
  return FALSE;
}



void
on_menu_hex_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	winmain_menu_hex_on_activate(menuitem);
}


gboolean
on_value_hotkey_focus_in_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	winvalue_hotkey_focus_in(GTK_ENTRY(widget));
  return FALSE;
}


gboolean
on_value_hotkey_focus_out_event        (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	winvalue_hotkey_focus_out(GTK_ENTRY(widget));
  return FALSE;
}


gboolean
on_value_value_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	winvalue_value_focus_out(GTK_ENTRY(widget));
  return FALSE;
}


gboolean
on_value_description_focus_out_event   (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	winvalue_description_focus_out(GTK_ENTRY(widget));
  return FALSE;
}


void
on_winvalue_value_add_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	winvalue_add_on_click(button);
}


void
on_winvalue_value_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	winvalue_cancel_on_click(button);
}


void
on_value_value_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winvalue_value_on_realize(GTK_ENTRY(widget));
}


void
on_value_description_realize           (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winvalue_description_on_realize(GTK_ENTRY(widget));
}


void
on_value_hotkey_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
	winvalue_hotkey_on_realize(GTK_ENTRY(widget));
}

