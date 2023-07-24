#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>

#include "TableModelColumns.h"


class MainWindow : public Gtk::Window
{
public:
    MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
    virtual ~MainWindow();

protected:
    // Glade RefPtr
    Glib::RefPtr<Gtk::Builder> builder;

    // UI Widgets
    Gtk::Notebook * pNotebook = nullptr;
    Gtk::Switch* pSwitch = nullptr;
    Gtk::Button* b_home = nullptr;
    Gtk::Button* b_rules = nullptr;
    Gtk::Button* b_status = nullptr;
    Gtk::Button* b_log = nullptr;
    Gtk::TreeView* pTreeView = nullptr;
    Gtk::ComboBoxText* pComboBoxIncomming = nullptr;
    Gtk::ComboBoxText* pComboBoxOutgoing = nullptr;
    Gtk::ComboBoxText* pComboBoxForward = nullptr;

    // Signal handlers
    void on_button_clicked_home();
    void on_button_clicked_rules();
    void on_button_clicked_status();
    void on_button_clicked_log();


    // Signal handlers Start/Stop service nftables
    bool on_switch_state_set(bool state);
private:
    void UpdateTreeViewRules();

protected:
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    TableModelColumns m_Columns;
};

#endif /* MAINWINDOW_H */

