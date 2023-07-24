#ifndef TABLEMODELCOLUMNS_H
#define TABLEMODELCOLUMNS_H

#include <gtkmm.h>


class TableModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    TableModelColumns();
public:
    Gtk::TreeModelColumn<Glib::ustring> m_col_target;
    Gtk::TreeModelColumn<Glib::ustring> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_action;
    Gtk::TreeModelColumn<Glib::ustring> m_col_interface;
    Gtk::TreeModelColumn<Glib::ustring> m_col_ip;
    Gtk::TreeModelColumn<Glib::ustring> m_col_protocol;
    Gtk::TreeModelColumn<Glib::ustring> m_col_port;
    Gtk::TreeModelColumn<Glib::ustring> m_col_comment;
};

#endif /* TABLEMODELCOLUMNS_H */

