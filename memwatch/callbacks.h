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


#include <gtk/gtk.h>


void
on_menubar_activate_current            (GtkMenuShell    *menushell,
                                        gboolean         force_hide,
                                        gpointer         user_data);

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_varview_row_activated               (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

gboolean
on_win_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_add_location_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_varview_popup_menu                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_btn_winaddrptr_add_location_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_btn_winaddptr_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_win_addptr_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_varview_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_addptr_values_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_addptr_type_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);


gboolean
on_varview_button_press                (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_winmain_menu_save_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_winmain_menu_process_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_addptr_values_button_press_event    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_winmain_menu_load_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_addptr_location_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_addptr_description_changed          (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_value_auto_update_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_value_type_changed                  (GtkComboBox     *combobox,
                                        gpointer         user_data);

gboolean
on_addptr_location_focus_out           (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_addptr_description_focus_out        (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_value_type_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_process_list_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_update_proc_list_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_menubar_load                        (GtkMenuShell    *menushell,
                                        gboolean         force_hide,
                                        gpointer         user_data);

void
on_process_list_changed                (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_addptr_location_realize             (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_addptr_description_realize          (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_win_addptr_close                    (GtkDialog       *dialog,
                                        gpointer         user_data);

gboolean
on_win_addptr_destroy_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_win_addptr_destroy_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_menu_hex_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_value_hotkey_focus_in_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_value_hotkey_focus_out_event        (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_value_value_focus_out_event         (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_value_description_focus_out_event   (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_winvalue_value_add_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_winvalue_value_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data);


void
on_value_value_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_value_description_realize           (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_value_hotkey_realize                (GtkWidget       *widget,
                                        gpointer         user_data);
